# M0 手写实现：朴素矩阵乘法

## 1. 用一维数组表示二维矩阵

矩阵按行拉平：

```text
A = [ 1 2 3 ]
    [ 4 5 6 ]

一维表示：{1, 2, 3, 4, 5, 6}
行数 M = 2
列数 K = 3
```

访问第 i 行第 k 列：

```cpp
A[i * K + k]
```

例如：

```text
A[0*3+0] = A[0] = 1
A[1*3+2] = A[5] = 6
```

## 2. 矩阵乘法公式

A 是 M x K，B 是 K x N，结果是 M x N。

```text
C[i][j] = sum over k of A[i][k] * B[k][j]
```

用一维下标写：

```cpp
C[i * N + j] += A[i * K + k] * B[k * N + j];
```

## 3. 函数签名

```cpp
std::vector<float> matmul(
    const std::vector<float>& A, std::size_t M, std::size_t K,
    const std::vector<float>& B, std::size_t K2, std::size_t N);
```

- A 的大小应该是 M * K；
- B 的大小应该是 K2 * N；
- 必须满足 K == K2；
- 返回大小为 M * N 的结果。

## 4. 三重循环

```cpp
for (std::size_t i = 0; i < M; ++i) {
    for (std::size_t j = 0; j < N; ++j) {
        for (std::size_t k = 0; k < K; ++k) {
            C[i * N + j] += A[i * K + k] * B[k * N + j];
        }
    }
}
```

## 5. 实验

打开 m0/examples/m0_matmul.cpp，按 TODO 补完。

测试数据：

```text
A = [[1, 2, 3],
     [4, 5, 6]]       2 x 3

B = [[7, 8],
     [9, 10],
     [11, 12]]        3 x 2

C 应该是：
[[58, 64],
 [139, 154]]
```

编译运行：

```powershell
g++ -std=c++17 -Wall -Wextra m0\examples\m0_matmul.cpp -o build\m0_matmul.exe
.\build\m0_matmul.exe
```

期望输出：

```text
58 64 139 154
```

## 6. 完成后

发我代码。重点检查：

- i、j、k 三层循环顺序；
- 下标公式有没有写错；
- K == K2 有没有检查。