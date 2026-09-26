#include <iostream>

int main() {
    int a = 10;
    int b = 99;

    // TODO 1: 创建指针 p，指向 a。
    // 提示：int* p = &a;
    int* p=&a;
    // TODO 2: 创建引用 r，绑定到 a。
    // 提示：int& r = a;
    int& r=a;
    // TODO 3: 打印 a、*p、r。
    std::cout<<a<<" "<<*p<<" "<<r<<std::endl;
    // TODO 4: 通过指针把值改成 20，再打印 a。
    *p=20;
    std::cout<<a<<std::endl;
    // TODO 5: 通过引用把值改成 30，再打印 a。
    r=30;
    std::cout<<a<<std::endl;
    // TODO 6: 让 p 指向 b，再打印 *p。
    p=&b;
    // TODO 7: 执行 r = b; 然后打印 a 和 b。
    r=b;
    std::cout<<a<<" "<<b<<std::endl;
    // 想一想：r 是改绑到了 b，还是把 b 的值赋给了 a？

    return 0;
}