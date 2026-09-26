# M0 工具链速查（环境是怎么搭的、坏了怎么修）

> 这份文件回答三个问题：
> 1. 现在这套工具链到底有什么、放在哪、怎么用；
> 2. 为什么这么放（踩过什么坑）；
> 3. 工具被删了 / 换台机器，怎么 10 分钟恢复。

## 1. 一句话速查（现在不用再激活了）

工具链已经写进**用户级 PATH**，所以新开的窗口直接就能用：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
cmake --version     # 直接就有
```

日常构建三连（M0 要求能不查笔记敲出来）：

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

想确认环境是活的（可选，不是每次都要做）：

```powershell
. ..\tools\doctor.ps1 -Full
```

### 1.1 以前为什么每次都要敲两行

老流程是这样的：

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
. ..\tools\env.ps1
```

两行各解决一个问题，而且**都是「只对当前窗口有效」**：

- `. ..\tools\env.ps1` 改的是 `$env:Path` —— 环境变量是**进程级**的，
  窗口一关就没了，所以新窗口要重来一次；
- `Set-ExecutionPolicy -Scope Process` 只放宽**当前这个进程**的执行策略，
  因为 Windows 默认策略是 Restricted，`.ps1` 会被直接拒绝运行
  （报 `running scripts is disabled on this system`）。

一句话：那两行不是在「装」什么，只是在给这个窗口临时打补丁。

### 1.2 现在怎么做到不用敲

改成一次性、永久的两件事：

```powershell
# 1) 把工具链写进用户级 PATH（注册表 HKCU\Environment）
. ..\tools\install-path.ps1

# 2) 把「当前用户」的执行策略改成 RemoteSigned（本地脚本放行，网上下载的要签名）
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```

两个都**不需要管理员权限**，都只影响当前用户：

| | 老办法（每个窗口） | 新办法（一次性） |
| --- | --- | --- |
| cmake / ninja 能不能用 | 点源 `env.ps1` 之后才行 | 新窗口直接能用 |
| 谁能用 | 只有那一个窗口 | 所有新窗口、cmd、VS Code、CMake GUI |
| `.ps1` 能不能跑 | 加 `-Scope Process Bypass` | 直接跑 |
| 怎么撤销 | 关窗口即可 | `. ..\tools\install-path.ps1 -Remove` + 策略改回 Restricted |

`env.ps1` 没有删，它的用途变成了「**临时救急**」：

- 新开了窗口但 PATH 还没生效（极少数情况）；
- 在别人机器 / 别人的窗口里，不想动任何永久设置；
- 排查问题时想确认「到底是 PATH 的问题还是脚本的问题」。

### 1.3 有个小坑要注意

用户级 PATH 里存的是**绝对路径**，指向工作区里的 `tools\`。
所以：**别随便搬 `实习日志` 这个目录**。搬了以后 PATH 里的两条就指向空目录，
`cmake` 又会找不到；这时重跑一次 `. ..\tools\install-path.ps1` 即可
（它会先删旧条目再加新条目，不会越堆越多）。

## 2. 工具清单（2026-09-25 实测）

| 工具 | 版本 | 位置 | 谁在用 |
| --- | --- | --- | --- |
| `g++` / `gcc` | 8.1.0 (MinGW-W64, posix-seh) | `..\tools\mingw64\bin` | 编译 C++17 |
| `mingw32-make` | GNU Make 4.2.1 | `..\tools\mingw64\bin` | 备用生成器 |
| `gdb` | 8.1 | `..\tools\mingw64\bin` | 调试 |
| `cmake` | 4.4.3（完整独立，`CMAKE_ROOT` 就在工作区） | `..\tools\cmake\bin` | 生成构建系统 |
| `ninja` | 1.13.2 | `..\tools\cmake\bin` | 真正执行构建 |
| `ctest` | 4.4.3 | `..\tools\cmake\bin` | 跑测试 |
| `git` | 2.55.0 | 系统（`Program Files\Git`） | 版本管理 |
| `python` | 3.14.6 | 系统 | 脚本 |
| `numpy` | 2.5.2 | python 包 | M1 对拍要用 |
| `torch` | 2.14.0+cu130 | python 包 | M1 对拍可以直接拿它当标准答案 |

## 2.1 一个容易骗过自己的细节：CMake 是不是「真的」在工作区里

pip 装出来的 `Scripts\cmake.exe` 只有 **108 KB**，它是个启动器，
真正的 CMake（`cmake.exe` 12.8 MB + `share\cmake-4.4\Modules\` 4000 多个模块）
在 Python 的 `site-packages\cmake\data\` 里。
一开始工作区里放的就是那个 108 KB 的启动器 —— 命令能跑、版本也能打，
但它**偷偷依赖 Python**，算不上自给自足。

现在 `tools\cmake\` 里放的是完整的 `data\`（bin + share + doc，93.5 MB）。
验证办法是让 CMake 自己报根目录：

```powershell
'message(STATUS "CMAKE_ROOT=${CMAKE_ROOT}")' | Set-Content C:\Temp\probe.cmake
cmake -P C:\Temp\probe.cmake
# 期望：CMAKE_ROOT=.../实习日志/tools/cmake/share/cmake-4.4
```

**教训**：文件名和版本号都可能是真的，但「依赖谁」要自己验。

另外系统里还有一份 `C:\mingw64`（同样是 GCC 8.1.0），**当作备份**：
工作区那份要是被删了，`build.bat` 会自动退回去用它，构建不会断。

**整个 C++ 工具链都放在工作区的 `tools\` 里，没有装进系统目录。** 这是有意的：

- 不需要管理员权限，不往 `Program Files` / `System32` 里塞东西；
- 2026-09-20 系统级的 MinGW 被项目之外的东西删过一次（还删过工作区里的
  `tools\ninja`），构建无缘无故就坏了。工具链放在工作区里才自给自足。

> 注意区分两件事：
> **「文件放在工作区」** ≠ **「PATH 里不加东西」**。
> 文件确实全在工作区；但为了让命令敲得出来，用户级 PATH 里加了
> 两个指向工作区的条目（见第 1.2 节）。那是用户级的、可一键撤销的，
> 不动系统级 PATH。这就是「零激活」和「不污染系统」之间的折中。

## 3. 四个入口脚本

| 脚本 | 干什么 | 什么时候用 |
| --- | --- | --- |
| `..\tools\install-path.ps1` | 把工作区工具链写进**用户级 PATH**（一次性） | 只做一次；搬过目录之后再跑一次 |
| `..\tools\install-path.ps1 -Remove` | 撤销上面那步 | 不想用了 / 要搬机器 |
| `..\tools\env.ps1` | 把工具链挂到**当前 PowerShell 窗口**的 PATH | 临时救急（正常情况不需要了） |
| `..\tools\env.cmd` | 同上，给 `cmd` 用 | cmd 里临时救急 |
| `..\tools\doctor.ps1` | 自检：版本 + **真编一个程序并运行** +（`-Full`）CMake 全链路 | 怀疑环境坏了、或换机器之后 |

`doctor.ps1` 的冒烟测试**故意在项目目录里编译**，而不是在临时目录。
因为「在别处能编」不等于「在项目里能编」——见下一节。

## 4. 坑一：中文路径 + 新版 binutils（这次踩的大坑）

**现象**：工具装好了、版本也能打印，但一编译就报：

```text
Assembler messages:
Fatal error: can't create C:\Users\�ĺ���\AppData\Local\Temp\ccXXXXXX.o: No such file or directory
ld.exe: cannot find .../crt2.o: No such file or directory
ld.exe: cannot find -lstdc++: No such file or directory
```

那个 `�ĺ���` 就是被搞坏的「文涵超」四个字。

**结论（实测过的边界）**：

| 组合 | 能不能编译 |
| --- | --- |
| GCC 8.1.0，装在中文路径，中文 TEMP，输出到中文目录 | **能** |
| winlibs GCC 16.2.0，装在中文路径 | 不能（`ld` 找不到 crt2.o） |
| winlibs GCC 16.2.0，装在 ASCII 路径，输出到中文目录 | 不能（`as` 建不了 .o） |
| winlibs GCC 16.2.0 + 全 ASCII 路径 + ASCII TEMP | 能 |

**原因**：GCC 自己（cc1plus）能正确处理中文，但它把路径转成 ANSI(GBK) 再交给
`as.exe` / `ld.exe`；新版 binutils（winlibs 的 GCC 16 配的是 binutils 2.47）
按 UTF-8 理解这些字节，于是路径全错。GCC 8.1.0 配的是老 binutils，
两边都是 ANSI，反而一致、反而能用。

**所以这个项目选了 GCC 8.1.0**：

- 项目路径 `C:\Users\文涵超\Desktop\实习日志\my_mini_infer` 天生带中文，
  改不了（用户名就是中文）；
- GCC 8 是第一个**完整支持 C++17** 的版本，项目要求的就是 C++17，够用；
- 换 GCC 16 就得把项目搬到 ASCII 路径、改 TEMP、加目录联接，为了一个
  「版本号更新」付这么多代价不值得。

**教训**：工具链的版本号不是越新越好。**能在你的路径下真的编译 + 真的运行**，
才算能用。这就是 `doctor.ps1` 存在的意义。

> 附：winlibs GCC 16.2.0 目前留在 `C:\mingw64-gcc16`（没有挂到 PATH 上）。
> 要真用它，需要全 ASCII 环境，配方在本文第 7 节。

## 5. 坑二：脚本编码（老坑，别忘）

- `.ps1` 含中文时必须存成 **UTF-8 with BOM**。这台机器的 PowerShell 是 5.1，
  读「UTF-8 无 BOM」会按 GBK 解析 → 中文乱码 + 报
  `The string is missing the terminator`。
  （改完 `env.ps1` / `doctor.ps1` 记得检查 BOM 还在不在。）
- `.bat` / `.cmd` 反过来**不能有 BOM**，而且只能写 ASCII。
- 控制台的 `chcp`：`build.bat` 开头有 `chcp 65001`，否则中文测试名会乱码。

## 6. 坑三：`-static`

`build.bat` 里 g++ 带 `-static`，把 libstdc++ / libgcc / winpthread 打进 exe。
不加的话 exe 依赖 MinGW 运行时 DLL，双击报 `0xC0000135`（DLL not found）。
判断「程序没起来」和「程序跑错了」要看退出码：负数多半是没起来。

## 7. 恢复步骤（工具链被删了怎么办）

### 7.1 只坏了 CMake / Ninja

工作区里的 `tools\cmake` 是完整的一份（bin + share + doc），先确认还在：

```powershell
Test-Path ..\tools\cmake\bin\cmake.exe
Test-Path ..\tools\cmake\bin\ninja.exe
Test-Path ..\tools\cmake\share\cmake-4.4\Modules\FindThreads.cmake
```

丢了就重新装一份，并把**整个 data 目录**复制进工作区（只复制 `cmake.exe`
会得到一个依赖 Python 的启动器，见第 2.1 节）：

```powershell
python -m pip install cmake ninja
$scripts = python -c "import sysconfig; print(sysconfig.get_path('scripts'))"
$data    = "$env:LOCALAPPDATA\Python\pythoncore-3.14-64\Lib\site-packages\cmake\data"
robocopy $data 'C:\Users\文涵超\Desktop\实习日志\tools\cmake' /E
Copy-Item (Join-Path $scripts 'ninja.exe') ..\tools\cmake\bin\ninja.exe -Force
```

### 7.2 只坏了 g++

工作区那份是 `tools\mingw64`（GCC 8.1.0，MinGW-W64 builds 4.3.5）。
丢了的话有两种回退：

1. 先用系统里的 `C:\mingw64`（同样是 8.1.0），`build.bat` 会自动退回去；
2. 想补回工作区，下载 `x86_64-8.1.0-release-posix-seh-rt_v6-rev0` 解压后
   把里面的 `mingw64\` 整个放到 `tools\` 下（改名成 `mingw64`）。

> 注意：**不要**往 `tools\mingw64` 里放 winlibs 的新版 GCC
> （见第 4 节，它在中文路径下必然报 `can't create ... .o`）。
> 一旦放进去，`build.bat` 会优先用它，构建直接坏掉，而且报错很难懂。

### 7.3 用 GCC 16.2.0（可选，只有全 ASCII 路径才行）

```powershell
# 1) 目录联接：给项目和工具链各开一个纯 ASCII 的门
New-Item -ItemType Junction -Path 'C:\mini' -Target 'C:\Users\文涵超\Desktop\实习日志' -ErrorAction SilentlyContinue
# 2) 用 ASCII 路径进入项目
cd C:\mini\my_mini_infer
# 3) 换 ASCII 临时目录（中文 TEMP 会让 as.exe 建不了中间文件）
$env:TEMP = 'C:\Temp'; $env:TMP = 'C:\Temp'
# 4) 用 GCC 16
$env:Path = 'C:\mingw64-gcc16\bin;' + $env:Path
g++ --version
```

## 8. 相关文件

- `..\tools\install-path.ps1`：把工具链写进用户 PATH（一次性）；`-Remove` 撤销；`-Show` 只看
- `..\tools\env.ps1` / `env.cmd`：只给当前窗口临时挂 PATH（救急用）
- `..\tools\doctor.ps1`：自检
- `..\tools\fix-system-path.ps1`：清掉 PATH 里的不可见字符（附录 A）；默认只预览
- `..\tools\path-backup.txt`：`install-path.ps1` 备份的原 PATH 值
- `build.bat`：一条命令搞定 g++ / CMake / 测试
- `docs/m0_cmake.md`：CMake 怎么用（教材）
- `docs/m0_compile_pipeline.md`：一个 .cpp 怎么变成 .exe（教材）

---

## 附录 A. 坑四：PATH 里的不可见字符（2026-09-26）

**现象**：每次新开 PowerShell 都刷一大段 conda 报错：

```text
UnicodeEncodeError: 'gbk' codec can't encode character '\u202a' in position 996
Invoke-Expression : 无法将参数绑定到参数"Command"，因为该参数为空字符串。
                   所在位置 ...\Conda.psm1:83
```

**原因链**（每一环单独看都「没错」）：

1. 系统级 PATH 里有一条 `C:\Program Files\Java\jdk-25.0.4\bin`，
   路径前面多了一个**不可见**的 `U+202A`（LEFT-TO-RIGHT EMBEDDING）。
   这种字符多半是从网页/聊天里复制粘贴路径时一起带进来的。
2. PowerShell 的 profile（`Documents\WindowsPowerShell\profile.ps1`）会跑
   conda 的启动钩子 → `conda activate base`。
3. conda 用 Python 把**整条 PATH** 打印成激活脚本。Windows 上 Python 往管道写时
   默认用 GBK，GBK 编不了 `U+202A` → 抛异常 → conda 崩溃、输出为空
   → 后面那句 `Invoke-Expression -Command ''` 就是第二个错。
4. 以前为什么没看到？因为执行策略是 `Restricted`，**profile 根本没被加载**
   （那时刷的是 `running scripts is disabled`）。把策略改成 `RemoteSigned` 之后
   profile 真的开始跑了，这个一直存在的隐患才露出来。

**怎么找出来**：把它按字符拆开看码点，肉眼是看不见的。

```powershell
$p = [Environment]::GetEnvironmentVariable('Path','Machine')
for ($i = 0; $i -lt $p.Length; $i++) {
    $c = [int]$p[$i]
    if ($c -ge 0x200B -and $c -le 0x2069) { "位置 $i : U+{0:X4}" -f $c }
}
```

### A.1 一个特别阴的坑：`-eq` 看不见它

PowerShell 的 `-eq` 比较字符串时用的是**文化敏感**比较，而 `U+202A` 这类控制字符
在 Unicode 排序规则里属于「可忽略字符」。于是：

```powershell
"a$([char]0x202A)b" -eq "ab"     # True  ！！长度 3 和 2，却判等
"a$([char]0x202A)b".Length       # 3
```

意思是：**用 `-eq` 去检查「PATH 清干净了没有」，永远会说「干净了」。**
要比就必须用序数比较：

```powershell
[string]::Equals($a, $b, [System.StringComparison]::Ordinal)
```

写 `fix-system-path.ps1` 时就先踩了这个坑：清理逻辑明明生效了
（清理后确实没有 U+202A），但脚本仍然报「没发现问题」——
因为它在用 `-eq` 判断「变没变」。

### A.2 处理

- **临时**（不需要管理员）：`profile.ps1` 顶部会先把**当前进程** PATH 里的
  不可见字符清掉，所以新窗口一开就干净，conda 也就正常了。
- **根治**（需要管理员，一次就够）：管理员 PowerShell 里执行

  ```powershell
  & 'C:\Users\文涵超\Desktop\实习日志\tools\fix-system-path.ps1' -Apply
  ```

  不加参数先跑一次是**只读预览**，会告诉你它发现了什么、要改成什么。
  改完把 `profile.ps1` 顶部那段临时补丁删掉即可。

