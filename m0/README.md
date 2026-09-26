# M0 语言与工具地基

M0 已完成。

- 完成日期：2026-09-26
- 原始完成 commit：c9b19ce
- 验收证据：m0/docs/self_check.md
- 当前计划入口：项目根目录 TODO.md
- M1 材料：docs/m1_file_io.md、examples/m1_file_io.cpp

## 目录说明

### m0/docs

M0 的概念讲解和自我检查：

- self_check.md：M0 验收回答
- m0_compile_pipeline.md：.cpp 到 .exe 的四步
- m0_stack_heap.md：栈与堆
- m0_pointer_reference.md：指针与引用
- m0_vector_growth.md：vector 扩容
- m0_move_semantics.md：移动语义
- m0_copy_semantics.md：Rule of Three 和深拷贝
- m0_dynamic_array.md：手写动态数组
- m0_dynamic_array_move.md：动态数组的移动构造和移动赋值
- m0_matmul.md：手写矩阵乘法
- m0_cmake_explained.md：CMake 是什么
- m0_tensor_walkthrough.md：Tensor 代码讲解
- m0_resources.md：M0 学习资源地图
- m0_toolchain.md：工作区工具链说明

### m0/examples

M0 的代码练习：

- m0_hello.cpp
- m0_stack_heap.cpp
- m0_pointer_reference.cpp
- m0_vector_growth.cpp
- m0_value_reference.cpp
- m0_move.cpp
- m0_dynamic_array.cpp
- m0_matmul.cpp
- m0_cmake_demo/：最小 CMake 项目

## 怎么重新编译 M0 示例

这些示例都是独立小程序，不需要项目库。例如：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer

g++ -std=c++17 -Wall -Wextra m0\examples\m0_dynamic_array.cpp -o build\m0_dynamic_array.exe
.\build\m0_dynamic_array.exe
```

CMake 示例：

```powershell
cd m0\examples\m0_cmake_demo
cmake -S . -B build -G Ninja
cmake --build build
.\build\m0_hello.exe
```