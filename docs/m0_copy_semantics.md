# M0 概念：动态数组的复制（Rule of Three）

## 1. 你现在这个类有一个隐藏 bug

你的 DynamicArray 里有：

```cpp
int* data_;
```

析构函数里：

```cpp
~DynamicArray() {
    delete[] data_;
}
```

如果直接复制：

```cpp
DynamicArray a;
a.push_back(1);

DynamicArray b = a;
```

编译器会生成一个默认的拷贝构造函数，它只会做一件事：

```text
把 a.data_ 这个地址直接复制给 b.data_
```

也就是说，a 和 b 会指向同一块堆内存。

然后程序结束时：

- a 的析构函数 delete[] 这块内存；
- b 的析构函数又 delete[] 同一块内存；

这叫 double free，程序会崩溃或出现堆损坏。

## 2. 解决办法：深拷贝

复制时不要只复制指针，而要：

1. 重新申请一块新内存；
2. 把旧元素复制到新内存；
3. 让新对象的 data_ 指向新内存。

这样两个对象各有自己的内存，互不影响。

## 3. Rule of Three

如果一个类自己管理资源，通常要写三个函数：

1. 析构函数
2. 拷贝构造函数
3. 拷贝赋值运算符

这叫 Rule of Three。

## 4. 函数签名

拷贝构造函数：

```cpp
DynamicArray(const DynamicArray& other) {
    // 深拷贝
}
```

拷贝赋值：

```cpp
DynamicArray& operator=(const DynamicArray& other) {
    if (this == &other) {
        return *this;   // 自己赋值给自己，直接返回
    }

    // 1. 释放自己的旧内存
    // 2. 申请新内存
    // 3. 复制 other 的元素
    // 4. 更新 size_ 和 capacity_

    return *this;
}
```

this == &other 是判断是不是自己赋值给自己。

## 5. 实验

在 examples/m0_dynamic_array.cpp 里加上这两个函数，并在 main 里测试：

```cpp
DynamicArray a;
a.push_back(1);
a.push_back(2);

DynamicArray b = a;   // 调用拷贝构造
b[0] = 100;
std::cout << a[0] << " " << b[0] << '\n';   // 应该输出 1 100

DynamicArray c;
c = a;                // 调用拷贝赋值
c[1] = 200;
std::cout << a[1] << " " << c[1] << '\n';   // 应该输出 2 200
```

如果 a 的值被 b 或 c 改掉，说明还是浅拷贝。

## 6. 编译运行

```powershell
g++ -std=c++17 -Wall -Wextra examples\m0_dynamic_array.cpp -o build\m0_dynamic_array.exe
.\build\m0_dynamic_array.exe
```

完成后发我代码，我检查：

- 是不是重新申请了内存；
- 拷贝赋值有没有先处理自我赋值；
- 有没有 delete[] 旧内存。