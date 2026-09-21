# my_mini_infer 项目日志

## 使用方式

每天只写真正发生的事：

- 日期：
- 今天写了什么代码：
- 是否编译通过：
- 是否提交 Git：
- 今天理解了什么概念：
- 卡在哪里：
- 下一步：

---

## 2026-09-18

- 项目骨架已创建。
- 当前 Tensor 支持构造、shape、at、fill、print。
- 下一步：实现 Tensor::add 和 Tensor::matmul。

## 2026-09-20

- 日期：2026-09-20
- 今天写了什么代码：没写 C++，补的是工具链。
  - 确认 `tools\cmake`（4.4.3）和 `tools\ninja`（1.13.2）之前已经下载好，
    但没接到 PATH 上，所以 `cmake` 命令根本用不了。
  - 新增 `tools\env.ps1` / `tools\env.cmd`：只给当前会话加 PATH，
    不动系统环境变量，不需要管理员权限。
  - 重写 `build.bat`：自动去 `..\tools\` 找 cmake 和 ninja，
    支持 `gpp`（默认）/ `cmake` / `test` / `clean` 四种模式。
  - 更新 README 的「工具链」和「怎么编译」两节，并记下编码坑。
- 是否编译通过：是。`build.bat gpp` 编译通过，`build.bat test` 输出
  `test_tensor passed`。
- 是否提交 Git：否（这个目录还不是 git 仓库，以后要 `git init`）。
- 今天理解了什么概念：
  - 工具「下载了」不等于「能用了」，中间差的是 PATH。
    而 PATH 有两种加法：改系统（`setx`，永久、影响所有程序、要重开终端）
    和改当前会话（`$env:PATH`，临时）。项目自带的工具适合后者。
  - 踩了两个和**文件编码**有关的坑：
    1. 这台机器的 `powershell` 其实是 Windows PowerShell 5.1。
       它读 `.ps1` 时，如果文件是「UTF-8 无 BOM」，会按 GBK 解析，
       中文注释直接乱码，而且会把结尾的引号吃掉，报
       `The string is missing the terminator`。解决办法：`.ps1` 存成
       **UTF-8 with BOM**。
    2. 反过来 `.bat` / `.cmd` **不能**有 BOM（cmd 会把 BOM 当成命令的一部分，
       报 `'ï»¿@echo' is not recognized`），而且 cmd.exe 按 GBK 解析文件，
       所以里面只写 ASCII 英文，中文说明放到 README。
  - 系统默认执行策略是 Restricted，`.ps1` 会直接被拒绝运行，
    报 `running scripts is disabled on this system`。
    用 `Set-ExecutionPolicy -Scope Process Bypass -Force` 只放宽当前窗口。
- 卡在哪里：CMake configure 在 MinGW 8.1.0 上**非常慢**。
  一度以为卡死了，但看 `CMakeFiles\CMakeConfigureLog.yaml` 和
  `CMakeScratch\` 目录名字在变，说明是在往前走，只是每步 try_compile
  都要编译+运行一个小程序（大概被杀毒软件拖慢了）。
  教训：判断「卡住」还是「在跑」，要看日志和进程，不要只看屏幕。
- 下一步：实现 Tensor::add 和 Tensor::matmul，并补测试。

## 2026-09-20（下午：Tensor::add / matmul 完成）

- 日期：2026-09-20
- 今天写了什么代码：
  - `Tensor::add`：逐元素相加，先检查形状相同。
  - `Tensor::matmul`：标准 i-j-k 三重循环，先检查 `cols() == other.rows()`。
  - 这两个函数一开始写进了 `tensor.hpp`，后来按「hpp 声明 / cpp 定义」
    的分工搬到了 `src\tensor.cpp` 第 55-80 行。
  - `tests/test_tensor.cpp`：补上 add / matmul 的测试，含非方阵用例。
- 是否编译通过：**这次没能验证**。测试写完之后发现系统里的 MinGW 编译器
  被删掉了（`C:\main\x86_64-8.1.0-release-posix-seh-rt_v6-rev0\mingw64`
  只剩一个空的 `opt` 目录，`g++.exe` 全盘找不到），
  而且网络也不通（TLS 握手失败），暂时装不回来。
  在这次删除之前，`build.bat gpp` 和 `build.bat test` 都是通过的。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **头文件（.hpp）和源文件（.cpp）的分工**：hpp 放声明，cpp 放定义。
    类内定义成员函数虽然合法（会自动变成 inline），
    但会让所有包含这个头文件的文件在实现变化时全部重编。
  - **头文件必须自给自足：谁使用，谁包含。**
    我在 hpp 里用了 `assert` 却没有 `#include <cassert>`；
    而 `tensor.cpp` 里的 `#include <cassert>` 写在
    `#include "mini_infer/tensor.hpp"` 的后面，
    所以解析头文件时 `assert` 还没声明 → 编译失败。
    编译器给的提示就是 `did you forget to '#include <cassert>'?`。
  - **`assert` 在 Release 下会被 `NDEBUG` 关掉**，两个后果：
    1. 形状不匹配时不会报错，而是变成越界读写（静默内存踩踏）；
    2. 用 `assert` 写的测试在 Release 下会整个静默失效——
       编译能过、跑起来也"通过"，其实一条都没检查。
    所以测试改成用 `CHECK` 宏自己计数，最后用返回值告诉 CTest。
  - **删掉 `Tensor() = default`**：原来默认构造出来的 Tensor
    `shape_` 是空的，而 `rows()` 用的是 `shape_.at(0)`，
    会在**运行时**抛 `std::out_of_range`。
    删掉之后 `Tensor t;` 直接**编译失败**，错误从运行时提前到了编译期。
  - **测试必须有非方阵用例**：2x2 × 2x2 这种方阵用例，
    即使把 result 的行列写反也照样能过，只有非方阵能把 bug 逼出来。
  - **浮点不能直接用 `==` 比**，要用相对误差；但整数结果（19、32）可以。
  - **`if errorlevel 1` 抓不住负的退出码**（这是我给 `build.bat` 写的错误检查，
    踩了大坑）：`if errorlevel N` 判断的是「退出码 >= N」，
    所以退出码是负数时条件为假。比如 -1073741515 = 0xC0000135
    （DLL 找不到，程序根本没启动），脚本会把**崩溃**当成**成功**，
    打印 "Test OK"。正确写法是 `if %ERRORLEVEL% neq 0`。
  - **小心"陈旧二进制"**：源码改了但没重新编译时，
    跑 `build\test_tensor.exe` 拿到的是旧程序的结果，会自己骗自己。
    现在 `build.bat test` 会比对时间戳，二进制比源码旧就警告。
  - 判断一次测试是不是真的跑了，要看三样：**退出码、时间戳、输出内容**。
    只看屏幕上有没有 "OK" 是不够的。
- 卡在哪里：
  - 找 `tensor.cpp` 找了半天，原来它一直在 `src\tensor.cpp`。
    教训：先看 `README.md` 的目录结构和 `CMakeLists.txt`，不要瞎找。
  - 系统里的 MinGW 编译器在 15:04 左右被删除，原因不明；
    网络同时也不通，所以无法重新下载。编译验证只能等环境恢复。
- 下一步：
  - 先恢复编译器（重装 MinGW-w64，或把便携工具链放进 `tools\`）。
  - 然后按 `docs/nn_primer.md` 第 14 节的顺序写：
    `relu` → `transpose` → `softmax` → `linear`。

## 2026-09-20（傍晚：编译器消失 → 重建工具链 → 测试真正通过）

- 日期：2026-09-20
- 环境事故：**系统里的 MinGW 编译器被删了。**
  - `C:\main\x86_64-8.1.0-release-posix-seh-rt_v6-rev0\mingw64` 只剩一个空的 `opt`；
    全盘搜 `g++.exe` 一个都没有；回收站 0 MB（是永久删除，不是删到回收站）。
  - PATH 里另一条 `C:\Users\...\Downloads\...\mingw64\bin` 也早就不存在。
  - **查不出是谁删的**：Defender 没有拦截记录（只有健康报告）、
    计划任务里没有相关项、Prefetch 被禁用（0 个文件）、
    没有任何一个 DSH 会话的命令碰过 `C:\main`、系统只有一个 C: 盘（不是被挪走）。
    结论：是项目之外的东西删的（手动操作或某个清理工具），无法指认具体是谁。
- 顺手查清楚的两件事：
  1. **同时有另一个 DSH 会话在跑同一个任务**
     （`session-8a768f8c`，开场白「辅助我完成my_mini_infer项目，首先帮我安装一个cmake」）。
     它在 15:04–15:07 之间**反复删了 7 次 `.tmp_tools`**，
     还删掉了 `tools\ninja` 和 `tools\ninja-official`、
     把 `ninja.exe` 合并进 `tools\cmake\bin`，并修改了注册表 `HKCU:\Environment` 的 Path。
     **我写的临时文件（`sign_demo.cpp`、`verify_ops.cpp`）就是这样没的。**
     教训：同一个工作区不要同时开两个 agent 会话，它们会互相踩。
  2. **之前的网络失败是 DSH 沙箱造成的**（`schannel: SEC_E_NO_CREDENTIALS`），
     不是系统故障。权限放开之后 `curl` 立刻返回 200。
- 今天做了什么：
  - GitHub 直连下载被重置，改用 `ghproxy.net` 镜像，下了 MinGW-w64 16.2.0，
    解压到 `tools\mingw64`，和 cmake/ninja 放在一起 ——
    工具链从此在**工作区内自给自足**，外面再清理也不影响。
  - `build.bat`：自动识别 `..\tools\mingw64\bin`；
    g++ 找不到时给出明确提示；`test` 模式会警告"二进制比源码旧"。
  - `build.bat` 的 g++ 加 `-static`：不加的话 exe 依赖 MinGW 运行时 DLL，
    双击 `build\mini_infer_app.exe` 会报 `0xC0000135`（DLL not found）。
  - `tools\env.ps1` / `env.cmd` 同步加入 mingw64。
- 是否编译通过：**是，而且这次是真的通过了。**
  `build.bat gpp` 成功；`build.bat test` 输出 11 个用例全 ok，exit=0。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **判断"测试通过"要看三样：退出码、二进制时间戳、输出内容。**
    我第一次跑 `build.bat test` 拿到 "Test OK"，但那跑的是 15:04 的旧二进制；
    而且那个旧 exe 因为缺 DLL 根本起不来（退出码 -1073741515 是负数），
    我 `build.bat` 里写的 `if errorlevel 1` **抓不住负数退出码**，于是谎报成功。
    正确写法是 `if %ERRORLEVEL% neq 0`。
    负退出码的含义：`-1073741515` = `0xC0000135` = STATUS_DLL_NOT_FOUND。
  - **动态链接的 exe 离不开工具链**，`-static` 把 libstdc++ / libgcc
    打进 exe，才能真正做到双击就跑。
  - **7z 不用装 7-Zip 也能解**：Windows 自带的 `tar.exe` 是 bsdtar（libarchive），
    直接 `tar -xf x.7z -C 目标目录` 就行。
  - 大文件下载要能断点续传：`curl -C -`，因为传到一半经常断。
- 卡在哪里：
  - GitHub 的 release 资源下载会被重置（`curl: (56)`），但 `api.github.com` 能用。
    解决办法是用镜像代理（`ghproxy.net`）。
  - 差点被"陈旧二进制 + 错误检查漏掉负退出码"这两个坑联合骗过去，
    以为测试早就通过了。
- 下一步：按 `docs/nn_primer.md` 第 14 节写 `relu` → `transpose` → `softmax` → `linear`。

## 2026-09-20（晚上：relu 完成，也想明白了「为什么要写这些」）

- 日期：2026-09-20
- 今天写了什么代码：
  - `Tensor::relu()`：逐元素 `max(0, x)`，形状不变，返回新对象、不改自己。
  - 声明加在 `tensor.hpp`，定义加在 `src\tensor.cpp`，测试加在 `tests\test_tensor.cpp`。
  - 测试三个用例（混合正负 / 全负 / 全正），外加「形状不变」和「原对象不被改动」。
- 是否编译通过：**是。** 干净重编 **0 警告 0 错误**（`-Wall -Wextra`），
  `build.bat test` 输出 3 条 relu ok，exit=0，二进制时间戳是新的。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **在类外面定义成员函数，必须写 `类名::`。**
    我第一版写成 `Tensor ReLU() const{`，漏了 `Tensor::`。
    编译器的第一个报错是
    `non-member function ... cannot have cv-qualifier`——
    这行就是线索：`const` 只有成员函数能有，
    说明编译器根本没把它当成 `Tensor` 的成员，而是当成了命名空间里的自由函数；
    于是 `rows()` / `cols()` / `at()` 全都「not declared in this scope」。
  - **`0.of` 是笔误**（字母 `o` 不是数字 `0`），应该是 `0.0f`。
    报错是 `unable to find numeric literal operator 'operator""of'`——
    因为 C++11 起支持自定义字面量后缀（`1s`、`10ms`），
    所以编译器把 `0.of` 读成「`0.` 后面跟一个叫 `of` 的后缀」。
  - **函数写了不等于会跑。** `test_relu()` 写好了，但忘了在 `main()` 里调用，
    结果测试照样"通过"、照样打印 "Test OK"，而 relu 的检查**一条都没执行**。
    这是"假通过"的另一种形态。防御办法：让每个测试函数第一行打印自己的名字
    （`--- relu ---`），看不到这一行就说明它根本没跑。
  - **中文乱码的机制**：源文件存成 UTF-8 → `std::cout` 把字节**原样**写出
    （不做任何转码）→ 控制台按自己的**活动代码页**解释这些字节。
    终端是 936（GBK）时，UTF-8 字节就被显示成 `--- 鏋勯€?/ at / fill ---`。
    原版 `build.bat` 开头本来有 `chcp 65001`，我改成纯 ASCII 时把它删了，
    这是个回归，已经加回来并实测过（936 → 65001）。
- 卡在哪里：relu 的逻辑本身很简单，卡的全是三个「低级但致命」的小问题：
  漏 `Tensor::`、`0.of` 笔误、忘了调用测试函数。
  另外修 `chcp` 的时候我自己又踩了两个坑：
  往 `build.bat` 注释里粘了中文（违反「`.bat` 必须纯 ASCII」，多出 15 个非 ASCII 字节）；
  以及 PowerShell 的 here-string 不留末尾换行，
  导致 `chcp 65001 >nul` 和 `setlocal` 粘成了一行。
  教训：**改完文本文件一定要回头验证编码和实际内容**，不能只看"写入成功"。
- 想明白的一件事（已写进 `docs/nn_primer.md` 第 15-21 节）：
  add / matmul / relu 不是「练习用的矩阵运算」，它们**就是神经网络本身**。
  - `matmul` = 一大堆神经元同时做加权求和（网络几乎所有算力都在这）；
  - `add` = 加偏置 `b`；
  - `relu` = 非线性。
  而且如果只有 matmul 和 add，堆多少层都等价于一层：
  `W2*(W1*x + b1) + b2 = (W2*W1)*x + (W2*b1 + b2)`，
  乘开还是一个矩阵乘法。**所以 ReLU 是不可省的**，它打破了这种"可合并"性。
- 下一步：`Tensor::transpose()` —— 线性层要用它。
  因为 PyTorch 的权重按 `(out_features, in_features)` 存，
  所以实际计算是 `y = x @ W.T + b`，不转置就没法直接用导出的权重。

## 2026-09-20（深夜：transpose 完成，并验证了测试真的有牙齿）

- 日期：2026-09-20
- 今天写了什么代码：
  - `Tensor::transpose()`：最终写成 `result.at(c, r) = at(r, c)`。
    循环遍历**原矩阵**的 `r`/`c`，把结果写到**交换后**的位置，
    这样边界天然是对的（`c` 落在 `result` 的行范围、`r` 落在列范围）。
  - `test_transpose()`：非方阵 2x3→3x2、边界 1x3 / 3x1、
    性质 `(A.T).T == A`、以及「原对象不被改动」。
- 是否编译通过：**是。** 干净重编 **0 警告 0 错误**（`-Wall -Wextra`），
  `build.bat test` 全过（4 条 transpose ok），exit=0。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **bug 的形态决定测试的形态。**
    第一版写的是 `result.at(r,c) = at(c,r)`——
    这个**赋值关系本身是对的**（`B[i][j] = A[j][i]`），
    错的是**循环边界**：`result` 已经是 `(cols, rows)`，
    可循环还在拿原来的 `rows()` / `cols()` 走，
    于是非方阵时 `result.at` 直接越界。
    **方阵时两个维度一样，所以方阵用例完全看不出问题**——
    这正是「必须用非方阵」这条经验的真实案例。
  - **测试要有「牙齿」：能被证明会失败。**
    验证方法：把旧的错误实现放回去，看测试会不会挂。
    实测（把错误写法还回去之后）：
    `Assertion failed: row < rows() && col < cols()`，程序直接 abort。
    说明这个测试不是摆设。
  - 测试抓到错误有**两条路径**：
    1. **边界 assert** —— 越界，程序直接 abort；
    2. **值不匹配** —— 打印 FAIL 然后继续跑。
    这次走的是第 1 条。因为 `at()` 里有边界检查，
    它比「结果算错」更早、更明确地报警。
  - **退出码能区分「崩了」和「没起来」**：
    `-1073740791`（`0xC0000409`）是 `abort()`；
    `-1073741515`（`0xC0000135`）是找不到 DLL。
  - 顺手踩的一个小坑：验证用的临时程序忘了加 `-static`，
    exe 起不来（`0xC0000135`）。这就是 `build.bat` 里为什么要加 `-static`。
- 下一步：`Tensor::softmax()` —— 记得减最大值（`docs/nn_primer.md` 第 11 节），
  不减的话输入大一点就会得到 `nan`。

## 2026-09-20（深夜二：softmax 完成，学会了「变异测试」）

- 日期：2026-09-20
- 今天写了什么代码：
  - `Tensor::softmax()`：对**每一行**做（行内加起来 = 1）。
    三步：找行内最大值 → `exp(x - max)` 并求和 → 除以和。
  - `test_softmax()`：4 个手算用例（含溢出用例 `[[1000,1001]]`）
    + 一条"每行加起来 = 1"的性质测试。
- 是否编译通过：**是。** 干净重编 **0 警告 0 错误**，`build.bat test` 全过，exit=0。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **`exp`（指数函数）**：`exp(x)` 就是 `e^x`，`e ≈ 2.71828`。
    三个关键性质：永远为正、单调递增、`exp(a+b) = exp(a)·exp(b)`。
    最后一条正是「减最大值」技巧的依据——
    `softmax(x - m) = softmax(x)`，推导是分子分母同乘 `exp(-m)` 再约掉。
  - **float 在 `exp(89)` 就溢出**（float 上限约 3.4e38）。
    所以只要分数里有超过 89 的数，不减最大值就会 `inf/inf = nan`。
    这不是极端情况，分类任务的 logits 到几百很常见。
  - **测试有两层「假通过」**，我这两次各踩了一个：
    1. **测试根本没跑** —— `test_softmax()` 写好了但忘了在 `main()` 里调用。
       这次是**编译器警告**抓到的：
       `'test_softmax()' defined but not used [-Wunused-function]`。
       教训：**要盯警告数**。前几次都是 0 警告，这次是 1，这就是信号。
    2. **检查写得太松** —— 原本写的是
       `CHECK(t.at(0,0)+t.at(0,1)-1.0 < 1e-5)`，少了绝对值。
       这只能抓到"和太大"，抓不到"和太小"：
       和是 0.5 时 `-0.5 < 1e-5` 依然成立，照样通过。
       正确写法是 `std::fabs(...) < 1e-5f`。
  - **变异测试（mutation test）**：证明测试有牙齿的方法——
    故意把实现改坏一处，确认测试会失败。
    这次把 `std::exp(at(r,c)-max)` 改成 `std::exp(at(r,c))`（去掉减最大值），
    结果正是预期的：
    ```
    ok  normal test
    ok  两个0
    ok  三个数
    FAIL  减去最大值测试 (0,0): got nan, want 0.268941
    test_tensor FAILED: 2 check(s)
    ```
    **前三条依然通过，只有溢出那条抓到了**——这说明那个用例不是重复的，
    它是唯一能守住"减最大值"这一步的测试。
    上次 transpose 的变异测试（把 `result.at(c,r)` 改回 `result.at(r,c)`）
    也是同样的用法。
  - 一个实测细节：变异后 `test_tensor` 返回 **exit=1**，
    说明测试框架的返回值能正确传递给 `build.bat` / CTest。
- 卡在哪里：softmax 的数学不难，难的是**把测试写「紧」**。
  这次两次翻车（忘调用、检查太松）都不是算法问题，而是测试自己的质量问题。
- 下一步：`linear(x, W, b)` —— `y = x @ W.T + b`，
  需要广播加偏置（`docs/nn_primer.md` 第 9、10 节）。
  写完这一步就能把 Linear → ReLU → Linear → Softmax 串成完整的 MLP 前向了。

## 2026-09-20（深夜三：linear 完成，阶段 1 的算子全齐了）

- 日期：2026-09-20
- 今天写了什么代码：
  - `linear(x, W, b)`：写成**自由函数**（不是成员函数），因为它是三个平级
    Tensor 之间的操作。`y = x.matmul(W.transpose())`，再用显式循环
    `result.at(r,c) += b.at(0,c)` 广播加偏置。
  - `test_linear()`：一组不对称的权重，能抓出"`W.T` 写成 `W`"这类错误。
- 是否编译通过：**是。** 干净重编 **0 警告 0 错误**，`build.bat test` 全过，exit=0。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **广播（broadcasting）**：`b` 是 `(1, out)`，`y` 是 `(N, out)`，
    形状不同不能直接加。广播的规则是「某一维长度为 1 就拉伸到对方的长度」。
    - 为什么 `b` 不存成 `(N, out)`：`b` 是**这一层自己的参数**，和样本数无关。
      一批 1000 条样本时，存 `(1000, out)` 就是把同样的数重复存 1000 遍。
      PyTorch 里 `nn.Linear(3,2).bias.shape` 就是 `(2,)`。
    - 代码里怎么广播：`b.at(0, c)` —— **行下标写死成 0**，
      物理上不复制数据，只是重复读同一个格子。
    - 我们的 Tensor 只有二维，所以一维的偏置只能用 `(1, out)` 表示。
  - **自由函数 vs 成员函数**：`linear` 不需要 `Tensor::` 前缀，
    因为它不是成员函数。判断标准：函数的主语是"某个对象自己"就是成员函数；
    `linear` 的主语是三个平级输入，所以是自由函数。
    （上次栽在"漏写 `Tensor::`"，这次正好是反过来——**不该加就别加**。）
  - **测试也会抓到「人的手算错误」，不只是代码 bug。**
    这次实现是对的，是我手算期望值算错了：`W.T` 最后一行 `[4, 1]` 里的 `1`
    被我当成了 `0`，导致两个格子各少 3 和 6。
    测试直接报出了精确差异：`(0,1): got 14.5, want 11.5`。
    矩阵乘法手算特别容易在"第几行第几列"上出错，自己检查三遍也未必看得出来。
  - **用例覆盖要够广**：那次四个期望值里**两个对、两个错**。
    如果我只写自己算得准的那两个，这个错误就永远发现不了。
- 一个小遗留：`test_softmax` 和 `test_linear` 的用例名都叫 `"normal test"`，
  输出里两条 `ok  normal test` 分不出谁是谁。
  如果其中一条失败，只看到 `FAIL normal test` 也不知道是哪个函数的问题。
  **用例名应该唯一且能说明测的是什么。**
- 阶段 1 的四个算子（`relu` / `transpose` / `softmax` / `linear`）**全部完成**。
- 下一步：`test_mlp()` —— 把四个算子串成完整前向
  `Linear → ReLU → Linear → Softmax`，并且**逐层对照手算值**
  （不是只看最终输出，出错了才知道是哪一层的问题）。

## 2026-09-20（收尾：MLP 前向完成，阶段 1 结束）

- 日期：2026-09-20
- 今天写了什么代码：
  - `test_mlp()`：2 输入 → 2 隐藏 → 2 输出的完整前向，**逐层**对照手算值。
  - 顺手把重复的用例名改掉了（`softmax{2,1}`、`linear normal test`）。
- 是否编译通过：**是。** 干净重编 **0 警告 0 错误**，全部测试通过，exit=0。
- 是否提交 Git：否。
- 今天理解了什么概念：
  - **逐层验证（step-by-step verification）**：MLP 前向有 5 步
    （linear → relu → linear → relu → softmax）。
    如果只测最终输出，一旦失败有 5 个地方要排查；
    逐层写的话，**第一个 FAIL 出现的位置就是出错的那一层**。
  - **这组权重确实在检验 ReLU**：第二层 linear 算出 `[[-4, 4]]`，里面有负数。
    如果漏掉 ReLU，最终 softmax 会变成 `[[0.0003354, 0.9996646]]`，
    而正确答案是 `[[0.0179862, 0.9820138]]`——差别巨大。
    所以这个用例能真正区分"有 ReLU"和"没 ReLU"。
  - **测试用例名必须唯一。** 之前 `test_softmax` 和 `test_linear` 的用例
    都叫 `"normal test"`，输出里出现两条一样的 `ok  normal test`，
    一旦失败根本分不清是哪个函数的问题。
- 环境情况：
  - Python 3.14.6 + **numpy 2.5.2 已装**，可以直接用。
  - **PyTorch 没装**，`download.pytorch.org` 不通。
  - 文件策略切回 workspace-write 之后，curl 访问 HTTPS 又开始报
    `schannel: SEC_E_NO_CREDENTIALS`（退出码 35）。这**反过来证实**了
    之前的判断：那个"网络故障"就是 DSH 沙箱造成的，权限放开时能通、收紧后又不行。
- **阶段 1（线性层与 MLP 前向）正式完成。**
  四个算子 `relu` / `transpose` / `softmax` / `linear` 全部实现并测试通过。
- 下一步（待定）：见 README 的「当前任务」一节。


## 2026-09-21
- linear 为什么是 x @ W.T + b？
因为一个神经元所做的就是x*(w.t)+b,linear只是包装起来以便测试和使用
为什么 W 的 shape 是 (out, in)？
因为pytorch中是这样表示的
为什么 softmax 要减掉每行最大值？
因为算概率时我们使用的是exp，如果不减去最大值那么会导致数值巨大甚至超出范围(float最大89)
为什么测试非方阵矩阵乘法很重要？
方阵数据两个维度是一样的，测试可能看不出问题，而非方阵测试则有“牙齿”。
