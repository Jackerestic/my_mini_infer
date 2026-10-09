# M4 第一小步：Mlp 类、批处理示例和测试

## 1. 已经有的东西

已创建：

```text
include/mini_infer/mlp.hpp
src/mlp.cpp
```

接口是：

```cpp
class Mlp {
public:
    Mlp(Tensor W1, Tensor b1, Tensor W2, Tensor b2);
    Tensor forward(const Tensor& x) const;
};
```

`forward` 内部做：

```text
linear -> relu -> linear -> relu -> softmax
```

输入：

```text
x: (N, in_features)
```

输出：

```text
y: (N, classes)
```

## 2. 下一步要写什么

### 2.1 批处理示例

新建：

```text
examples/batch_infer.cpp
```

要求：

- 构造一个小的 Mlp；
- 一次输入 N 条样本；
- 打印输出形状；
- 打印每一行概率之和。

可以先不用 data/mlp.txt，先用简单权重验证接口能工作。

例如：

```cpp
#include "mini_infer/mlp.hpp"
#include <iostream>

using mini_infer::Tensor;
using mini_infer::Mlp;

int main() {
    Tensor W1(2, 2, 1.0f);
    Tensor b1(1, 2, 0.0f);
    Tensor W2(2, 2, 1.0f);
    Tensor b2(1, 2, 0.0f);

    Mlp model(W1, b1, W2, b2);

    Tensor x(3, 2, 1.0f);

    Tensor y = model.forward(x);

    std::cout << "y shape: " << y.rows() << " x " << y.cols() << '\n';

    for (std::size_t r = 0; r < y.rows(); ++r) {
        float sum = 0.0f;
        for (std::size_t c = 0; c < y.cols(); ++c) {
            sum += y.at(r, c);
        }
        std::cout << "row " << r << " sum = " << sum << '\n';
    }

    return 0;
}
```

编译：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
New-Item -ItemType Directory -Force -Path build\tmp | Out-Null
$env:TEMP = "$PWD\build\tmp"
$env:TMP = "$env:TEMP"

g++ -std=c++17 -Wall -Wextra -Iinclude examples\batch_infer.cpp src\tensor.cpp src\mlp.cpp -o build\batch_infer.exe
.\build\batch_infer.exe
```

期望：

```text
y shape: 3 x 2
row 0 sum = 1
row 1 sum = 1
row 2 sum = 1
```

### 2.2 Catch2 测试

新建：

```text
tests/test_mlp.cpp
```

开头：

```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "mini_infer/mlp.hpp"
```

至少检查：

- `y.rows() == N`；
- `y.cols() == classes`；
- 每一行概率和约等于 1；
- 概率值都在 [0, 1]。

## 3. CMake 里加目标

`examples/batch_infer.cpp` 写好后，在 CMakeLists.txt 末尾加：

```cmake
add_executable(batch_infer
    examples/batch_infer.cpp
)
target_link_libraries(batch_infer PRIVATE mini_infer)

add_executable(test_mlp
    tests/test_mlp.cpp
)
target_include_directories(test_mlp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2
)
target_link_libraries(test_mlp PRIVATE mini_infer)
catch_discover_tests(test_mlp)
```

然后：

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## 4. 这一小步的验收

- `batch_infer.exe` 能一次算 N 条样本；
- `test_mlp` 被 CTest 逐个列出并通过；
- `ctest` 全绿；
- 提交：

```powershell
git add examples/batch_infer.cpp tests/test_mlp.cpp CMakeLists.txt
git commit -m "test(m4): add batch inference example and mlp tests"
```