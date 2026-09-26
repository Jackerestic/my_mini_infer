# M0 学习资料地图

> 先看本地资料，不够再查外部。
> 看资料不算有效时间；必须边看边写代码、写自己的解释，才算。

## 1. 编译过程：.cpp 怎么变成 .exe

- 目标：能说出预处理、编译、汇编、链接四步。
- 本地实验：
  - 运行 `.\build.bat gpp`，看它实际调用了什么命令。
  - 之后自己用 g++ 一步步跑：`-E`、`-S`、`-c`、链接。
- 外部资料：
  - cppreference: Compiler support / Translation phases
  - 搜索关键词：C++ translation phases preprocessing compilation linking

## 2. 栈与堆、指针与引用

- 本地资料：
  - `my_CS106L/lec03_initialization&reference.md`
  - `my_CS106L/c++Notes/动态内存管理.txt`
- 目标：
  - 各写一个可运行的小例子。
  - 能用一句话说清：什么时候用栈，什么时候用堆。
- 外部资料：
  - cppreference: pointer, reference, new/delete

## 3. std::vector 扩容

- 本地资料：
  - `my_CS106L/lec02_Types&Structs.md`
  - `my_CS106L/c++Notes/动态内存管理.txt`
- 外部资料：
  - cppreference: std::vector, capacity, reserve, push_back
- 目标：
  - 打印 size 和 capacity，观察 push_back 多少次后扩容。
  - 自己写一个动态数组，支持 push_back 和扩容。

## 4. 值传递、引用传递、移动语义、拷贝

- 本地资料：
  - `my_CS106L/c++Notes/拷贝构造和拷贝赋值.md`
- 外部资料：
  - cppreference: copy constructor, move constructor, std::move, rule of three/five
- 目标：
  - 写一个类，打印拷贝构造、移动构造、析构分别在什么时候调用。

## 5. RAII

- 目标：写一个类，在构造函数里拿资源，在析构函数里放资源。
- 外部资料：
  - cppreference: RAII
  - 搜索关键词：C++ RAII destructor resource management

## 6. 现有项目代码

- `m0/docs/m0_tensor_walkthrough.md`：先读这个，它解释了辅助代码。
- `include/mini_infer/tensor.hpp`
- `src/tensor.cpp`
- `tests/test_tensor.cpp`
- 目标：把 `m0/docs/self_check.md` 第二部分的表格填完。
- 方法：
  - 不要一行一行从第一行读到最后。
  - 按函数读：先看函数签名，猜它做什么；再看函数体；最后用自己的话写出来。

## 7. CMake

- 目标：不看笔记完成：
  ```powershell
  cmake -S . -B build
  cmake --build build
  ctest --test-dir build --output-on-failure
  ```
- 外部资料：
  - CMake 官方 Tutorial
- 当前状态：本机 PATH 里没有 cmake。修工具链算成本，不算有效产出，但要放到 M0 的固定任务里完成。