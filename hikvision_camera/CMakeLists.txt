cmake_minimum_required(VERSION 3.1)

# 找到海康威视摄像头的引用包 
# set(HIKI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
# set(HIKI_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
# link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(SOURCE_FILE
hikvision_double_camera.h
MyCamera.h
hikvision_double_camera.cpp
MyCamera.cpp 
)

add_library(hikvision_camera STATIC ${SOURCE_FILE})
add_library(stereo_camera::hikvision_camera ALIAS hikvision_camera)
target_link_libraries(hikvision_camera "${CMAKE_CURRENT_SOURCE_DIR}/lib/MvCameraControl.lib")
