# Tensor 入门讲解

> 这份文件解释 src/tensor.cpp 和 include/mini_infer/tensor.hpp 里那些你没写的辅助代码。
> 不要背，也不要抄。读完以后，用自己的话重写 m0/docs/self_check.md 第一部分。

## 0. 先看一个 2x3 的例子

假设有一个 2 行 3 列的表：

```text
1 2 3
4 5 6
```

在这个项目里，它会被存成：

- shape_ = {2, 3}
- data_ = {1, 2, 3, 4, 5, 6}

shape_ 记录有几行几列。
data_ 是一个一维数组，把二维表按行拉平。

为什么能拉平？
因为二维表只是人看起来的排列，计算机内存是一长条。
按行读：

```text
第0行：1 2 3
第1行：4 5 6
合起来：1 2 3 4 5 6
```

## 1. 类、对象、成员分别是什么

- 类（class）：图纸。Tensor 就是图纸。
- 对象（object）：按图纸造出来的东西。Tensor t(2, 3); 就是造出一个对象。
- 成员变量：对象里面存的变量。Tensor 里有 shape_ 和 data_。
- 成员函数：对象会做的动作。at、add、matmul 都是。

访问方式：

```cpp
Tensor t(2, 3);   // 造对象
t.at(0, 0) = 7;   // 让 t 执行 at 这个动作
```

## 2. 头文件里在写什么

include/mini_infer/tensor.hpp 是说明书，告诉别人 Tensor 有哪些函数。

- #pragma once：这个头文件只允许被编译器读一次。
- #include <cstddef>：提供 std::size_t。
- #include <vector>：提供 std::vector。
- namespace mini_infer { ... }：把这些名字装进一个盒子，避免和别人重名。
- class Tensor { ... };：图纸本体。
- public:：外面可以直接用的部分。
- private:：只有类自己内部才能碰的部分。

std::size_t 是什么？
- 一种专门用来表示大小、下标的整数类型。
- 你可以先把它当成非负整数。

float 是什么？
- 小数类型。例如 0.5f、3.14f。

## 3. 构造函数：Tensor::Tensor

代码：

```cpp
Tensor::Tensor(std::size_t rows, std::size_t cols, float value)
    : shape_{rows, cols}, data_(rows * cols, value) {
    assert(rows > 0 && cols > 0);
}
```

作用：创建一个 Tensor，并立刻把里面填好。

三个参数：
- rows：几行。
- cols：几列。
- value：每个格子一开始填什么数。默认是 0.0f。

冒号后面的部分叫初始化列表：

```cpp
shape_{rows, cols}
```

意思是：把成员变量 shape_ 初始化成 {rows, cols}。

```cpp
data_(rows * cols, value)
```

意思是：data_ 里面放 rows * cols 个 value。

例如：

```cpp
Tensor t(2, 3, 1.5f);
```

等价于：

- shape_ = {2, 3}
- data_ = {1.5, 1.5, 1.5, 1.5, 1.5, 1.5}

assert(rows > 0 && cols > 0);
- 保证行数和列数都大于 0。
- 如果违反，程序会立刻停住，告诉你这里有问题。
- 它是在调试时帮你抓错误的，不是给用户看的错误提示。

## 4. rows、cols、size、shape

```cpp
std::size_t Tensor::rows() const { return shape_.at(0); }
std::size_t Tensor::cols() const { return shape_.at(1); }
std::size_t Tensor::size() const { return data_.size(); }
const std::vector<std::size_t>& Tensor::shape() const { return shape_; }
```

- rows()：返回第 0 个 shape 值，也就是行数。
- cols()：返回第 1 个 shape 值，也就是列数。
- size()：返回 data_ 里数字的总个数。
- shape()：把整个 shape_ 返回出去。

后面的 const 是什么意思？
- 表示这个函数不会修改对象自己。
- 例如 rows() 只是读，不会改数据。

const std::vector<std::size_t>& 为什么又写引用又写 const？
- 加 & 表示不复制整个 vector，直接返回原来的那个。
- 加 const 表示外面只能看，不能通过这个返回值偷偷修改。

## 5. at：读写某个格子

有两个版本，代码几乎一样，区别是一个能改，一个只能读。

修改版：

```cpp
float& Tensor::at(std::size_t row, std::size_t col) {
    assert(row < rows() && col < cols());
    return data_[row * cols() + col];
}
```

只读版：

```cpp
const float& Tensor::at(std::size_t row, std::size_t col) const {
    assert(row < rows() && col < cols());
    return data_[row * cols() + col];
}
```

关键公式：

```text
data_ 下标 = row * 总列数 + col
```

为什么？
因为 data_ 是把二维表按行拉平的。

例子：2 行 3 列。

```text
(0,0) -> 0*3+0 = 0
(0,1) -> 0*3+1 = 1
(0,2) -> 0*3+2 = 2
(1,0) -> 1*3+0 = 3
(1,1) -> 1*3+1 = 4
(1,2) -> 1*3+2 = 5
```

assert(row < rows() && col < cols())：
- 保证行号和列号没有越界。
- 如果越界，调试时程序直接停住。

为什么要有两个版本？
- float& at(...)：返回可以修改的格子，能写 t.at(0,0) = 7;
- const float& at(...) const：对象是 const 时用，只能读。

## 6. fill 和 print

fill：

```cpp
void Tensor::fill(float value) {
    for (float& x : data_) {
        x = value;
    }
}
```

意思是：把 data_ 里每个数都改成 value。
float& x 表示 x 是每个元素的引用，改 x 就是改原来的数。

print：

```cpp
void Tensor::print() const {
    std::cout << "Tensor(" << rows() << ", " << cols() << ")\n";
    for (std::size_t r = 0; r < rows(); ++r) {
        for (std::size_t c = 0; c < cols(); ++c) {
            std::cout << at(r, c) << ' ';
        }
        std::cout << '\n';
    }
}
```

意思是：先打印形状，再一行一行打印数字。

## 7. 为什么说这些是辅助代码

你写的 add、matmul、transpose、softmax、linear 都依赖它们：

- add 用 at 读数和写结果。
- matmul 用 rows/cols 检查形状，用 at 读写格子。
- transpose 用 at。
- softmax 用 at 和 std::exp。
- linear 用 matmul、transpose 和 at。

所以这些辅助函数不是多余的，它们是整个 Tensor 的地基。
你不理解它们，就会觉得自己的函数像魔法。

## 8. 现在要做的练习

不要继续往后面看。先做这三步：

1. 在纸上画一个 2x4 的表，写出它的 shape_ 和 data_。
2. 在上面例子中，写出 at(1, 3) 对应的 data_ 下标。
3. 回到 m0/docs/self_check.md 第一部分，用你自己的话重写 1 到 5 题。

写完发我，我只看这 5 题有没有真正理解。