# M0 概念：移动语义

## 1. 复制和移动的区别

复制：

```cpp
std::vector<int> a(5, 7);
std::vector<int> b = a;
```

- 会重新申请一块内存；
- 把 a 里的元素逐个复制过去；
- a 和 b 各有一块数据，互不影响。

移动：

```cpp
std::vector<int> a(5, 7);
std::vector<int> c = std::move(a);
```

- 不重新复制所有元素；
- 直接把 a 内部那块数据搬给 c；
- a 会变成空的有效状态；
- 所以移动通常比复制便宜。

## 2. std::move 是什么意思

std::move(a) 不是真的移动东西。

它只是告诉编译器：

```text
a 现在可以被当成临时对象，允许把 a 的资源拿走。
```

真正执行移动的是 vector 的移动构造函数。

## 3. 一个直观比喻

- 复制：把一箱书一本一本抄一遍，再装一箱。
- 移动：直接把原来那箱书搬到新箱子里，只换个标签。

移动省掉的是重新申请内存和逐个复制元素。

## 4. 实验

打开 examples/m0_move.cpp，按 TODO 1 到 TODO 5 补完。

编译运行：

```powershell
g++ -std=c++17 -Wall -Wextra examples\m0_move.cpp -o build\m0_move.exe
.\build\m0_move.exe
```

观察：

- 复制出来的 b，指针和 a 一样吗？
- 移动出来的 c，指针和原来的 a 一样吗？
- 移动后 a 的 size 是多少？
- 移动后 c 的 size 是多少？

## 5. 回答 self_check 第三部分第 5 题

在之前值传递、引用传递的基础上，补上：

- 移动语义是什么？
- 移动为什么能省一次拷贝？
- std::move 本身有没有真的搬东西？