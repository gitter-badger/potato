cmake_minimum_required(VERSION 2.8)

project(asset)

# needed packages
find_package(ZLIB REQUIRED)

set(SRC_PATH_asset ${SRC_PATH_support}/asset)

if(OS_IS_LINUX)
set(INC_PATH_LIST
  ${SRC_PATH_potato}
  ${SRC_PATH_potato}/engine
  ${SRC_PATH_asset}
  )
elseif(OS_IS_WINDOWS)
set(INC_PATH_LIST
  ${SRC_PATH_potato}
  ${SRC_PATH_potato}/engine
  ${CODE_PATH}/external/libpng
  ${CODE_PATH}/external/zlib
  ${SRC_PATH_asset}
  )
endif()

set(SRC_FILE_LIST_base
  ${SRC_PATH_support}/../engine/asset.h
  ${SRC_PATH_asset}/asset_impl.h
  ${SRC_PATH_asset}/asset.cpp
  ${SRC_PATH_asset}/file_png.cpp
  )
  
if(OS_IS_LINUX)
set(SRC_FILE_LIST
  ${SRC_FILE_LIST_base}
  )
elseif(OS_IS_WINDOWS)
set(SRC_FILE_LIST
  ${SRC_FILE_LIST_base}
  ${SRC_PATH_asset}/asset.def
  )
endif()

source_group("source" FILES ${SRC_FILE_LIST})

set(LIBRARY_OUTPUT_PATH
  ${OUT_PATH}/bin
  )

link_directories(${OUT_PATH}/lib/Release)

include_directories(${INC_PATH_LIST})

add_definitions(-DC4G_API_EXPORT)

add_library(asset SHARED ${SRC_FILE_LIST})


if(OS_IS_LINUX)
set(LINK_FILE_LIST
  png12
  z
  )
elseif(OS_IS_WINDOWS)
  if(BUILD_DEBUG)
    set(LINK_FILE_LIST
      ${OUT_PATH}/lib/Debug/png.lib
      ${ZLIB_LIBRARY}
      )
  elseif(BUILD_MINSIZEREL)
    set(LINK_FILE_LIST
      ${OUT_PATH}/lib/MinSizeRel/png.lib
      ${ZLIB_LIBRARY}
      )
  elseif(BUILD_RELEASE)
    set(LINK_FILE_LIST
      ${OUT_PATH}/lib/Release/png.lib
      ${ZLIB_LIBRARY}
      )
  elseif(BUILD_RELWITHDEBINFO)
    set(LINK_FILE_LIST
      ${OUT_PATH}/lib/RelWithDebInfo/png.lib
      ${ZLIB_LIBRARY}
      )
  endif()
endif()
target_link_libraries(asset ${LINK_FILE_LIST})

add_dependencies(asset png)
