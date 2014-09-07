#include <rapidjson/document.h>

#include "display_linux_gl.h"

#include "render.h"
#include "scene.h"

#include "utility/file.h"
#include "utility/sharedlibrary.h"
#include "utility/log.h"

#include <cassert>
#include <unistd.h>
#include <sys/time.h>
#include <GL/glx.h>

FUNC_API_TYPEDEF(CreateRender, c4g::core::IRender, const c4g::base::Config);
FUNC_API_TYPEDEF(DestroyRender, c4g::core::IRender, const c4g::base::Config);

namespace c4g {
namespace display {
namespace linux_gl {

CDisplay::CDisplay(const base::Config& roConfig)
  : m_pDisplay(NULL)
  , m_lWindow(0)
  , m_pGLContext(NULL)
  , m_bIsRunning(true)
  , m_iWidth(0)
  , m_iHeight(0)
  , m_pRender(NULL)
  , m_pLibraryManager(NULL)
{
  utility::Log::Instance().Info(__PRETTY_FUNCTION__);

  std::string file_context = utility::ReadFile(roConfig.GetConfigureFile());

  rapidjson::Document jdoc;
  jdoc.Parse(file_context.c_str());
  assert(jdoc.IsObject());
  const rapidjson::Value& jtitle = jdoc["title"];
  assert(jtitle.IsString());
  const rapidjson::Value& jsize = jdoc["size"];
  assert(jsize.IsObject());
  const rapidjson::Value& jwidth = jsize["width"];
  assert(jwidth.IsInt());
  const rapidjson::Value& jheight = jsize["height"];
  assert(jheight.IsInt());

  const rapidjson::Value& render = jdoc["render"];
  assert(render.IsObject());
  const rapidjson::Value& library = render["library"];
  assert(library.IsObject());
  const rapidjson::Value& library_file = library["file"];
  assert(library_file.IsString());
  const rapidjson::Value& configure = render["configure"];
  assert(configure.IsObject());
  const rapidjson::Value& configure_file = configure["file"];
  assert(configure_file.IsString());

  m_sTitle = jtitle.GetString();
  m_iWidth = jwidth.GetInt();
  m_iHeight = jheight.GetInt();

  m_oConfigRender._sLibrPath = roConfig._sLibrPath;
  m_oConfigRender._sDataPath = roConfig._sDataPath;
  m_oConfigRender._sLibraryFile = library_file.GetString();
  m_oConfigRender._sConfigureFile = configure_file.GetString();

  /// load the shared library
  typedef FUNC_API_TYPE(CreateRender) CreateRenderFuncPtr;
  CreateRenderFuncPtr func_create_func_ptr = m_pLibraryManager->GetFunc<CreateRenderFuncPtr>(m_oConfigRender.GetLibraryFile(), TOSTRING(CreateRender));
  /// create the display with configure
  func_create_func_ptr(m_pRender, m_oConfigRender);
}

CDisplay::~CDisplay()
{
  /// load the shared library
  typedef FUNC_API_TYPE(DestroyRender) DestroyRenderFuncPtr;
  DestroyRenderFuncPtr func_destroy_func_ptr = m_pLibraryManager->GetFunc<DestroyRenderFuncPtr>(m_oConfigRender.GetLibraryFile(), TOSTRING(DestroyRender));
  /// create the display with configure
  func_destroy_func_ptr(m_pRender, m_oConfigRender);

  delete m_pLibraryManager;
  m_pLibraryManager = NULL;

  utility::Log::Instance().Info(__PRETTY_FUNCTION__);
}

void CDisplay::Run(core::IScene* const& rpScene)
{
  CreateWindow();

  m_pRender->Start();
  m_pRender->Resize(m_iWidth, m_iHeight);

  //TODO:
  rpScene->Load(m_pRender, "");
  rpScene->Resize(m_iWidth, m_iHeight);

  timeval time;
  gettimeofday(&time, NULL);
  double second = time.tv_sec * 1.0 + time.tv_usec / 1000000.0;
  double second_temp = 0.0;
  double second_delta = 0.0;
  double second_sleep = 0.0;
  double second_per_frame_min = 1.0 / 60.0;
  //int count = 0;

  while (m_bIsRunning)
  {
    while (XPending(m_pDisplay) > 0)
    {
      static XEvent event;
      XNextEvent(m_pDisplay, &event);
      switch (event.type)
      {
      /*case Expose:
       if (event.xexpose.count != 0)
       {
       break;
       }
       if (m_pRender->Tick(second_delta))
       {
       glXSwapBuffers(m_pDisplay, m_lWindow);
       }
       break;*/

      case ConfigureNotify:
        if (event.xconfigure.width != m_iWidth || event.xconfigure.height != m_iHeight)
        {
          m_iWidth = event.xconfigure.width;
          m_iHeight = event.xconfigure.height;
          m_pRender->Resize(m_iWidth, m_iHeight);
          rpScene->Resize(m_iWidth, m_iHeight);
        }
        break;

      case ButtonRelease:
        break;

      case KeyRelease:
        if (XK_Escape == XLookupKeysym(&event.xkey, 0))
        {
          m_bIsRunning = false;
        }
        break;
      }
    }

    gettimeofday(&time, NULL);
    second_temp = second;
    second = time.tv_sec * 1.0 + time.tv_usec / 1000000.0;
    second_delta = second - second_temp;
    if (m_pRender->Render(static_cast<float>(second_delta), rpScene))
    {
      glXSwapBuffers(m_pDisplay, m_lWindow);
    }

    gettimeofday(&time, NULL);
    second_sleep = second_per_frame_min - (time.tv_sec * 1.0 + time.tv_usec / 1000000.0 - second);
    if (0.001 < second_sleep)
    {
      //printf("%d time: s-%f | temp-%f | delta-%f | spf-%f | sleep-%f\n", ++count, second, second_temp, second_delta, second_per_frame_min, second_sleep);
      usleep(static_cast<__useconds_t >(second_sleep * 1000000));
    }
  }

  rpScene->Unload(m_pRender);
  m_pRender->End();
  DestroyWindow();
}

void CDisplay::CreateWindow()
{
  m_pDisplay = XOpenDisplay(NULL);
  assert(NULL != m_pDisplay);
  int screen_id = DefaultScreen(m_pDisplay);

  static int attrListDbl[] = {
  GLX_RGBA, GLX_DOUBLEBUFFER,
  GLX_RED_SIZE, 4,
  GLX_GREEN_SIZE, 4,
  GLX_BLUE_SIZE, 4,
  GLX_DEPTH_SIZE, 16,
  None };
  XVisualInfo* visual_info_ptr = glXChooseVisual(m_pDisplay, screen_id, attrListDbl);
  assert(NULL != visual_info_ptr);

  m_pGLContext = glXCreateContext(m_pDisplay, visual_info_ptr, 0, GL_TRUE);
  assert(NULL != m_pGLContext);

  XSetWindowAttributes attr;
  attr.colormap = XCreateColormap(m_pDisplay, RootWindow(m_pDisplay, visual_info_ptr->screen), visual_info_ptr->visual,
  AllocNone);
  attr.border_pixel = 0;
  attr.event_mask = ExposureMask | KeyReleaseMask | ButtonReleaseMask | StructureNotifyMask;

  m_lWindow = XCreateWindow(m_pDisplay, RootWindow(m_pDisplay, visual_info_ptr->screen), 0, 0, m_iWidth, m_iHeight, 0, visual_info_ptr->depth, InputOutput, visual_info_ptr->visual,
  CWBorderPixel | CWColormap | CWEventMask, &attr);
  XMapWindow(m_pDisplay, m_lWindow);
  XFree(visual_info_ptr);

  glXMakeCurrent(m_pDisplay, m_lWindow, m_pGLContext);
}

void CDisplay::DestroyWindow()
{
  glXMakeCurrent(m_pDisplay, None, NULL);
  glXDestroyContext(m_pDisplay, m_pGLContext);
  XDestroyWindow(m_pDisplay, m_lWindow);
  XCloseDisplay(m_pDisplay);
}

}
}
}

bool CreateDisplay(c4g::core::IDisplay*& rpDisplay, const c4g::base::Config& roConfig)
{
  assert(rpDisplay == NULL);
  rpDisplay = new c4g::display::linux_gl::CDisplay(roConfig);
  return true;
}

bool DestroyDisplay(c4g::core::IDisplay*& rpDisplay, const c4g::base::Config& roConfig)
{
  assert(rpDisplay != NULL);
  delete rpDisplay;
  rpDisplay = NULL;
  return true;
}
