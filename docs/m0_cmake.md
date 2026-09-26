# M0 工具链：安装并使用 CMake

## 1. 目标

不靠 build.bat，自己完成：

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## 2. 当前状态（2026-09-25 已就绪）

- g++ 8.1.0 可用：`..\tools\mingw64\bin\g++.exe`（工作区自带）
- mingw32-make 4.2.1 可用：`..\tools\mingw64\bin\mingw32-make.exe`
- **cmake 4.4.3 已就位**：`..\tools\cmake\bin\cmake.exe`
- **ninja 1.13.2 已就位**：`..\tools\cmake\bin\ninja.exe`
- **ctest 4.4.3 已就位**：`..\tools\cmake\bin\ctest.exe`

也就是说：安装这一步**已经做完了**，你要做的是「自己会用」，
不是「自己会装」。先按第 6 节把环境激活，然后直接从第 4 节开始练。

顺带确认一下「装了」和「能用」的区别（这是 9/20 那次的教训）：

```powershell
. ..\tools\doctor.ps1 -Full
```

它会真的编译、真的运行、真的跑 CTest。最后看到「环境可用」才算数。

## 3. CMake 是怎么装上的（背景知识，不用重做）

工作区这份 CMake 来自 pip 包，不需要管理员权限：

```powershell
python -m pip install cmake
```

但**只把 `pip` 装出来的那个 `cmake.exe` 复制进工作区是不够的**：
`Scripts\cmake.exe` 只有 108 KB，它其实是个启动器，真正的 CMake
（12.8 MB 的 `cmake.exe` + `share\cmake-4.4\Modules\` 4000 多个模块文件）
还在 Python 的 `site-packages\cmake\data\` 里。
只复制启动器的话，工具链就偷偷依赖 Python 了，「自给自足」是假的。

所以正确做法是把整个 `data\` 目录都复制过去：

```powershell
$data = "$env:LOCALAPPDATA\Python\pythoncore-3.14-64\Lib\site-packages\cmake\data"
robocopy $data 'C:\Users\文涵超\Desktop\实习日志\tools\cmake' /E
```

**怎么验证它是独立的**（而不是又抄了个启动器）：让 CMake 自己报根目录。

```powershell
# 建一个 probe.cmake，内容就一行：
#   message(STATUS "CMAKE_ROOT=${CMAKE_ROOT}")
cmake -P probe.cmake
```

期望输出里 `CMAKE_ROOT` 指向 `...\实习日志\tools\cmake\share\cmake-4.4`。
如果它指向 Python 的 `site-packages`，那就还是启动器，没复制对。

## 4. 构建项目

进入项目根目录：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
```

配置：

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

构建：

```powershell
cmake --build build
```

测试：

```powershell
ctest --test-dir build --output-on-failure
```

如果成功，最后会看到类似：

```text
100% tests passed, 0 tests failed out of 1
```

## 5. 记录结果

完成后，在 docs/self_check.md 第四部分填写：

- 日期
- 能不能独立完成
- 实际结果
- 报错

如果失败，不要连续折腾超过 40 分钟。直接把命令和报错发我。
---

## 6. 环境已经准备好的部分

工作区的 `tools\cmake\` 里是一份**完整独立**的 CMake：

```text
C:\Users\文涵超\Desktop\实习日志\tools\cmake\bin\cmake.exe       (12.8 MB，真身)
C:\Users\文涵超\Desktop\实习日志\tools\cmake\bin\ctest.exe
C:\Users\文涵超\Desktop\实习日志\tools\cmake\bin\cpack.exe
C:\Users\文涵超\Desktop\实习日志\tools\cmake\bin\cmake-gui.exe
C:\Users\文涵超\Desktop\实习日志\tools\cmake\bin\ninja.exe
C:\Users\文涵超\Desktop\实习日志\tools\cmake\share\cmake-4.4\Modules\   (模块)
```

环境脚本：

```text
C:\Users\文涵超\Desktop\实习日志\tools\env.ps1    (PowerShell)
C:\Users\文涵超\Desktop\实习日志\tools\env.cmd    (cmd)
```

**新开一个 PowerShell 窗口，直接验证就行**（工具链已经在用户级 PATH 里，
不需要 `Set-ExecutionPolicy`，也不需要点源脚本）：

```powershell
cmake --version
ctest --version
```

期望看到 `cmake version 4.4.3` 和 `ctest version 4.4.3`。

> 这套「零激活」是怎么来的（一次性设置，见 `docs/m0_toolchain.md` 第 1 节）：
>
> ```powershell
> . ..\tools\install-path.ps1                                          # 写进用户级 PATH
> Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned  # 本地 .ps1 放行
> ```
>
> 如果这两个都没做，或者你在一个**老窗口**里（PATH 是开窗口那一刻定下的），
> 才需要临时补一下：
>
> ```powershell
> Set-ExecutionPolicy -Scope Process Bypass -Force
> . ..\tools\env.ps1
> ```
>
> 纯粹手动加 PATH 也可以：
>
> ```powershell
> $env:Path = "C:\Users\文涵超\Desktop\实习日志\tools\cmake\bin;C:\Users\文涵超\Desktop\实习日志\tools\mingw64\bin;$env:Path"
> ```

## 7. 中文路径这件事（重要，别踩）

项目路径里有中文（`文涵超` / `实习日志`），这不是小事：

- **MinGW Makefiles 生成器**：`mingw32-make` 处理中文路径不稳，容易报
  CreateProcess 失败。**所以用 Ninja 生成器**（`-G Ninja`），Ninja 没这个问题。
  这就是本项目的工具链里专门放了一份 `ninja.exe` 的原因。
- **编译器本身**：winlibs 出的新版 GCC（如 16.2.0）配的新 binutils
  在中文路径下会直接编不过（`can't create ...o` / `cannot find crt2.o`）。
  本项目因此固定用 GCC 8.1.0。完整证据和边界表见 `docs/m0_toolchain.md` 第 4 节。

确认 Ninja 在不在：

```powershell
ninja --version     # 期望 1.13.2
```

然后用 Ninja 配置：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## 8. 如果还是失败

不要连续折腾超过 40 分钟。把下面三样发我：

1. 完整命令；
2. 完整报错；
3. 已经试过什么。

我会只针对卡住的那一步给下一步。