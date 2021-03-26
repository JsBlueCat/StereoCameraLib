cmake_minimum_required(VERSION 3.1)

# 找到海康威视摄像头的引用包 
# set(HIKI_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
# set(HIKI_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
# link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")


add_executable(stereo_grab "stereo_grab.cpp")
target_link_libraries(stereo_grab stereo_camera::hikvision_camera)
add_executable(stereo_match "stereo_match.cpp")
target_link_libraries(stereo_match stereo_camera::hikvision_camera)
