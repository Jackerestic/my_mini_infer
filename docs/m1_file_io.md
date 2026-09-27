# M1 前置：C++ 怎么读文件

> M1 的第一步是把 data/mlp.txt 读进 C++。
> 先不要解析矩阵，只做一件事：打开文件，一行一行打印出来。

## 1. 需要的新头文件

```cpp
#include <fstream>
```

这个头文件里有两个常用类型：

- std::ifstream：读文件；
- std::ofstream：写文件。

## 2. 打开文件

```cpp
std::ifstream fin("data/mlp.txt");
```

这行代码的意思是：
- 创建一个读文件的对象 fin；
- 尝试打开 data/mlp.txt。

打开可能失败，所以要检查：

```cpp
if (!fin) {
    std::cout << "open failed\n";
    return 1;
}
```

## 3. 一行一行读

需要：

```cpp
#include <string>
```

然后：

```cpp
std::string line;
while (std::getline(fin, line)) {
    std::cout << line << '\n';
}
```

- std::getline：从文件读一行；
- 读到了就放进 line；
- 读不到就结束循环。

## 4. 文件什么时候关闭

fin 离开作用域时会自动关闭文件。
这也是 RAII 的一个例子：对象负责自己管理的文件资源。

## 5. 实验

打开 examples/m1_file_io.cpp，按 TODO 1 到 TODO 3 补完。

在项目根目录编译运行：

```powershell
g++ -std=c++17 -Wall -Wextra examples\m1_file_io.cpp -o build\m1_file_io.exe
.\build\m1_file_io.exe
```

期望看到 data/mlp.txt 的内容，比如：

```text
x 3 4
0.304717094 -1.03998411 0.750451207 0.940564692
...
```

## 6. 注意运行目录

上面的命令必须在项目根目录运行，因为程序里写的是相对路径：

```text
data/mlp.txt
```

如果从 build 目录运行，路径应写成：

```text
..\data\mlp.txt
```

或者把文件路径作为 main 的参数传进去。M1 后面我们会用更稳的方式。

## 7. 完成后

发我代码和运行输出。下一步才是把每一行解析成名字、行数、列数和数字。