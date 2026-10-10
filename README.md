# Mini Infer

![C++ CI](https://github.com/Jackerestic/my_mini_infer/actions/workflows/ci.yml/badge.svg)

用 C++17 从零实现的最小神经网络推理引擎，支持 MLP 前向、批处理推理和 HTTP 服务。

## Features

- Tensor
- Matrix multiplication
- ReLU / Softmax / Linear
- MLP forward
- NumPy reference verification
- Catch2 tests
- Benchmark
- HTTP inference server

## Architecture

```mermaid
flowchart LR
    X["Input x<br/>(N, 4)"] --> L1["Linear W1, b1"]
    L1 --> R1["ReLU"]
    R1 --> L2["Linear W2, b2"]
    L2 --> R2["ReLU"]
    R2 --> S["Softmax"]
    S --> Y["Output y<br/>(N, 3)"]
```

## Requirements

- CMake >= 3.15
- Ninja
- C++17 compiler
  - MSVC 2019+
  - GCC 8+
  - Clang 10+

## Quick Start

Run the following commands from the project root:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## HTTP Inference Server

Start the server:

```powershell
.\build\infer_server.exe
```

Start the server in one terminal. Keep it running, then use another terminal for the `curl` commands.

Check health:

```powershell
curl.exe http://127.0.0.1:8080/health
```

Run a prediction:

```powershell
$body = "0.30 -1.04 0.75 0.94`n-1.95 -1.30 0.13 -0.32"
curl.exe -X POST -H "Content-Type: text/plain" --data-binary $body http://127.0.0.1:8080/predict
```

Example output:

```text
0.102567 0.0378496 0.859583
0.0061975 0.00838873 0.985414
```

## Tests

```powershell
cmake --build build
ctest --test-dir build -N
ctest --test-dir build --output-on-failure
```

## Benchmark

See [Benchmark report](docs/benchmark.md).

- i-k-j loop order: about 5x stable speedup
- inline at(): removes cross-TU function-call overhead

## Project Layout

```text
mini_infer/
├─ include/mini_infer/
├─ src/
├─ tests/
├─ benchmarks/
├─ examples/
├─ third_party/
└─ docs/
```

## Limitations

- Only 2D Tensor
- Only a fixed two-layer MLP
- Single-threaded HTTP server
- No JSON API
- No CUDA or quantization

## Documentation

- [M1 numerical verification](docs/verify.md)
- [Benchmark report](docs/benchmark.md)
- [M4 plan](docs/m4_plan.md)

## License

[MIT](LICENSE)