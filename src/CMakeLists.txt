cmake_minimum_required(VERSION 3.13.0)

include(opencv)
include(hikvision)
include(finetune)


include_directories(${OpenCV_INCLUDE_DIRS} ${HIKVISION_INCLUDE_DIR} ${STEREO_CAMERA_INCLUDE_PATH}) 
add_library(StereoCamera STATIC ${STEREO_CAMERA_SOURCES})
target_link_libraries(StereoCamera PUBLIC ${Opencv_LIBS} ${HIKVISION_LIB} )
target_compile_options(StereoCamera PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/std:c++17 /wd4819 /EHsc>)
set_target_properties(StereoCamera PROPERTIES 
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/package/lib/"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/package/lib/"
)


SET(STEREO_GRAB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/experiment/stereo_grab.cpp")
SET(STEREO_MATCH_SRC "${CMAKE_CURRENT_SOURCE_DIR}/experiment/stereo_match.cpp")

add_executable(STEREO_GRAB ${STEREO_GRAB_SRC})
target_link_libraries(STEREO_GRAB PUBLIC StereoCamera ${OpenCV_LIBS} ${HIKVISION_LIB})
target_include_directories(STEREO_GRAB PUBLIC ${HIKVISION_INCLUDE_DIR} ${STEREO_CAMERA_INCLUDE_PATH} ${OpenCV_INCLUDE_DIRS})
target_compile_options(STEREO_GRAB PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/std:c++17 /wd4819 /EHsc>)
set_target_properties(STEREO_GRAB PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin

)
add_executable(STEREO_MATCH ${STEREO_MATCH_SRC})
target_compile_options(STEREO_MATCH PUBLIC $<$<COMPILE_LANGUAGE:CXX>:/std:c++17 /wd4819 /EHsc>)
target_link_libraries(STEREO_MATCH PUBLIC StereoCamera ${OpenCV_LIBS} ${HIKVISION_LIB})
target_include_directories(STEREO_MATCH PUBLIC ${HIKVISION_INCLUDE_DIR} ${STEREO_CAMERA_INCLUDE_PATH} ${OpenCV_INCLUDE_DIRS})
set_target_properties(STEREO_MATCH PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)



# include lib and include files
install(FILES ${STEREO_CAMERA_HEADERS} ${HIKVISION_INCLUDE_FILES}
DESTINATION ${PROJECT_SOURCE_DIR}/package/include/
)
FILE(GLOB BUILD_LIB ${PROJECT_SOURCE_DIR}/package/lib/*/*.lib)
install(FILES ${HIKVISION_LIB} ${BUILD_LIB}
 DESTINATION ${PROJECT_SOURCE_DIR}/package/lib/
)