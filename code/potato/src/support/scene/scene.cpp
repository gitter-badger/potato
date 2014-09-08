#include <rapidjson/document.h>

#include "scene_impl.h"
#include "render.h"
#include "asset.h"

#include "utility/file.h"
#include "utility/log.h"
#include "utility/sharedlibrary.h"

#include <cassert>

FUNC_API_TYPEDEF(CreateAsset, c4g::core::IAsset, const c4g::base::Config);
FUNC_API_TYPEDEF(DestroyAsset, c4g::core::IAsset, const c4g::base::Config);

namespace c4g {
namespace scene {

static unsigned char g_aiTexArray[4 * 4] = {
    0xFF, 0x00, 0x00, 0x38,
    0x00, 0xFF, 0x00, 0x38,
    0x00, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};
static render::Glyph g_Glyph;

class CProcess : public render::IProcess
{
public:
  CProcess() : angle(0.0f) { ; }
  virtual ~CProcess() { ; }

public:
  virtual void Begin(const render::Glyph& rGlyph) { ; }
  virtual bool Do(render::ITransform* const& rpTransform)
  {
    // keep on the 3d transform
    rpTransform->Translate(100.0f, 100.0f);
    rpTransform->Rotate(angle, 0.0f, 0.0f, 1.0f, 150.0f, 150.0f, 0.0f);
    return true;
  }
  virtual void End() { ; }

public:
  bool Tick(const float& rfDelta)
  {
    angle += rfDelta * 100.0f;
    if (angle > 360.0f) angle-= 360.0f;
    return true;
  }

private:
  float angle;
};
static CProcess g_process;

CScene::CScene(const base::Config& roConfig)
  : m_pAsset(NULL)
{
  C4G_LOG_INFO(__PRETTY_FUNCTION__);

  m_pLibraryManager = new utility::CSharedLibraryManager();

  std::string file_context = utility::ReadFile(roConfig.GetConfigureFile());

  rapidjson::Document jdoc;
  jdoc.Parse(file_context.c_str());
  assert(jdoc.IsObject());
  const rapidjson::Value& render = jdoc["asset"];
  assert(render.IsObject());
  const rapidjson::Value& library = render["library"];
  assert(library.IsObject());
  const rapidjson::Value& library_file = library["file"];
  assert(library_file.IsString());
  const rapidjson::Value& configure = render["configure"];
  assert(configure.IsObject());
  const rapidjson::Value& configure_file = configure["file"];
  assert(configure_file.IsString());

  m_oConfigAsset._sLibrPath = roConfig._sLibrPath;
  m_oConfigAsset._sDataPath = roConfig._sDataPath;
  m_oConfigAsset._sLibraryFile = library_file.GetString();
  m_oConfigAsset._sConfigureFile = configure_file.GetString();

  /// load the shared library
  typedef FUNC_API_TYPE(CreateAsset) CreateAssetFuncPtr;
  CreateAssetFuncPtr func_create_func_ptr = m_pLibraryManager->GetFunc<CreateAssetFuncPtr>(m_oConfigAsset.GetLibraryFile(), TOSTRING(CreateAsset));
  /// create the display with configure
  func_create_func_ptr(m_pAsset, m_oConfigAsset);
}

CScene::~CScene()
{
  /// load the shared library
  typedef FUNC_API_TYPE(DestroyAsset) DestroyAssetFuncPtr;
  DestroyAssetFuncPtr func_destroy_func_ptr = m_pLibraryManager->GetFunc<DestroyAssetFuncPtr>(m_oConfigAsset.GetLibraryFile(), TOSTRING(DestroyAsset));
  /// create the display with configure
  func_destroy_func_ptr(m_pAsset, m_oConfigAsset);

  delete m_pLibraryManager;
  m_pLibraryManager = NULL;

  C4G_LOG_INFO(__PRETTY_FUNCTION__);
}

bool CScene::Load(core::IRender* const& rpRender, const std::string& rsFileName)
{
  C4G_LOG_INFO(__PRETTY_FUNCTION__);
  g_Glyph.l = 0.0f;
  g_Glyph.r = 5.0f;
  g_Glyph.t = 0.0f;
  g_Glyph.b = 5.0f;
  g_Glyph.id = rpRender->GenerateTexId(2, 2, g_aiTexArray);
  return true;
}

bool CScene::Unload(core::IRender* const& rpRender)
{
  rpRender->DeleteTexId(1, &g_Glyph.id);
  C4G_LOG_INFO(__PRETTY_FUNCTION__);
  return true;
}

bool CScene::Resize(const int& riWidth, const int& riHeight)
{
  C4G_LOG_INFO(__PRETTY_FUNCTION__);
  return true;
}

bool CScene::Handle(const display::IInput* const pInput)
{
  return true;
}

bool CScene::Tick(const float& rfDelta)
{
  g_process.Tick(rfDelta);
  return true;
}

bool CScene::Draw(render::ICanvas* const& rpCanvas)
{
  rpCanvas->DrawGlyph(g_Glyph, 300, 300, &g_process);
  return true;
}

}
}

bool CreateScene(c4g::core::IScene*& rpScene, const c4g::base::Config& roConfig)
{
  assert(rpScene == NULL);
  if (NULL != rpScene) return false;
  rpScene = new c4g::scene::CScene(roConfig);
  return true;
}

bool DestroyScene(c4g::core::IScene*& rpScene, const c4g::base::Config& roConfig)
{
  assert(rpScene != NULL);
  if (NULL == rpScene) return false;
  delete rpScene;
  rpScene = NULL;
  return true;
}
