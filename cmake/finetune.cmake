FILE(GLOB STEREO_CAMERA_HEADERS ${PROJECT_SOURCE_DIR}/src/include/*.h)
FILE(GLOB STEREO_CAMERA_SOURCES ${PROJECT_SOURCE_DIR}/src/impl/*.cpp)
SET(STEREO_CAMERA_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/src/include)

MESSAGE(STATUS "find finetune stereocamera header files: ${STEREO_CAMERA_HEADERS}")
MESSAGE(STATUS "find finetune stereocamera header path: ${STEREO_CAMERA_INCLUDE_PATH}")
MESSAGE(STATUS "find finetune stereocamera sources files: ${STEREO_CAMERA_SOURCES}")