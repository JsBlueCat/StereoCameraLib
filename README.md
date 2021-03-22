# Setup

## windows
```
mkdir build && cd build
cmake -DOpenCV_DIR="D:\opencv-3.4.7\build" -DCMAKE_GENERATOR_PLATFORM=x64  -DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING ..
```
替换 D:\opencv-3.4.7\build 为你的opencv build 目录

## macos 
尚未开发