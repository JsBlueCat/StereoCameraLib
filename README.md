# Setup

## windows
```
mkdir build && cd build
cmake -G "Visual Studio 15 2017 Win64" -DOpenCV_DIR="D:\opencv-3.4.7\build" -DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING  ..
cmake --build . --config Debug
```
替换 D:\opencv-3.4.7\build 为你的opencv build 目录
### 打包所有依赖

```
cmake --install . --config Debug
```
### 单元测试

```
ctest all 
```
or show verbose

```
ctest all --vebose
```


## macos 
尚未开发