cmake_minimum_required(VERSION 3.1)

# 找到海康威视摄像头的引用包 
# set(HIKI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
# set(HIKI_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
# link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")
# find_library(stereo_camera::stereo_lib)
FILE(GLOB HEADER_FILES "./*.h")
FILE(GLOB CAMERA_SOURCE_FILES "./*.cpp")

add_library(hikvision_camera STATIC ${HEADER_FILES} ${CAMERA_SOURCE_FILES})
add_library(stereo_camera::hikvision_camera ALIAS hikvision_camera)
target_link_libraries(hikvision_camera "${CMAKE_CURRENT_SOURCE_DIR}/lib/MvCameraControl.lib")