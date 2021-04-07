cmake_minimum_required(VERSION 3.1)
project(stereocamera CXX)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang|GNU")
        # using Clang or GCC
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "-std=c++17")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        # using Visual Studio C++
        add_compile_definitions(_HAS_TR1_NAMESPACE )
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/dist/bin)

find_package(OpenCV 3.4 REQUIRED HINTS "D:\\opencv-3.4.7\\build")

include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")


FILE(GLOB CAMERA_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/impl/*.cpp")
FILE(GLOB CAMERA_HEADER_FILES "${PROJECT_SOURCE_DIR}/src/include/*.h")
FILE(GLOB HIKILIB "${PROJECT_SOURCE_DIR}/src/lib/MvCameraControl.*")

include_directories(${PROJECT_SOURCE_DIR}/src/include)
add_library(stereocamera STATIC ${CAMERA_SOURCE_FILES}) 
# target_include_directories(stereocamera PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(stereocamera ${OpenCV_LIBS} ${HIKILIB})
set_target_properties(stereocamera  PROPERTIES 
  ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/package/lib/"
  LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/package/lib/"
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/dist/bin)

add_executable(stereo_grab "${PROJECT_SOURCE_DIR}/src/experiment/stereo_grab.cpp" ${CAMERA_SOURCE_FILES})
target_link_libraries(stereo_grab ${OpenCV_LIBS} ${HIKILIB})

add_executable(stereo_match "${PROJECT_SOURCE_DIR}/src/experiment/stereo_match.cpp" ${CAMERA_SOURCE_FILES})
target_link_libraries(stereo_match ${OpenCV_LIBS} ${HIKILIB})



# enable CTest testing
add_subdirectory(3rd_party/google-test)
enable_testing()

MESSAGE(STATUS "INSTALL ${CAMERA_HEADER_FILES}")

install(FILES ${CAMERA_HEADER_FILES}
DESTINATION "${PROJECT_SOURCE_DIR}/package/include/"
)

FILE(GLOB CAMERADLL "${PROJECT_SOURCE_DIR}/build/Debug/stereocamera*.dll")

MESSAGE(STATUS "INSTALL ${CAMERADLL}")
install(FILES ${CAMERADLL}
DESTINATION "${PROJECT_SOURCE_DIR}/package/lib/"
)