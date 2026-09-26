# M0 手写实现：动态数组

> 目标：自己写一个 mini vector，理解 vector 内部到底在做什么。
> 不要看 std::vector 源码。先按步骤写朴素版本。

## 1. 数据结构

一个动态数组需要三个成员：

```cpp
int* data_;              // 指向堆上的一块连续内存
std::size_t size_;        // 现在用了几个格子
std::size_t capacity_;    // 总共申请了几个格子
```

图示：

```text
data_ -> [ 1 ][ 2 ][ 3 ][   ][   ]
          size_ = 3
          capacity_ = 5
```

## 2. 构造函数和析构函数

构造函数：

```cpp
DynamicArray()
    : data_(nullptr), size_(0), capacity_(0) {}
```

析构函数：

```cpp
~DynamicArray() {
    delete[] data_;
}
```

这就是 RAII：
- 对象创建时拿到资源；
- 对象销毁时自动释放资源。

## 3. grow：扩容函数

当 size_ == capacity_ 时，要扩容。

步骤：

1. 新容量 = 0 时变 1，否则乘 2；
2. `new int[new_capacity]` 申请新内存；
3. 把旧数据逐个复制到新内存；
4. `delete[] data_` 释放旧内存；
5. 让 data_ 指向新内存；
6. 更新 capacity_。

## 4. push_back：往末尾加元素

步骤：

1. 如果 size_ == capacity_，调用 grow；
2. 把 value 放到 data_[size_]；
3. size_ 加 1。

## 5. 下标访问

```cpp
int& operator[](std::size_t index) {
    return data_[index];
}
```

这样就能写：

```cpp
a[0] = 7;
std::cout << a[0];
```

## 6. 实验

打开 m0/examples/m0_dynamic_array.cpp，按 TODO 补完。

编译运行：

```powershell
g++ -std=c++17 -Wall -Wextra m0\examples\m0_dynamic_array.cpp -o build\m0_dynamic_array.exe
.\build\m0_dynamic_array.exe
```

期望看到 size 和 capacity 的变化：

```text
size=1 capacity=1
size=2 capacity=2
size=3 capacity=4
size=4 capacity=4
size=5 capacity=8
...
```

## 7. 完成后

把代码发我，我会检查：

- grow 是不是先申请新内存，再复制，再 delete 旧内存；
- 有没有出现 use-after-free；
- 析构函数有没有正确 delete[]。