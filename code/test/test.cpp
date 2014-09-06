#include "test.h"

#include <string>

#if !defined(LIBR_PATH)
#error "can't get the library path!!!"
#endif

#if !defined(DATA_PATH)
#error "can't get the data path!!!"
#endif

void GetConfig(std::string& rsLibrPath, std::string& rsDataPath, std::string& rsFile)
{
  rsLibrPath = LIBR_PATH;
  rsDataPath = DATA_PATH;
  rsFile = "potato.json";
}
