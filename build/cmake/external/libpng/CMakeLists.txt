cmake_minimum_required(VERSION 2.4.3)

project(png C)

# needed packages
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

set(CODE_PATH_libpng ${CODE_PATH_external}/libpng)

set(libpng_sources
  ${CODE_PATH_libpng}/png.h
  ${CODE_PATH_libpng}/pngconf.h
  ${CODE_PATH_libpng}/png.c
  ${CODE_PATH_libpng}/pngerror.c
  ${CODE_PATH_libpng}/pngget.c
  ${CODE_PATH_libpng}/pngmem.c
  ${CODE_PATH_libpng}/pngpread.c
  ${CODE_PATH_libpng}/pngread.c
  ${CODE_PATH_libpng}/pngrio.c
  ${CODE_PATH_libpng}/pngrtran.c
  ${CODE_PATH_libpng}/pngrutil.c
  ${CODE_PATH_libpng}/pngset.c
  ${CODE_PATH_libpng}/pngtrans.c
  ${CODE_PATH_libpng}/pngwio.c
  ${CODE_PATH_libpng}/pngwrite.c
  ${CODE_PATH_libpng}/pngwtran.c
  ${CODE_PATH_libpng}/pngwutil.c
)

add_definitions(-DPNG_CONFIGURE_LIBPNG)

if(MSVC)
  add_definitions(-DPNG_NO_MODULEDEF -D_CRT_SECURE_NO_DEPRECATE)
else(MSVC)
  add_definitions(-DZLIB_DLL)
endif(MSVC)

add_definitions(-DLIBPNG_NO_MMX)
add_definitions(-DPNG_NO_MMX_CODE)
add_definitions(-DPNG_NO_FLOATING_POINT_SUPPORTED)

set(LIBRARY_OUTPUT_PATH "${OUT_PATH}/lib")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ZLIB_INCLUDE_DIR})

add_library(png STATIC ${libpng_sources})
if(MSVC)
  # msvc does not append 'lib' - do it here to have consistent name
  set_target_properties(${PNG_LIB_NAME_STATIC} PROPERTIES PREFIX "lib")
endif()