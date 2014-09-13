#include "builder.h"

#include "scene_base.h"
#include "asset.h"
#include "panel.h"
#include "image.h"

namespace c4g {
namespace scene {

// construct the static instance about builder
CBuilderManager CBuilderManager::instance;
CGlyphBuilder CGlyphBuilder::instance;
CRectFBuilder CRectFBuilder::instance;
CWidgetBuilder CWidgetBuilder::instance;
CFileBuilder CFileBuilder::instance;
CAssetsBuilder CAssetsBuilder::instance;
CAllWidgetBuilder CAllWidgetBuilder::instance;
CScriptBuilder CScriptBuilder::instance;



IBuilder::IBuilder(const std::string& rsName)
  : name(rsName)
{
  // register self to the manager
  CBuilderManager::instance.Set(rsName, this);
}



CGlyphBuilder::CGlyphBuilder()
  : TBuilder<render::Glyph>("glyph")
{
  ;
}

bool CGlyphBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, render::Glyph& rGlyph) const
{
  const rapidjson::Value& jleft = roConfig["l"];
  assert(jleft.IsDouble());
  if (!jleft.IsDouble()) return false;
  const rapidjson::Value& jtop = roConfig["t"];
  assert(jtop.IsDouble());
  if (!jtop.IsDouble()) return false;
  const rapidjson::Value& jright = roConfig["r"];
  assert(jright.IsDouble());
  if (!jright.IsDouble()) return false;
  const rapidjson::Value& jbottom = roConfig["b"];
  assert(jbottom.IsDouble());
  if (!jbottom.IsDouble()) return false;
  const rapidjson::Value& jid = roConfig["id"];
  assert(jid.IsString());
  if (!jid.IsString()) return false;

  int width = 0;
  int height = 0;
  unsigned int id = 0;
  if (!rpAsset->FindImageInfo(jid.GetString(), width, height, id)) return false;

  rGlyph.l = static_cast<float>(jleft.GetDouble() / (width * 1.0f));
  rGlyph.r = static_cast<float>(jright.GetDouble() / (width * 1.0f));
  rGlyph.t = static_cast<float>(jtop.GetDouble() / (height * 1.0f));
  rGlyph.b = static_cast<float>(jbottom.GetDouble() / (height * 1.0f));
  rGlyph.id = id;
  return true;
}



CRectFBuilder::CRectFBuilder()
  : TBuilder<RectF>("rectf")
{
  ;
}

bool CRectFBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, RectF& rRectF) const
{
  const rapidjson::Value& jleft = roConfig["l"];
  assert(jleft.IsDouble());
  if (!jleft.IsDouble()) return false;
  const rapidjson::Value& jtop = roConfig["t"];
  assert(jtop.IsDouble());
  if (!jtop.IsDouble()) return false;
  const rapidjson::Value& jright = roConfig["r"];
  assert(jright.IsDouble());
  if (!jright.IsDouble()) return false;
  const rapidjson::Value& jbottom = roConfig["b"];
  assert(jbottom.IsDouble());
  if (!jbottom.IsDouble()) return false;

  rRectF.l = static_cast<float>(jleft.GetDouble());
  rRectF.t = static_cast<float>(jtop.GetDouble());
  rRectF.r = static_cast<float>(jright.GetDouble());
  rRectF.b = static_cast<float>(jbottom.GetDouble());
  rRectF();
  return true;
}



CWidgetBuilder::CWidgetBuilder()
  : TBuilder<IWidget* const>("widget")
{
  ;
}

bool CWidgetBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, IWidget* const& rpWidget) const
{
  const rapidjson::Value& jid = roConfig["id"];
  assert(jid.IsString());
  if (!jid.IsString()) return false;
  const rapidjson::Value& jrect = roConfig["dst"];
  assert(jrect.IsObject());
  if (!jrect.IsObject()) return false;
  const rapidjson::Value& jlayer = roConfig["layer"];
  assert(jlayer.IsInt());
  if (!jlayer.IsInt()) return false;
  const rapidjson::Value& jvisible = roConfig["visible"];
  assert(jvisible.IsBool());
  if (!jvisible.IsBool()) return false;

  rpWidget->id = jid.GetString();
  CRectFBuilder::instance.Do(rpAsset, jrect, rpWidget->dst);
  rpWidget->dst_config = rpWidget->dst;
  rpWidget->layer = jlayer.GetInt();
  rpWidget->visible = jvisible.GetBool();
  return true;
}



CFileBuilder::CFileBuilder()
  : TBuilder<rapidjson::Document>("file")
{
  ;
}

bool CFileBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, rapidjson::Document& rDoc) const
{
  const rapidjson::Value& jvalue = roConfig["value"];
  assert(jvalue.IsString());
  if (!jvalue.IsString()) return false;

  std::string file_context;
  rpAsset->LoadFile(jvalue.GetString(), file_context);
  rDoc.Parse(file_context.c_str());
  return true;
}



CAssetsBuilder::CAssetsBuilder()
  : TBuilder<const void* const>("asset")
  , m_pRender(NULL)
{
  ;
}

void CAssetsBuilder::BindRender(core::IRender* const& rpRender)
{
  m_pRender = rpRender;
}

bool CAssetsBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, const void* const& rpPtr) const
{
  assert(roConfig.IsArray());
  if (!roConfig.IsArray()) return false;

  for (int i = 0; i < static_cast<int>(roConfig.Size()); ++i)
  {
    const rapidjson::Value& jasset = roConfig[i];
    assert(jasset.IsObject());
    if (!jasset.IsObject()) continue;
    const rapidjson::Value& jtype = jasset["type"];
    assert(jtype.IsString());
    const rapidjson::Value& jvalue = jasset["value"];
    assert(jvalue.IsObject());

    std::string type = jtype.GetString();
    if (type == "image")
    {
      if (!jtype.IsString()) continue;
      const rapidjson::Value& jid = jvalue["id"];
      assert(jid.IsString());
      if (!jid.IsString()) continue;
      const rapidjson::Value& jfile = jvalue["file"];
      assert(jfile.IsString());
      if (!jfile.IsString()) continue;

      int width = 0;
      int height = 0;
      unsigned char* buffer_ptr = NULL;
      rpAsset->LoadImage(jfile.GetString(), width, height, buffer_ptr);
      if (NULL == m_pRender) continue;
      int texid = m_pRender->GenerateTexId(width, height, buffer_ptr);
      rpAsset->PushImageInfo(jid.GetString(), width, height, texid);
    }
  }

  return true;
}



CAllWidgetBuilder::CAllWidgetBuilder()
  : TBuilder<IWidget* const>("all")
{
  ;
}

bool CAllWidgetBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, IWidget* const& rpWidget) const
{
  const rapidjson::Value& jtype = roConfig["type"];
  assert(jtype.IsString());
  if (!jtype.IsString()) return false;

  const IBuilder* builder_ptr = CBuilderManager::instance.Get(jtype.GetString());
  if (NULL == builder_ptr) return false;

  if (builder_ptr->name == "panel")
  {
    // TODO: where to delete?
    IPanel* new_panel_ptr = new CPanel(rpWidget->scene, rpWidget);
    const TBuilder<IPanel* const>* panel_builder_ptr = reinterpret_cast<const TBuilder<IPanel* const>*>(builder_ptr);
    if (panel_builder_ptr->Do(rpAsset, roConfig, new_panel_ptr))
    {
      rpWidget->Add(new_panel_ptr);
      return true;
    }
    delete new_panel_ptr;
  }
  else if (builder_ptr->name == "image")
  {
    // TODO: where to delete?
    IImage* new_image_ptr = new CImage(rpWidget->scene, rpWidget);
    const TBuilder<IImage* const>* image_builder_ptr = reinterpret_cast<const TBuilder<IImage* const>*>(builder_ptr);
    if (image_builder_ptr->Do(rpAsset, roConfig, new_image_ptr))
    {
      rpWidget->Add(new_image_ptr);
      return true;
    }
    delete new_image_ptr;
  }
  else if (builder_ptr->name == "file")
  {
    rapidjson::Document doc;
    const TBuilder<rapidjson::Document>* file_builder_ptr = reinterpret_cast<const TBuilder<rapidjson::Document>*>(builder_ptr);
    file_builder_ptr->Do(rpAsset, roConfig, doc);
    return Do(rpAsset, doc, rpWidget);
  }

  return false;
}



CScriptBuilder::CScriptBuilder()
  : TBuilder<script::ISubstance* const>("script")
{
  ;
}

bool CScriptBuilder::Do(core::IAsset* const& rpAsset, const rapidjson::Value& roConfig, script::ISubstance* const& rpSubstance) const
{
  if (NULL == rpSubstance) return false;

  assert(roConfig.IsString());
  rpSubstance->Compile(roConfig.GetString());
  return true;
}


CBuilderManager::CBuilderManager()
{
  ;
}

void CBuilderManager::Set(const std::string& rsName, IBuilder* const& rpBuilder)
{
  m_mBuilder.insert(std::make_pair(rsName, rpBuilder));
}

const IBuilder* const CBuilderManager::Get(const std::string& rsName) const
{
  MBuilder::const_iterator cit_find = m_mBuilder.find(rsName);
  if (cit_find == m_mBuilder.end()) return NULL;
  return cit_find->second;
}

}
}
