# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Define project name

project(stereo-camera CXX)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang|GNU")
        # using Clang or GCC
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "-std=c++17")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        # using Visual Studio C++
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
endif()
add_compile_definitions(cxx_std_17)
# 添加编译模式
# set(CMAKE_BUILD_TYPE Release)#compile mode

# # 设定编译输出的位置
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/dist/bin)
# set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/lib)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV 3.4 REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")



include_directories(${OpenCV_INCLUDE_DIRS})
link_libraries(${OpenCV_LIBS})
#############################################
# Unit tests
add_subdirectory(3rd_party/google-test)
add_subdirectory(stereo_lib)
add_subdirectory(hikvision_camera)

# enable CTest testing
enable_testing()
# to solve the gtest
add_compile_definitions(_HAS_TR1_NAMESPACE)


# Declare the executable target built from your sources
add_executable(example example.cpp)
add_executable(unit_test unit_tests.cpp)

# Link your application with OpenCV libraries
target_link_libraries(example  PRIVATE ${OpenCV_LIBS})
target_include_directories(unit_test PUBLIC "${PROJECT_SOURCE_DIR}/hikvision_camera")

target_link_libraries(unit_test
stereo_camera::hikvision_camera
GTest::GTest 
GTest::Main
)

add_test(test_all unit_test)


#install 
install(CODE "MESSAGE(\"安装双目标定链接库.\")")
install(TARGETS unit_test
        CONFIGURATIONS Debug
        RUNTIME DESTINATION "${PROJECT_SOURCE_DIR}/dist/bin")