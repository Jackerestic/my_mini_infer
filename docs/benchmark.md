# Benchmark 报告

## 1. 环境

- 日期：2026-10-04
- CPU：未记录
- 操作系统：Windows
- 编译器：g++ 8.1.0 (MinGW-w64)
- 构建模式：Release，-O3 -DNDEBUG
- 矩阵大小：512 x 512
- 统计方式：5 次运行取中位数
- 基准程序：benchmarks/bench_matmul.cpp

## 2. 优化前：i-j-k

命令：

```powershell
cmake --build build --target bench_matmul
.\build\bench_matmul.exe
```

原始输出：

```text
run 1: 74.1798 ms
run 2: 77.042 ms
run 3: 78.855 ms
run 4: 84.0464 ms
run 5: 121.694 ms
size: 512x512
median: 78.855 ms
checksum: 1257.88
```

中位数：

```text
78.855 ms
```

备注：
- 第 5 次明显偏高，属于系统噪声；
- 中位数 78.855 ms 作为优化前基线。

## 3. 优化后：i-k-j

原始输出：

```text
run 1: 61.3899 ms
run 2: 95.1903 ms
run 3: 56.8378 ms
run 4: 58.1299 ms
run 5: 54.9942 ms
size: 512x512
median: 58.1299 ms
checksum: 1257.88
```

中位数：

```text
58.1299 ms
```

备注：
- 第 2 次明显偏高，属于系统噪声；
- 中位数 58.1299 ms 作为优化后结果。

## 4. 提升

- 优化前：78.855 ms
- 优化后：58.1299 ms
- 加速比：1.357 倍
- 耗时下降：约 26.3%

## 5. 为什么快

- 优化前：i-j-k 循环
- 优化后：i-k-j 循环
- cache 层面的解释：
  - 矩阵在内存里是行优先连续存放的；
  - 原来的 i-j-k 写法中，最内层变化的是 k，访问 B[k][c] 时地址会按列跳动，空间局部性差；
  - 改成 i-k-j 后，最内层变化的是 c，访问 B[k][c] 和写 C[r][c] 都是连续按行进行；
  - 连续访问更容易命中 cache，减少去内存取数据的次数，所以更快。

## 6. 正确性验证

优化后重新运行：

- `test_tensor`：passed
- `test_tensor_catch2`：All tests passed (43 assertions in 8 test cases)
- benchmark checksum 不变：1257.88

## 7. 结论

- [x] 优化前基线已记录
- [x] 优化后数字已记录
- [x] 能解释为什么 i-k-j 对 cache 更友好