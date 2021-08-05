# Setup

## windows
```
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -T v142 -DOpenCV_DIR="D:\opencv-3.4.7\build" ..
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

## 常见问题 

0x000035 需要配置cmake路径，不然 vs的cmake nijia build 冲突。
需要安装 .net framework 最新