cmake_minimum_required(VERSION 3.1)

# 找到海康威视摄像头的引用包 
# set(HIKI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
# set(HIKI_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
# link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(STEREO_LIB_SOURCE_FILE
stereo_clib.h 
stereo_clib.cpp
)

add_library(STEREO_LIB STATIC ${STEREO_LIB_SOURCE_FILE})
add_library(stereo_camera::stereo_lib ALIAS STEREO_LIB)
