# M0 概念：CMake 到底是什么

> 这一节的目标不是命令能跑，而是你能用自己的话说明：
> CMake 是什么，它替你解决了什么问题。

## 1. 一句话理解

CMake 不是编译器。

CMake 是一个构建系统生成器。
意思是：

- 你告诉 CMake：项目里有哪些源文件、要生成哪些程序、测试怎么跑；
- CMake 帮你生成 Ninja 或 Makefile 能执行的构建文件；
- 真正调用 g++ 编译和链接的，是 Ninja 或 Make。

## 2. 没有 CMake 时怎么做

你之前手动编译过：

```powershell
g++ -std=c++17 -Iinclude src\main.cpp src\tensor.cpp -o build\mini_app.exe
```

问题：
- 文件一多，命令很长；
- 加一个源文件就要改命令；
- 测试、Release、Debug 配置都要自己记；
- 换电脑、换编译器又要重来。

CMake 把这些东西写进一个文件：CMakeLists.txt。

## 3. 三个核心命令

### 配置

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

含义：
- `-S .`：源代码目录是当前目录；
- `-B build`：构建目录叫 build；
- `-G Ninja`：生成 Ninja 构建文件；
- `-DCMAKE_BUILD_TYPE=Release`：设置 Release 模式。

配置完成后，build 目录里会出现：
- `CMakeCache.txt`：CMake 记住的配置；
- `build.ninja`：Ninja 的构建文件；
- `CTestTestfile.cmake`：CTest 的测试列表。

### 构建

```powershell
cmake --build build
```

它会调用 Ninja，让 Ninja 去调用 g++，生成 exe 和库。

### 测试

```powershell
ctest --test-dir build --output-on-failure
```

它会运行 CMakeLists.txt 中 add_test 注册的测试。

## 4. 本项目的 CMakeLists.txt 逐行看

打开 `CMakeLists.txt`，对照下面的解释：

```cmake
cmake_minimum_required(VERSION 3.15)
```

要求 CMake 至少 3.15 版本。

```cmake
project(my_mini_infer LANGUAGES CXX)
```

项目名叫 my_mini_infer，使用 C++ 语言。

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

使用 C++17，不允许编译器私有扩展。

```cmake
add_library(mini_infer
    src/tensor.cpp
)
```

把 `src/tensor.cpp` 编译成一个库，名字叫 mini_infer。

```cmake
target_include_directories(mini_infer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

告诉编译器去哪里找头文件。

```cmake
add_executable(mini_infer_app
    src/main.cpp
)
target_link_libraries(mini_infer_app PRIVATE mini_infer)
```

生成 `mini_infer_app.exe`，并链接 mini_infer 库。

```cmake
enable_testing()
```

打开测试功能。

```cmake
add_executable(test_tensor
    tests/test_tensor.cpp
)
target_link_libraries(test_tensor PRIVATE mini_infer)
```

生成测试程序 `test_tensor.exe`。

```cmake
add_test(NAME test_tensor COMMAND $<TARGET_FILE:test_tensor>)
```

注册一个测试，名字叫 test_tensor，要运行的程序是 test_tensor.exe 的真实路径。

## 5. 最小实验

基础实验在：

```text
examples/m0_cmake_demo/
├─ CMakeLists.txt
└─ main.cpp
```

进入目录：

```powershell
cd examples\m0_cmake_demo
```

配置、构建、运行：

```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\m0_hello.exe
```

期望输出：

```text
hello cmake
```

观察 build 目录里生成了哪些文件。

## 6. 完成后回答

在 docs/self_check.md 第四部分写下：

- CMake 是什么？
- CMake 和 Ninja、g++ 分别负责什么？
- `-S`、`-B`、`-G` 分别是什么意思？
- 配置、构建、测试分别用哪条命令？

直到你能不看资料说出来，再算 M0 的 CMake 通过。