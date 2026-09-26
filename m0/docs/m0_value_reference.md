# M0 概念：值传递与引用传递

## 1. 值传递

```cpp
void change_by_value(int x) {
    x = 100;
}
```

调用：

```cpp
int a = 10;
change_by_value(a);
// a 还是 10
```

为什么？
- 调用函数时，会先复制一份 a 的值给 x；
- 函数里改的是副本 x，不是原来的 a；
- 对很大的对象来说，复制一次很贵。

## 2. 引用传递

```cpp
void change_by_reference(int& x) {
    x = 100;
}
```

调用：

```cpp
int a = 10;
change_by_reference(a);
// a 变成 100
```

为什么？
- `int& x` 是引用；
- x 是 a 的别名；
- 改 x 就是改 a；
- 不会复制一份新的 int。

## 3. const 引用

```cpp
void print_value(const int& x) {
    std::cout << x << '\n';
}
```

- 加 `&`：不复制；
- 加 `const`：函数里不能改 x；
- 适合传入大对象，比如 Tensor。

在这个项目里：

```cpp
Tensor add(const Tensor& other) const;
```

`const Tensor& other` 表示：
- 不复制 other 这个矩阵；
- 函数里也不能改 other。

## 4. 实验

打开 m0/examples/m0_value_reference.cpp，按 TODO 补完。

编译运行：

```powershell
g++ -std=c++17 -Wall -Wextra m0\examples\m0_value_reference.cpp -o build\m0_value_reference.exe
.\build\m0_value_reference.exe
```

观察：

- 值传递后，原变量有没有变？
- 引用传递后，原变量有没有变？
- const 引用能不能修改原变量？

## 5. 先回答第 5 题的前半部分

暂时只回答：

- 值传递会发生什么？
- 引用传递会发生什么？
- 为什么这个项目里 Tensor 参数常写成 const Tensor&？

移动语义等这一步做完再讲。