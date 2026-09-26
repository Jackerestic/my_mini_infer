# 下一步唯一入口：M1 数值对齐

> 这是给你自己看的操作清单，不要跳步。
> 每完成一个 [ ]，改成 [x]。卡住时只记录：文件、命令、完整报错。

## 0. M1 到底做什么（大白话）

C++ 程序已经会：
- 把数字放进二维表格（Tensor，也就是矩阵）；
- 矩阵乘法 matmul；
- 加偏置 linear；
- ReLU：负数变 0；
- Softmax：把一行数变成概率，一行加起来约等于 1。

M1 要证明：同一组输入和权重，Python（NumPy）和 C++ 算出来的结果，每个数字都对得上。
不是看起来差不多，而是逐个数比较，并算出最大误差。
最终最大误差必须小于 1e-5（0.00001）。

## 1. 只需要碰这 5 个文件

| 文件 | 作用 |
| --- | --- |
| data/mlp.txt | NumPy 生成的标准答案，不要手改。 |
| tests/test_mlp_reference.cpp | 我已放好的骨架，里面有 TODO 1 到 TODO 3。 |
| CMakeLists.txt | 把新测试注册进去。 |
| build.bat | 一键编译和运行测试。 |
| docs/verify.md | 最后写报告的地方，已有模板。 |

## 2. 先看懂 data/mlp.txt

在项目根目录执行：

```powershell
Get-Content data\mlp.txt
```

格式是：

```text
名字 行数 列数
第1行数字...
第2行数字...
...
```

例如：

```text
x 3 4
0.30 -1.03 0.75 0.94
...
W1 6 4
...
h 3 6
...
y 3 3
...
```

现在只做一件事：

- [ ] 找到这 8 个名字：x、W1、b1、W2、b2、h、o、y。
- [ ] 确认每个名字后面都有两个整数：行数、列数。
- [ ] 不要修改这个文件。

找到后继续下一步。
## 3. 补 tests/test_mlp_reference.cpp 的 TODO

打开 tests/test_mlp_reference.cpp。里面有三个 TODO。
- TODO 1：读一个矩阵；
- TODO 2：读整个文件，把名字和矩阵存起来；
- TODO 3：在 main 里跑 C++ 前向，并和标准答案对比。

### TODO 1：读一个矩阵

Tensor 有两个常用函数：
- t.at(r, c)：读取或写入第 r 行、第 c 列的数；
- t.rows()、t.cols()：表格有几行、几列。

文件里的矩阵长这样：

```text
x 3 4
0.30 -1.03 0.75 0.94
-1.95 -1.30 0.12 -0.31
-0.01 -0.85 0.87 0.77
```

当文件流读到 x 3 4 后，后面 12 个数字就是一个 3x4 矩阵。

TODO 1 的核心逻辑是：

```cpp
for (std::size_t r = 0; r < rows; ++r) {
    for (std::size_t c = 0; c < cols; ++c) {
        in >> result.at(r, c);   // 从文件流读下一个数字，放进矩阵
    }
}
```

骨架里已经写好循环，你只需要把中间那一行从 TODO 变成真正的代码。

- [ ] 完成 TODO 1。
- [ ] 先不要编译，先把函数写完整。

### TODO 2：读整个文件

一个文件里有多个矩阵。名字后面跟着行数和列数，所以逻辑是：

```cpp
std::string name;
std::size_t rows = 0;
std::size_t cols = 0;
while (in >> name >> rows >> cols) {
    result[name] = read_tensor(in, rows, cols);
}
```

含义：
- in >> name >> rows >> cols：从文件里读一个名字、两个整数；
- 还能读成功，就说明还有一个矩阵；
- read_tensor 把后面的数字读成 Tensor；
- result[name] 相当于把矩阵存进字典，以后用 result["x"] 就能取出来。

- [ ] 完成 TODO 2。

### TODO 3：在 main 里做对拍

C++ 前向的顺序必须和 Python 完全一致：

```text
x
 -> linear(x, W1, b1)
 -> relu()
 -> linear(..., W2, b2)
 -> relu()
 -> softmax()
```

骨架里会给你三个变量名：
- h_cpp：第一层 ReLU 之后的结果，对应文件里的 h；
- o_cpp：第二层 ReLU 之后的结果，对应文件里的 o；
- y_cpp：Softmax 之后的结果，对应文件里的 y。

然后写一个比较函数：

```cpp
float max_abs_error(const Tensor& got, const Tensor& want);
```

它逐个数计算：

```text
误差 = |got 的数 - want 的数|
```

最后要看到：
- h 最大误差 < 1e-5
- o 最大误差 < 1e-5
- y 最大误差 < 1e-5

任何一项超了，就打印失败并返回 1。

- [ ] 完成 TODO 3。
## 4. 把新测试加入构建

### 如果是用 CMake

在 CMakeLists.txt 里找到现有的：

```cmake
add_executable(test_tensor
    tests/test_tensor.cpp
)
target_link_libraries(test_tensor PRIVATE mini_infer)
add_test(NAME test_tensor COMMAND test_tensor)
```

照着加一份：

```cmake
add_executable(test_mlp_reference
    tests/test_mlp_reference.cpp
)
target_link_libraries(test_mlp_reference PRIVATE mini_infer)
add_test(
    NAME test_mlp_reference
    COMMAND test_mlp_reference ${CMAKE_CURRENT_SOURCE_DIR}/data/mlp.txt
)
```

${CMAKE_CURRENT_SOURCE_DIR} 会变成项目根目录，这样测试才找得到 data/mlp.txt。

- [ ] 改完 CMake。

### 如果是用 build.bat

在 build.bat 里找到编译 test_tensor 的部分，照着写一份：

```bat
g++ -std=c++17 -Wall -Wextra -static -Iinclude tests\test_mlp_reference.cpp src\tensor.cpp -o build\test_mlp_reference.exe
if %ERRORLEVEL% neq 0 goto :error
```

然后在 :test 里面，运行：

```bat
build\test_mlp_reference.exe data\mlp.txt
if %ERRORLEVEL% neq 0 goto :error
```

- [ ] 改完 build.bat。

为什么不能只写 data\mlp.txt：CTest 运行程序时，工作目录不一定是项目根目录。用绝对路径最稳。

## 5. 运行

在项目根目录执行：

```powershell
.\build.bat gpp
.\build.bat test
```

如果走 CMake：

```powershell
.\build.bat cmake
```

期望最后看到：

```text
h max_abs_error = 0.000000...
o max_abs_error = 0.000000...
y max_abs_error = 0.000000...
M1 PASS: all errors < 1e-5
```

如果看到 FAIL：
- 不要改 data/mlp.txt；
- 先看是 h、o、y 哪一层开始错；
- 把 FAIL 行、你的代码、完整命令发给我。

- [ ] 编译通过。
- [ ] 测试通过。
- [ ] 记下 h、o、y 的最大误差。

## 6. 填写 docs/verify.md

打开模板，填写：
- 日期；
- 编译器版本；
- Python 和 NumPy 版本；
- 生成数据的命令；
- h、o、y 最大误差；
- 最终结论。

- [ ] 填完报告。

## 7. 提交 Git

建议拆成小 commit：

```powershell
git add tests/test_mlp_reference.cpp
git commit -m "test(m1): add cpp mlp reference comparison"

git add CMakeLists.txt build.bat
git commit -m "build(m1): register mlp reference test"

git add docs/verify.md TODO.md
git commit -m "docs(m1): record numerical alignment"
```

- [ ] 完成至少 1 个 commit。
- [ ] 更新 logs/log.md 今天的记录。

## 8. 词汇表

| 词 | 大白话 |
| --- | --- |
| Tensor | 二维数字表格，也就是矩阵。 |
| shape | 表格形状，例如 3 行 4 列。 |
| 前向 | 从输入算到输出，不训练，不更新权重。 |
| 权重 | 模型里的数字，例如 W1、b1。 |
| 标准答案 | NumPy 算出来的 h、o、y。 |
| 对拍 | 两个程序算同一道题，逐个数对比。 |
| 误差 | abs(a - b)，两数相减后取绝对值。 |
| 1e-5 | 0.00001。 |
| TODO | 还没写、等你补的代码位置。 |
| std::ifstream | C++ 里打开和读取文件的工具。 |
| std::map | 字典：一个名字对应一个值，例如 data["x"]。 |

## 9. 如果还是卡住

只做这一个动作：

> 打开 data/mlp.txt，找到第一行 x 3 4，然后看它下面的 3 行数字。

看完后把 x 下面的 3 行数字发给我，并说明你看到的是几行几列。
我下一轮只带你做 TODO 1，不会一次塞更多东西。