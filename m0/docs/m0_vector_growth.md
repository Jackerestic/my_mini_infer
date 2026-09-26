# M0 概念：std::vector 为什么会扩容

## 1. std::vector 是什么

std::vector 可以理解成：

- 一个会自动变长的数组；
- 里面的元素在内存里连续排列；
- 知道当前有几个元素，也就是 size；
- 还知道当前最多能装几个，也就是 capacity。

```cpp
std::vector<int> v;
v.push_back(10);   // 往末尾加一个元素
```

## 2. size 和 capacity 的区别

- size：现在实际有几个元素。
- capacity：不重新申请内存的话，最多能装几个元素。

刚创建的 vector 通常是：

```text
size = 0
capacity = 0
```

第一次 push_back 后，它可能会申请一小块内存。

## 3. 扩容时发生什么

当 size == capacity，再 push_back 时：

1. 申请一块更大的新内存；
2. 把旧元素复制或移动到新内存；
3. 释放旧内存；
4. 把新元素加进去；
5. capacity 变大。

关键后果：

- 扩容前拿到旧元素的地址、指针、迭代器，扩容后会失效；
- 所以 vector 扩容不是原地变大，而是搬家。

不同编译器扩容倍数可能不同，常见是 2 倍或 1.5 倍。

## 4. 实验

打开 m0/examples/m0_vector_growth.cpp，按 TODO 补完。

编译运行：

```powershell
g++ -std=c++17 -Wall -Wextra m0\examples\m0_vector_growth.cpp -o build\m0_vector_growth.exe
.\build\m0_vector_growth.exe
```

观察：

- size 每次加 1；
- capacity 什么时候跳；
- 每次扩容时，第一个元素的地址有没有变。

## 5. 回答 self_check 第三部分第 4 题

用你自己的话回答：

- vector 为什么会扩容？
- 扩容时旧元素怎么办？
- 为什么扩容后旧的地址可能不能再用？