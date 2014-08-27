#include "potato.h"

#include "common.h"

#include "utility/util_dl.h"
#include "utility/util_file.h"

#include <rapidjson/document.h>

FUNC_API_TYPEDEF(CreateEngine, ac::core::IEngine, const ac::base::Config);
FUNC_API_TYPEDEF(DestroyEngine, ac::core::IEngine, const ac::base::Config);

namespace ac {

/*!
 * \class Potato
 * \brief A main entry of the application
 */
class Potato
{
public:
  static Potato& Instance(const std::string& rsDataPath, const std::string& rsConfigFile);

protected:
  Potato(const std::string& rsDataPath, const std::string& rsConfigFile);
  virtual ~Potato();

public:
  core::IEngine*& GetEngine();

private:
  ac::base::Config m_oConfigEngine;
  core::IEngine* m_pEngine;
};

Potato& Potato::Instance(const std::string& rsDataPath, const std::string& rsConfigFile)
{
  static Potato gs_potato(rsDataPath, rsConfigFile);
  return gs_potato;
}

Potato::Potato(const std::string& rsRootPath, const std::string& rsConfigFile) :
    m_pEngine(NULL)
{
  std::string file_context = utility::ReadFile((rsRootPath + "/" + rsConfigFile).c_str());

  /// parse the configure file
  /// and check the value's type
  rapidjson::Document doc;
  doc.Parse(file_context.c_str());
  assert(doc.IsObject());
  const rapidjson::Value& engine = doc["engine"];
  assert(engine.IsObject());
  const rapidjson::Value& library = engine["library"];
  assert(library.IsObject());
  const rapidjson::Value& library_file = library["file"];
  assert(library_file.IsString());
  const rapidjson::Value& configure = engine["configure"];
  assert(configure.IsObject());
  const rapidjson::Value& configure_file = configure["file"];
  assert(configure_file.IsString());

  m_oConfigEngine._sPath = rsRootPath;
  m_oConfigEngine._sLibraryFile = library_file.GetString();
  m_oConfigEngine._sConfigureFile = configure_file.GetString();

  /// load the dynamic library
  typedef FUNC_API_TYPE(CreateEngine) CreateEngineFuncPtr;
  CreateEngineFuncPtr func_create_func_ptr = utility::DynamicLibraryManager::Instance().GetFunc<CreateEngineFuncPtr>(m_oConfigEngine.GetLibraryFile(), TOSTRING(CreateEngine));
  /// create the engine with configure
  func_create_func_ptr(m_pEngine, m_oConfigEngine);
}

Potato::~Potato()
{
  /// load the dynamic library
  typedef FUNC_API_TYPE(DestroyEngine) DestroyEngineFuncPtr;
  DestroyEngineFuncPtr func_destroy_func_ptr = utility::DynamicLibraryManager::Instance().GetFunc<DestroyEngineFuncPtr>(m_oConfigEngine.GetLibraryFile(), TOSTRING(DestroyEngine));
  /// destroy the engine with configure
  func_destroy_func_ptr(m_pEngine, m_oConfigEngine);
}

core::IEngine*& Potato::GetEngine()
{
  return m_pEngine;
}
}

int main(int argc, char* argv[])
{
  std::string path;
  std::string file;
  GetConfig(path, file);
  ac::core::IEngine*& engine = ac::Potato::Instance(path, file).GetEngine();
  engine->Run();
  return 0;
}