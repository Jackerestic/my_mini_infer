# M3 第一步：先测基线，不要先优化

## 1. 目标

先知道当前朴素 matmul 有多慢。

固定：

- 矩阵大小：512 x 512
- 输入：benchmarks/bench_matmul.cpp 里固定生成
- 运行次数：5 次
- 指标：中位数耗时，单位 ms
- 编译模式：Release，-O3

## 2. 构建 benchmark

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target bench_matmul
.\build\bench_matmul.exe
```

如果你的终端 CMake 正常，应该看到类似：

```text
run 1: ...
run 2: ...
...
size: 512x512
median: ... ms
checksum: ...
```

## 3. 先记录基线

把这次 median 数字写进：

```text
docs/benchmark.md
```

优化前表格。

不要先改代码。先把基线固定下来。

## 4. 下一步：改循环顺序

当前 `Tensor::matmul` 是：

```text
i-j-k
```

也就是：

```cpp
for i:
    for j:
        for k:
            C[i][j] += A[i][k] * B[k][j]
```

问题：

- B 是按列访问的；
- 内存是一行一行连续存的；
- 按列访问 cache 不友好。

优化成：

```text
i-k-j
```

也就是：

```cpp
C.fill(0);
for i:
    for k:
        const float a = A[i][k];
        for j:
            C[i][j] += a * B[k][j]
```

这样：

- A 按行访问；
- B 按行访问；
- C 按行访问；
- 三个矩阵的访问都更连续，cache 命中率更高。

## 5. 优化后再测一次

改完 `src/tensor.cpp` 的 `matmul` 后：

```powershell
cmake --build build --target bench_matmul
.\build\bench_matmul.exe
```

把新的 median 写进 `docs/benchmark.md`。

## 6. 必须解释为什么快

最终 `docs/benchmark.md` 要能回答：

- 优化前访问 B 的方式为什么慢？
- 优化后访问顺序为什么对 cache 更友好？
- 数字从多少降到多少？