#include <iostream>

// TODO 1: 写一个值传递函数：
// void change_by_value(int x) { x = 100; }
void change_by_value(int x){
    x=100;
}
// TODO 2: 写一个引用传递函数：
// void change_by_reference(int& x) { x = 100; }
void change_by_reference(int& x){
    x=100;
}
// TODO 3: 写一个 const 引用打印函数：
// void print_by_const_ref(const int& x) { std::cout << x << std::endl; }
void print_by_const_ref(const int& x){
    std::cout<<x<<std::endl;
}

int main() {
    int a = 10;

    // TODO 4: 调用 change_by_value(a)，然后打印 a。
    // 观察 a 有没有变成 100。
    change_by_value(a);
    std::cout<<a<<std::endl;

    // TODO 5: 调用 change_by_reference(a)，然后打印 a。
    // 观察 a 有没有变成 100。
    change_by_reference(a);
    std::cout<<a<<std::endl;
    
    // TODO 6: 调用 print_by_const_ref(a)。
    print_by_const_ref(a);

    return 0;
}