# M0 实现：DynamicArray 的移动构造与移动赋值

## 1. 为什么要移动

复制 DynamicArray 时：

- 申请新内存；
- 逐元素复制；
- 两个对象各有自己的内存。

移动 DynamicArray 时：

- 直接把对方的 data_ 指针拿过来；
- 把对方置空；
- 不申请新内存，也不逐元素复制。

## 2. 移动构造函数

签名：

```cpp
DynamicArray(DynamicArray&& other) noexcept
```

步骤：

1. 偷走 other 的 data_、size_、capacity_；
2. 把 other.data_ 改成 nullptr；
3. 把 other.size_ 和 other.capacity_ 改成 0。

示例：

```cpp
DynamicArray(DynamicArray&& other) noexcept
    : data_(other.data_),
      size_(other.size_),
      capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
```

## 3. 移动赋值

签名：

```cpp
DynamicArray& operator=(DynamicArray&& other) noexcept
```

步骤：

1. 如果是自己赋值给自己，直接返回；
2. delete[] 自己原来的内存；
3. 偷走 other 的资源；
4. 把 other 置空；
5. 返回 *this。

示例：

```cpp
DynamicArray& operator=(DynamicArray&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] data_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}
```

## 4. 实验

在 examples/m0_dynamic_array.cpp 里加入：

```cpp
#include <utility>   // std::move
```

然后在 main 里测试：

```cpp
DynamicArray a;
a.push_back(1);
a.push_back(2);

DynamicArray b = std::move(a);

std::cout << "a.size=" << a.size() << '\n';   // 应该 0
std::cout << "b.size=" << b.size() << '\n';   // 应该 2

DynamicArray c;
c.push_back(9);

c = std::move(b);

std::cout << "b.size=" << b.size() << '\n';   // 应该 0
std::cout << "c.size=" << c.size() << '\n';   // 应该 2
```

## 5. 关于 noexcept

`noexcept` 表示这个函数承诺不抛异常。
标准库容器在扩容时，如果移动构造是 noexcept，会更愿意使用移动而不是复制。

现在先记住：

- 移动构造和移动赋值通常写 noexcept；
- 参数类型是 `DynamicArray&&`；
- 里面把对方置空。

## 6. 编译运行

```powershell
g++ -std=c++17 -Wall -Wextra examples\m0_dynamic_array.cpp -o build\m0_dynamic_array.exe
.\build\m0_dynamic_array.exe
```

完成后发我代码，我检查：

- 移动后原对象是不是空的；
- 原对象析构时会不会重复 delete；
- 有没有把 data_ 置为 nullptr。