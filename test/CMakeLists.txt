cmake_minimum_required(VERSION 3.13.0)

FILE(GLOB TEST_SRCS ./*cpp)
MESSAGE(STATUS "find test source files: ${TEST_SRCS}")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(opencv)
include(hikvision)
include(finetune)

add_executable(FineTuneTest ${TEST_SRCS})
set_target_properties(FineTuneTest PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)
target_link_libraries(FineTuneTest PUBLIC gtest_main gtest StereoCamera ${OpenCV_LIBS} ${HIKVISION_LIB})
target_include_directories(FineTuneTest PUBLIC ${HIKVISION_INCLUDE_DIR} ${STEREO_CAMERA_INCLUDE_PATH} ${OpenCV_INCLUDE_DIRS})


include(GoogleTest)
gtest_discover_tests(FineTuneTest)
