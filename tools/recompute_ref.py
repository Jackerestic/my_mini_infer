"""Independently recompute the MLP reference with NumPy.

Rules:
- Do not modify tools/gen_weights.py.
- Read only x, W1, b1, W2, b2 from data/mlp.txt.
- Do not read or use h, o, y from data/mlp.txt.
- Compute the forward pass with NumPy.
- Write h, o, y to data/mlp_numpy_ref.txt.
"""

from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parent.parent
INPUT_PATH = ROOT / "data" / "mlp.txt"
OUTPUT_PATH = ROOT / "data" / "mlp_numpy_ref.txt"

NEEDED = {"x", "W1", "b1", "W2", "b2"}


def load_inputs(path: Path) -> dict[str, np.ndarray]:
    """Read only the five input tensors. Skip h, o, y."""
    tensors: dict[str, np.ndarray] = {}

    with path.open("r", encoding="utf-8") as fin:
        while True:
            header = fin.readline()
            if not header:
                break

            parts = header.split()
            if len(parts) != 3:
                raise ValueError(f"bad header: {header!r}")

            name = parts[0]
            rows = int(parts[1])
            cols = int(parts[2])

            if name in NEEDED:
                values = []
                for _ in range(rows):
                    row = fin.readline().split()
                    if len(row) != cols:
                        raise ValueError(f"bad row width for {name}")
                    values.append([float(x) for x in row])
                tensors[name] = np.array(values, dtype=np.float32)
            else:
                for _ in range(rows):
                    fin.readline()

            if NEEDED.issubset(tensors):
                break

    missing = NEEDED - tensors.keys()
    if missing:
        raise ValueError(f"missing tensors: {sorted(missing)}")

    return tensors


def relu(t: np.ndarray) -> np.ndarray:
    return np.maximum(t, np.float32(0.0))


def softmax(t: np.ndarray) -> np.ndarray:
    shifted = t - t.max(axis=1, keepdims=True)
    e = np.exp(shifted)
    return e / e.sum(axis=1, keepdims=True)


def dump(name: str, array: np.ndarray) -> list[str]:
    a = np.atleast_2d(array)
    lines = [f"{name} {a.shape[0]} {a.shape[1]}"]
    for row in a:
        lines.append(" ".join(f"{float(v):.9g}" for v in row))
    return lines


def main() -> int:
    tensors = load_inputs(INPUT_PATH)

    x = tensors["x"]
    W1 = tensors["W1"]
    b1 = tensors["b1"]
    W2 = tensors["W2"]
    b2 = tensors["b2"]

    h = relu(x @ W1.T + b1)
    o = relu(h @ W2.T + b2)
    y = softmax(o)

    lines: list[str] = []
    for name, array in [("h", h), ("o", o), ("y", y)]:
        lines += dump(name, array)

    OUTPUT_PATH.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"written: {OUTPUT_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())