# M1 第二步：解析 data/mlp.txt

> 目标：把文本文件里的 8 个矩阵读成 C++ 的 `Tensor`。
> 这一步不训练、不推理，只是把数据读正确。

## 1. 文件格式回顾

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
```

规则：
- 一行是矩阵头：名字、行数、列数；
- 接下来正好有 `行数` 行；
- 每行正好有 `列数` 个浮点数。

## 2. 新工具：std::istringstream

`std::istringstream` 可以把一个字符串当成输入流来读。

```cpp
#include <sstream>

std::string line = "x 3 4";
std::istringstream iss(line);

std::string name;
std::size_t rows = 0;
std::size_t cols = 0;
iss >> name >> rows >> cols;
```

读完后：
- `name == "x"`
- `rows == 3`
- `cols == 4`

## 3. 解析流程

外层循环用 `std::getline` 一行一行读。

每读到一行：

1. 用 `std::istringstream` 解析出 `name / rows / cols`；
2. 创建一个 `Tensor t(rows, cols)`；
3. 再连续读 `rows` 行；
4. 每行用 `std::istringstream` 读 `cols` 个数字，写进 `t.at(r, c)`；
5. 把 `name` 和 `Tensor` 存进 `std::map<std::string, Tensor>`。

伪代码：

```cpp
std::map<std::string, Tensor> tensors;
std::string line;

while (std::getline(fin, line)) {
    if (line.empty()) continue;

    std::istringstream header(line);
    std::string name;
    std::size_t rows = 0;
    std::size_t cols = 0;

    if (!(header >> name >> rows >> cols)) {
        std::cout << "bad header: " << line << '\n';
        return 1;
    }

    Tensor t(rows, cols);

    for (std::size_t r = 0; r < rows; ++r) {
        std::getline(fin, line);
        std::istringstream row_stream(line);
        for (std::size_t c = 0; c < cols; ++c) {
            row_stream >> t.at(r, c);
        }
    }

    tensors.emplace(name, std::move(t));
}
```

注意：
- `std::map` 需要 `#include <map>`；
- `std::istringstream` 需要 `#include <sstream>`；
- `std::move` 需要 `#include <utility>`。

## 4. 实验

打开 `examples/m1_parse_mlp.cpp`，按 TODO 补完。

在项目根目录编译运行：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer

New-Item -ItemType Directory -Force -Path build\tmp | Out-Null
$env:TEMP = "$PWD\build\tmp"
$env:TMP = "$env:TEMP"

g++ -std=c++17 -Wall -Wextra -Iinclude examples\m1_parse_mlp.cpp src\tensor.cpp -o build\m1_parse_mlp.exe
.\build\m1_parse_mlp.exe
```

期望看到：

```text
x 3 4
W1 6 4
b1 1 6
W2 3 6
b2 1 3
h 3 6
o 3 3
y 3 3
```

如果某个矩阵行列不对，先检查：
- 头行解析是否成功；
- 每行数字个数是否正确；
- 有没有跳过空行导致错位。

## 5. 完成后

下一步才是：
- 用这些 Tensor 跑 `linear -> relu -> linear -> relu -> softmax`；
- 和文件里的 `h / o / y` 对比；
- 算最大绝对误差。