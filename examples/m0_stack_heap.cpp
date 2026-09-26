
// TODO 1: 在栈上创建一个 int，初值 10。
// 提示：int a = 10;

// TODO 2: 在堆上创建一个 int，初值 20。
// 提示：int* p = new int(20);

// TODO 3: 打印 a 和 *p 的值。

// TODO 4: 释放堆内存。
// 提示：delete p;

#include <iostream>

int main() {
    // 在下面写 TODO 1 到 TODO 4
    int a=10;
    int* p=new int(20);
    std::cout<<a<<" "<<*p<<std::endl;
    delete p;
    return 0;
}