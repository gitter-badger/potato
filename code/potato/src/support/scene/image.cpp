#include "image.h"

#include "scene.h"
#include "effect.h"

#include "utility/log.h"

#include <cassert>

namespace c4g {
namespace scene {

CImage::CBuilder CImage::builder;

CImage::CImage(ISceneWithScript* const& rpScene, IWidget* const& rpParent)
  : TWidget<IImage>(rpScene, rpParent)
  , m_pEffect(NULL)
{
  //m_pSubstance->Compile("void fun1() { log_info(\"hello world\"); }");

  m_pEffect = new CEffect();
}

CImage::~CImage()
{
  delete m_pEffect;
  m_pEffect = NULL;
}

void CImage::Resize(const float& rfWidth, const float& rfHeight)
{
  ;
}

// for script
typedef bool (*script_tick)(float fDelta);
bool script_tick_default(float fDelta)
{
  //utility::Log::Instance().Warning("missing 'tick' function in script");
  return false;
}

bool CImage::Tick(const float& rfDelta)
{
  // call script
  bool res = CallScript<script_tick>("tick", script_tick_default)(rfDelta);

  return res || m_pEffect->Tick(rfDelta);
}

// script
typedef void (*script_draw)(int iLayer);
void script_draw_default(int iLayer)
{
  //utility::Log::Instance().Warning("missing 'draw' function in script");
}

void CImage::Draw(const int& riLayer, render::ICanvas* const & rpCanvas)
{
  // call script
  CallScript<script_draw>("draw", script_draw_default)(riLayer);

  m_pEffect->SetPos(dst.l, dst.t);
  rpCanvas->DrawGlyph(src, dst.w, dst.h, m_pEffect);
  //
}

bool CImage::Handle(const int& riLayer, const display::IInput* const & rpInput)
{
  //
  return true;
}

bool CImage::CBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, CImage* const & rpImage) const
{
  const rapidjson::Value& jvalue = roConfig["value"];
  assert(jvalue.IsObject());
  if (!jvalue.IsObject()) return false;

  if (!CWidgetBuilder::instance.Do(rpAsset, jvalue, rpImage))
  {
    return false;
  }

  const rapidjson::Value& jsrc = jvalue["src"];
  assert(jsrc.IsObject());
  if (!jsrc.IsObject()) return false;

  if (!CGlyphBuilder::instance.Do(rpAsset, jsrc, rpImage->src))
  {
    return false;
  }

  if (jvalue.HasMember("script"))
  {
    const rapidjson::Value& jscript = jvalue["script"];
    CScriptBuilder::instance.Do(rpAsset, jscript, rpImage->m_pSubstance);
  }

  return true;
}

}
}
