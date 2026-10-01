# M1 第三步：跑 MLP 前向并对拍

> 目标：用 C++ 自己算 h / o / y，和 data/mlp.txt 里的标准答案逐个数字比较。
> 这一步完成后，M1 就只剩写 docs/verify.md。

## 1. 你现在有什么

`examples/m1_parse_mlp.cpp` 现在能把文件读成：

```cpp
std::map<std::string, Tensor> tensors;
```

里面有 8 个矩阵：

- x
- W1
- b1
- W2
- b2
- h
- o
- y

其中 x、W1、b1、W2、b2 是输入和权重。
h、o、y 是 NumPy 算出的标准答案。

## 2. 要做的计算

C++ 前向：

```cpp
Tensor h_cpp = linear(x, W1, b1).relu();
Tensor o_cpp = linear(h_cpp, W2, b2).relu();
Tensor y_cpp = o_cpp.softmax();
```

然后对比：

- `h_cpp` 和 `tensors.at("h")`
- `o_cpp` 和 `tensors.at("o")`
- `y_cpp` 和 `tensors.at("y")`

## 3. 最大绝对误差

写一个函数：

```cpp
float max_abs_error(const Tensor& got, const Tensor& want) {
    // 要求两个 Tensor 形状相同
    // 返回 max |got - want|
}
```

提示：

- 先检查 `got.rows() == want.rows()` 和列数；
- 用 `std::fabs(got.at(r, c) - want.at(r, c))`；
- 维护一个当前最大值。

需要：

```cpp
#include <cmath>
#include <algorithm>
```

## 4. 验收标准

打印三行：

```text
h max error = ...
o max error = ...
y max error = ...
```

三个误差都必须小于 `1e-5`。

如果全部通过，打印：

```text
M1 PASS
```

并返回 0。  
任何一项不通过，打印：

```text
M1 FAIL
```

并返回 1。

## 5. 编译运行

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer

New-Item -ItemType Directory -Force -Path build\tmp | Out-Null
$env:TEMP = "$PWD\build\tmp"
$env:TMP = "$env:TEMP"

g++ -std=c++17 -Wall -Wextra -Iinclude examples\m1_parse_mlp.cpp src\tensor.cpp -o build\m1_verify.exe
.\build\m1_verify.exe
```

期望看到：

```text
h max error = ...e-07
o max error = ...e-07
y max error = ...e-07
M1 PASS
```

## 6. 注意

- 不要改 data/mlp.txt。
- 不要用 Python 重算一遍再填答案。
- C++ 必须自己跑出 h / o / y。
- 报错时先看是 h、o、y 哪一层开始错。