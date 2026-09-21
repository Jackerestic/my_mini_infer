"""用 numpy 生成一组随机 MLP 权重和期望输出，供 C++ 引擎对拍。

这是「独立实现」：和 C++ 用完全相同的数学，但由另一个库算出来。
两边结果一致，才能说明 C++ 引擎写对了。

用法：
    cd my_mini_infer
    python tools/gen_weights.py

输出：
    data/mlp.txt
"""

from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parent.parent
OUT_PATH = ROOT / "data" / "mlp.txt"

# 固定种子 -> 每次生成同一组数据，方便复现
rng = np.random.default_rng(42)

N, IN, HID, OUT = 3, 4, 6, 3

# standard_normal: 标准正态分布（均值 0、方差 1）
# 会产生正负都有的一批数，这样 ReLU 才真的起作用
x = rng.standard_normal((N, IN)).astype(np.float32)
W1 = rng.standard_normal((HID, IN)).astype(np.float32)
b1 = rng.standard_normal((1, HID)).astype(np.float32)
W2 = rng.standard_normal((OUT, HID)).astype(np.float32)
b2 = rng.standard_normal((1, OUT)).astype(np.float32)

# 注意 dtype 是 float32，和 C++ 里的 float 一致。
# 如果用 float64，算出来的"标准答案"会比 C++ 更精确，
# 对比时就会出现本来不该有的差异。


def relu(t):
    return np.maximum(t, 0.0)


def softmax(t):
    # 沿 axis=1（每一行）做，减掉行内最大值 —— 和 C++ 完全一样的算法
    e = np.exp(t - t.max(axis=1, keepdims=True))
    return e / e.sum(axis=1, keepdims=True)


# 前向：Linear -> ReLU -> Linear -> ReLU -> Softmax
h = relu(x @ W1.T + b1)  # W1.T 是因为 W1 是 (out, in)
o = relu(h @ W2.T + b2)
y = softmax(o)

# ---- 数据质量检查 ----
# 「随机数据」不等于「好的测试数据」。
# 如果 ReLU 把绝大多数隐藏单元都关掉，第二层的大部分列就没被用到，
# 索引写错也发现不了，测试就形同虚设。
firing = int((h > 0).sum())
total = int(h.size)
print(f"ReLU 之后激活的隐藏单元: {firing}/{total}")
if firing < total // 2:
    raise SystemExit("激活单元太少，测试覆盖不够：换个种子或调大维度")
if (o > 0).sum() == 0:
    raise SystemExit("第二层输出全是 0，ReLU 之后没有信息了")


def dump(name, arr):
    """把一个二维数组变成文件里的几行：名字 行 列 + 数据。"""
    a = np.atleast_2d(arr)  # (2,) 这种一维数组升成 (1, 2)
    out = [f"{name} {a.shape[0]} {a.shape[1]}"]
    for row in a:
        # %.9g 对 float32 来说精度足够，能原样读回
        out.append(" ".join(f"{v:.9g}" for v in row))
    return out


lines = []
for name, arr in [
    ("x", x),
    ("W1", W1),
    ("b1", b1),
    ("W2", W2),
    ("b2", b2),
    # 中间结果也存下来，C++ 那边可以逐层对拍，
    # 一眼看出是哪一层开始算错的
    ("h", h),
    ("o", o),
    ("y", y),
]:
    lines += dump(name, arr)

OUT_PATH.write_text("\n".join(lines) + "\n", encoding="utf-8")

print(f"written: {OUT_PATH}")
print()
print("--- 期望输出 y（softmax 之后的概率，每行加起来 = 1）---")
print(y)
print()
print("--- 中间结果，C++ 跑不出来时可以拿来定位是哪一层 ---")
print("h (第一层 relu 之后) =")
print(h)
print("o (第二层 relu 之后) =")
print(o)
