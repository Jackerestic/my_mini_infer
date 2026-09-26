# M0 实验：一个 .cpp 文件怎么变成 .exe

> 目标：不要背四步。亲手生成中间文件，看到每一步的产物不一样。
> 用 examples/m0_hello.cpp 做实验。

## 准备

打开项目根目录：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
```

确认 g++ 能用：

```powershell
g++ --version
```

## 第 1 步：预处理

命令：

```powershell
g++ -E examples\m0_hello.cpp -o build\m0_hello.i
```

产物：`build\m0_hello.i`

这一步做什么：
- 把 `#include <iostream>` 这种语句展开；
- 把宏展开；
- 去掉注释；
- 输出仍然主要是文本。

观察：

```powershell
Get-Content build\m0_hello.i | Select-Object -First 30
```

你会看到很多原来 `m0_hello.cpp` 里没有的内容，那些是从头文件里展开出来的。

## 第 2 步：编译

命令：

```powershell
g++ -S examples\m0_hello.cpp -o build\m0_hello.s
```

产物：`build\m0_hello.s`

这一步做什么：
- 把 C++ 代码翻译成汇编语言；
- 汇编语言是更接近 CPU 的指令，但人还能看懂一点。

观察：

```powershell
Get-Content build\m0_hello.s | Select-Object -First 40
```

## 第 3 步：汇编

命令：

```powershell
g++ -c examples\m0_hello.cpp -o build\m0_hello.o
```

产物：`build\m0_hello.o`

这一步做什么：
- 把汇编继续翻译成机器码；
- 但这个 `.o` 文件还不能直接运行；
- 它不知道自己用到的库函数最后在哪里，需要链接器来拼起来。

## 第 4 步：链接

命令：

```powershell
g++ build\m0_hello.o -o build\m0_hello.exe
```

产物：`build\m0_hello.exe`

这一步做什么：
- 把 `.o` 文件和运行库拼在一起；
- 确定所有函数和数据的地址；
- 生成可以运行的程序。

运行：

```powershell
.\build\m0_hello.exe
```

应该看到：

```text
hello from m0_hello.cpp
```

## 一步完成

平时我们写的：

```powershell
g++ examples\m0_hello.cpp -o build\m0_hello.exe
```

其实是把上面四步一次性做完了。

## 写完 self_check

做完实验后，回到 docs/self_check.md 第三部分第 1 题，用你自己的话回答：

- 预处理做了什么？
- 编译做了什么？
- 汇编做了什么？
- 链接做了什么？

不要抄这份文件，写你能讲出来的版本。