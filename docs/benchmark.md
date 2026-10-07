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
把完整输出贴在这里
```

中位数：

```text
__ ms
```

## 4. 提升

- 优化前：78.855 ms
- 优化后：__ ms
- 加速比：__ 倍

## 5. 为什么快

- 优化前：
- 优化后：
- cache 层面的解释：

## 6. 结论

- [x] 优化前基线已记录
- [ ] 优化后数字已记录
- [ ] 能解释为什么 i-k-j 对 cache 更友好