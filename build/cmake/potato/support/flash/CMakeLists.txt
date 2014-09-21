cmake_minimum_required(VERSION 2.8)

project(flash)

set(SRC_PATH_flash ${SRC_PATH_support}/flash)

set(INC_PATH_LIST
  ${SRC_PATH_potato}
  ${SRC_PATH_potato}/engine
  ${SRC_PATH_flash}
  )

set(SRC_FILE_LIST
  ${SRC_PATH_support}/../engine/flash.h
  ${SRC_PATH_flash}/flash_impl.h
  ${SRC_PATH_flash}/flash_base.h
  ${SRC_PATH_flash}/flash.cpp
  ${SRC_PATH_flash}/effect_rotate.h
  ${SRC_PATH_flash}/effect_rotate.cpp
  ${SRC_PATH_flash}/effect_blend.h
  ${SRC_PATH_flash}/effect_blend.cpp
  ${SRC_PATH_potato}/utility/file.h
  ${SRC_PATH_potato}/utility/log.h
  ${SRC_PATH_potato}/utility/sharedlibrary.h
  ${SRC_PATH_potato}/utility/sharedlibrary.cpp
  )

source_group("source" FILES ${SRC_FILE_LIST})

set(LIBRARY_OUTPUT_PATH "${OUT_PATH}/lib")

include_directories(${INC_PATH_LIST})

add_library(flash SHARED ${SRC_FILE_LIST})

set(LINK_FILE_LIST
  dl
  )
target_link_libraries(flash ${LINK_FILE_LIST})