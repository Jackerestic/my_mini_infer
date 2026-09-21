# my_mini_infer

用 C++ 从零实现一个极小的神经网络推理引擎，作为 AI Infra 入门项目。

目标不是一开始就支持大模型，而是先把最核心的东西跑通：

- Tensor：多维数组；
- 矩阵乘法：matmul；
- 线性层：y = Wx + b；
- 激活函数：ReLU；
- Softmax；
- MLP 前向推理；
- 内存池、benchmark、多线程、HTTP 服务。

## 第一阶段只做这些

1. Tensor 构造、shape、data、at、print；
2. Tensor::add；
3. Tensor::matmul；
4. 单元测试；
5. CMake 或 build.bat 编译通过。

暂时不碰：

- 反向传播；
- 梯度下降；
- 训练；
- CUDA；
- Transformer；
- 大模型。

## 目录结构

```text
my_mini_infer/
├─ CMakeLists.txt
├─ build.bat
├─ README.md
├─ include/mini_infer/tensor.hpp
├─ src/tensor.cpp
├─ src/main.cpp
├─ tests/test_tensor.cpp
├─ docs/roadmap.md
├─ docs/nn_primer.md
├─ logs/log.md
├─ data/
└─ examples/
```

## 工具链

本项目需要：

| 工具 | 用途 | 位置 |
| --- | --- | --- |
| `g++` 16.2.0 (MinGW-w64) | 编译 C++17 | `..\tools\mingw64\bin` |
| `mingw32-make` 4.4.1 | make | `..\tools\mingw64\bin` |
| `cmake` 4.4.3 | 生成构建系统 | `..\tools\cmake\bin` |
| `ninja` 1.13.2 | 实际执行构建 | `..\tools\cmake\bin` |
| `ctest` 4.4.3 | 跑测试 | `..\tools\cmake\bin` |

**整个工具链都放在工作区的 `tools\` 里，没有装到系统里。** 这是有意的：

- 不污染系统环境，也不需要管理员权限；
- 2026-09-20 系统级的 MinGW（装在 `C:\main\...`）被项目之外的东西删掉了，
  构建无缘无故就坏了。放进工作区之后，构建就自给自足了。

用之前先激活（只对当前窗口有效）：

```powershell
cd C:\Users\文涵超\Desktop\dsh_agent

# PowerShell（注意开头有个点，表示点源）
. .\tools\env.ps1

# cmd
call tools\env.cmd
```

激活后 `g++` / `cmake` / `ninja` / `ctest` 就能直接用了。

> **如果 PowerShell 报错 `running scripts is disabled on this system`**
>
> 这是 Windows 默认的执行策略（Restricted），不是文件坏了。先执行下面这行，
> 它只放宽当前这个窗口，关掉就恢复：
>
> ```powershell
> Set-ExecutionPolicy -Scope Process Bypass -Force
> ```
>
> 不想改策略也可以直接用 `cmd`，或者干脆用 `build.bat`（见下）。

> **如果 PowerShell 报错 `running scripts is disabled on this system`**
>
> 这是 Windows 默认的执行策略（Restricted），不是文件坏了。先执行下面这行，
> 它只放宽当前这个窗口，关掉就恢复：
>
> ```powershell
> Set-ExecutionPolicy -Scope Process Bypass -Force
> ```
>
> 不想改策略也可以直接用 `cmd`，或者干脆用 `build.bat`（见下）。

### 维护提示：脚本编码

这台机器上踩过的两个坑，改脚本时注意：

- `.ps1` 存成 UTF-8 **不带** BOM 时，Windows PowerShell 5.1 会按 GBK 解析，
  中文注释会乱码，甚至把引号吃掉导致语法错误。
  所以 `tools\env.ps1` 是 **UTF-8 with BOM**，别用「UTF-8」重新保存。
- `.bat` / `.cmd` 反过来**不能**有 BOM，而且 cmd.exe 按 GBK 解析文件，
  所以里面只写 ASCII 英文。中文说明统一放在这个 README 里。

## 怎么编译

### 方式一：双击

双击 `build.bat`。默认走 g++，不需要 CMake。

编译出来的 `build\mini_infer_app.exe` 是**静态链接**的（`g++ -static`），
可以直接双击运行，不需要 MinGW 在 PATH 上。
不加 `-static` 的话它会依赖 MinGW 的运行时 DLL，
双击时报 `0xC0000135`（DLL not found）。

### 方式二：命令行

```powershell
cd C:\Users\文涵超\Desktop\dsh_agent\my_mini_infer

.\build.bat            # g++ 直接编译（最快）
.\build.bat cmake      # 完整 CMake + Ninja + CTest
.\build.bat test       # 只跑已经编译好的测试
.\build.bat clean      # 删掉 build\
```

`build.bat` 会自己去 `..\tools\` 找 CMake 和 Ninja，所以**不用**先激活 `env.ps1`。

### 方式三：手动跑 CMake

先按上面的「工具链」一节激活环境，然后：

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## 当前任务

阶段 0（Tensor 基础）已完成：

- [x] 实现 `Tensor::add`
- [x] 实现 `Tensor::matmul`
- [x] 为 add 和 matmul 写测试
- [x] 每次完成后在 `logs/log.md` 写一行记录

阶段 1（线性层与 MLP 前向）：

基本算子已完成：

- [x] `Tensor::relu`
- [x] `Tensor::transpose`
- [x] `Tensor::softmax`
- [x] `linear(x, W, b)`

下一步：把上面的算子串成完整 MLP 前向，逐层对照手算值：

- [x] `test_mlp()`：Linear → ReLU → Linear → Softmax

详见 `docs/nn_primer.md` 第 12 节（MLP 前向的形状表）、第 14 节（路线）
和第 15-21 节（为什么这些函数就是神经网络）以及 `docs/roadmap.md`。

## 学习原则

- 先写代码，再补概念；
- 每个函数都要能手算验证；
- 不复制不理解的大段模板；
- 卡住时先写清楚卡在哪一步。
