#pragma once

#include "common.h"

#include "display.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <EGL/egl.h>

namespace c4g {

namespace utility{
class DynamicLibraryManager;
}

namespace display {
namespace linux_gles {

class CDisplay: public core::IDisplay
{
public:
  CDisplay(const base::Config& roConfig);
  virtual ~CDisplay();

public:
  virtual void Run();

protected:
  void CreateWindow();
  void DestroyWindow();

private:
  Display* m_pDisplay;
  Window m_lWindow;

  EGLConfig m_pGLConfig;
  EGLDisplay m_pGLDisplay;
  EGLContext m_pGLContext;
  EGLSurface m_pGLSurface;

  bool m_bIsRunning;

  std::string m_sTitle;
  int m_iWidth;
  int m_iHeight;

  base::Config          m_oConfigRender;
  core::IRender*        m_pRender;
  utility::DynamicLibraryManager* m_pLibraryManager;
};

}
}
}
