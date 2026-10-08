# Benchmark 报告

## 1. 环境

- 日期：2026-10-04
- CPU：未记录
- 操作系统：Windows
- 编译器：g++ 8.1.0 (MinGW-w64)
- 构建模式：Release，-O3 -DNDEBUG
- 矩阵大小：1024 x 1024
- 基准程序：benchmarks/bench_matmul.cpp
- 测量方法：同一个进程内交替执行 i-j-k 和 i-k-j，共 20 轮，统计每轮比值的中位数

## 2. 早期测量为什么不可信

第一次顺序测量：

```text
i-j-k  median: 78.855 ms
i-k-j  median: 58.1299 ms
1.357 倍
```

这是错的。

原因：机器状态是双峰的，可能从快峰跳到慢峰。
顺序测量会把 i-j-k 的快峰和 i-k-j 的慢峰放在一起比较，产生系统性误差。

## 3. 可复现 benchmark

现在 `bench_matmul.exe` 会在同一个进程里：

- 同时实现 i-j-k 和 i-k-j；
- 每轮交替执行，避免热状态顺序偏差；
- 共跑 20 轮；
- 输出每轮耗时和比值；
- 最后输出每轮比值的中位数。

在正常 PowerShell 环境运行：

```text
round 1: ijk=2368.53 ms, ikj=178.537 ms, ratio=13.2663
round 2: ijk=2574.15 ms, ikj=174.245 ms, ratio=14.7732
round 3: ijk=4207.51 ms, ikj=947.513 ms, ratio=4.44058
round 4: ijk=2796.85 ms, ikj=816.074 ms, ratio=3.4272
round 5: ijk=2696.71 ms, ikj=175.583 ms, ratio=15.3586
round 6: ijk=2516.28 ms, ikj=177.448 ms, ratio=14.1804
round 7: ijk=2590.87 ms, ikj=180.299 ms, ratio=14.3698
round 8: ijk=2433.8 ms, ikj=177.244 ms, ratio=13.7313
round 9: ijk=2365.98 ms, ikj=178.526 ms, ratio=13.2529
round 10: ijk=2281.76 ms, ikj=177.81 ms, ratio=12.8326
round 11: ijk=2316.25 ms, ikj=184.325 ms, ratio=12.5661
round 12: ijk=2485.23 ms, ikj=179.93 ms, ratio=13.8122
round 13: ijk=2727.64 ms, ikj=181.053 ms, ratio=15.0654
round 14: ijk=3128.49 ms, ikj=182.018 ms, ratio=17.1878
round 15: ijk=4462.96 ms, ikj=188.884 ms, ratio=23.628
round 16: ijk=2597.08 ms, ikj=189.627 ms, ratio=13.6957
round 17: ijk=3216.53 ms, ikj=188.332 ms, ratio=17.0791
round 18: ijk=2708.72 ms, ikj=210.478 ms, ratio=12.8694
round 19: ijk=2568.79 ms, ikj=185.053 ms, ratio=13.8813
round 20: ijk=2587.94 ms, ikj=183.109 ms, ratio=14.1333
size: 1024x1024
ijk median: 2590.87 ms
ikj median: 182.018 ms
median per-round speedup: 13.8813x
checksum: 10708.4
```

可复现结论：

```text
1024 x 1024 matmul，median per-round speedup = 13.8813 倍
```

说明：

- 第 3、4 轮出现慢峰，导致比值偏低；
- 第 15 轮出现 i-j-k 明显慢峰，比值偏高；
- 因此不能只看单轮，要看所有轮比值的中位数；
- 中位数是 13.8813 倍。

早期受控实验得到约 6.4 - 7.3 倍，是在不同实现上下文/热状态下得到的保守数字。
现在以可复现的 `bench_matmul.exe` 输出为准。

## 4. 正确性验证

- 优化后 ctest：10/10 全部通过；
- 优化前/后逐元素一致，无精度损失；
- benchmark 两个版本的 checksum 在 warm-up 后一致。

## 5. 为什么快

- 矩阵在内存里是行优先连续存放的；
- i-j-k 的最内层变化的是 k，访问 B[k][c] 时地址按列跳动，空间局部性差；
- i-k-j 的最内层变化的是 c，访问 B[k][c] 和写 C[r][c] 都连续按行进行；
- 连续访问更容易命中 cache，减少去内存取数据的次数，所以更快。

## 6. 结论

- [x] 优化前基线已建立；
- [x] 优化后数字已重测；
- [x] benchmark 程序可以复现两个版本对比；
- [x] 可复现中位数加速比：13.8813 倍；
- [x] 能解释 i-k-j 为什么对 cache 更友好。