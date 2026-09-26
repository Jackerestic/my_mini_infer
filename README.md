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
| `g++` 8.1.0 (MinGW-w64) | 编译 C++17 | `..\tools\mingw64\bin` |
| `mingw32-make` 4.2.1 | make | `..\tools\mingw64\bin` |
| `gdb` 8.1 | 调试 | `..\tools\mingw64\bin` |
| `cmake` 4.4.3 | 生成构建系统 | `..\tools\cmake\bin` |
| `ninja` 1.13.2 | 实际执行构建 | `..\tools\cmake\bin` |
| `ctest` 4.4.3 | 跑测试 | `..\tools\cmake\bin` |

**整个工具链都放在工作区的 `tools\` 里，没有装到系统里。** 这是有意的：

- 不污染系统环境，也不需要管理员权限；
- 2026-09-20 系统级的 MinGW（装在 `C:\main\...`）被项目之外的东西删掉了，
  构建无缘无故就坏了。放进工作区之后，构建就自给自足了。

> **为什么 g++ 是 8.1.0 而不是更新的版本？**
>
> 本项目路径里带中文（`文涵超` / `实习日志`）。试过 winlibs 的 GCC 16.2.0：
> 它配的新版 binutils（`as.exe` / `ld.exe`）会把这些中文路径读成乱码，
> 一编译就报 `can't create ...\ccXXXX.o` 或 `cannot find crt2.o`。
> GCC 8.1.0 配的是老 binutils，对中文路径反而是好的。
> 而且 GCC 8 是第一个完整支持 C++17 的版本，本项目要的就是 C++17。
>
> 完整的排查过程和证据在 `m0/docs/m0_toolchain.md` 第 4 节。

**现在新开窗口就能直接用，不需要每次激活。** 工具链已经写进用户级 PATH
（`HKCU\Environment`，不需要管理员权限），新开的 PowerShell / cmd / VS Code
里 `g++` / `cmake` / `ninja` / `ctest` 直接可用：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
cmake --version
```

只做了两件一次性的事（2026-09-25）：

```powershell
# 1) 把工作区工具链写进用户级 PATH（可重复执行；-Remove 撤销；-Show 只看）
. ..\tools\install-path.ps1

# 2) 当前用户的执行策略改成 RemoteSigned，让本地 .ps1 能直接跑
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```

> 为什么以前每次都要敲 `Set-ExecutionPolicy -Scope Process Bypass -Force` 和
> `. ..\tools\env.ps1`？因为那两行都是「**只对当前窗口有效**」的临时补丁：
> `$env:Path` 是进程级的，窗口一关就没了；执行策略默认是 Restricted，`.ps1`
> 会被拒绝。现在改成一次性的用户级设置，就不用每次都来一遍了。
> 详细对比见 `m0/docs/m0_toolchain.md` 第 1 节。

`env.ps1` / `env.cmd` **没有删**，留作「临时救急」：比如你不想动任何永久设置、
或者在别人的机器上。用法不变：

```powershell
. ..\tools\env.ps1        # PowerShell（开头那个点表示点源，不能省）
call ..\tools\env.cmd     # cmd
```

激活脚本会自己打印一遍每个工具的版本和实际路径。如果还想确认工具是**真的能用**，
而不只是「文件躺在磁盘上」，再跑一次自检：

```powershell
. ..\tools\doctor.ps1          # 工具版本 + 真编一个小程序并运行
. ..\tools\doctor.ps1 -Full    # 再加 CMake + Ninja + CTest 全链路
```

> **如果 PowerShell 还是报 `running scripts is disabled on this system`**
>
> 说明第 2 步没做（或者被别的策略覆盖了）。先执行下面这行只放宽当前窗口：
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
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer

.\build.bat            # g++ 直接编译（最快）
.\build.bat cmake      # 完整 CMake + Ninja + CTest
.\build.bat test       # 只跑已经编译好的测试
.\build.bat clean      # 删掉 build\
```

`build.bat` 会自己去 `..\tools\` 找 CMake 和 Ninja，所以**不用**先激活 `env.ps1`。

### 方式三：手动跑 CMake

工具链已经在用户级 PATH 里，**新开一个窗口直接就能跑**：

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

---

## 2026-09-25 v2 计划更新

当前最高优先级不是继续加新算子，而是先完成 M0 语言与工具地基。
请先看：

- 项目根目录 `TODO.md`：当前只做 M0 的操作清单；
- `m0/docs/self_check.md`：M0 自我检查与代码讲解模板；
- `docs/TODO_M1_after_11_01.md`：M1 数值对齐操作单，11/1 后再开始；
- `m0/docs/m0_toolchain.md`：工具链速查（有什么、在哪、坏了怎么修、踩过哪些坑）。

## 2026-09-25 环境状态（实测通过）

- 工作区里的工具链已经补齐并自验：`g++ 8.1.0` + `cmake 4.4.3` + `ninja 1.13.2` + `ctest 4.4.3`。
- 三种构建方式全部通过：`build.bat gpp` / `build.bat test` / `build.bat cmake`。
- 自检命令：`. ..\tools\doctor.ps1 -Full`（会真的编译、运行、跑 CTest）。
- 顺带确认：Python 3.14.6 + numpy 2.5.2 + **PyTorch 2.14.0** 都可用，
  M1 的 `tools/gen_weights.py` 对拍可以跑起来。