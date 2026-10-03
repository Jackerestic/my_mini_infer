# M1 数值对齐验证报告

> 没有原始输出和数字，不算完成。

## 1. 环境

- 日期：2026-10-01
- 编译器：g++ 8.1.0 (MinGW-w64)
- 操作系统：Windows
- 数据文件：data/mlp.txt

## 2. 参考数据生成

固定输入和权重：

```powershell
python tools/gen_weights.py
```

测试时独立重算 h/o/y：

```powershell
python tools/recompute_ref.py
```

`recompute_ref.py` 只读取 x、W1、b1、W2、b2，不读取 data/mlp.txt 里的 h、o、y。
参考值由独立脚本在测试时重新计算。

随机种子：42

## 3. C++ 对拍结果

| 中间结果 | 最大绝对误差 | 是否小于 1e-5 |
| --- | --- | --- |
| h | 2.38419e-07 | 是 |
| o | 4.76837e-07 | 是 |
| y | 1.49012e-08 | 是 |

## 4. C++ 原始输出

```text
x 3 4
W1 6 4
b1 1 6
W2 3 6
b2 1 3
h 3 6
o 3 3
y 3 3
h max error = 2.38419e-07
o max error = 4.76837e-07
y max error = 1.49012e-08
M1 PASS
```

## 5. 结论

- [x] h 误差小于 1e-5
- [x] o 误差小于 1e-5
- [x] y 误差小于 1e-5
- [x] M1 PASS

## 6. 备注

- 卡住的地方：文件读取的 TEMP 权限，已通过把 TEMP/TMP 指向 build/tmp 解决。
- 下一步：M2 工程化。