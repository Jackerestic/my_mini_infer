#include <iostream>
#include <vector>

int main() {
    std::vector<int> v;

    // TODO 1: 打印一开始的 size 和 capacity。
    std::cout<<"size:"<<v.size()<<" "<<"capacity:"<<v.capacity()<<std::endl;
    // TODO 2: 循环 i 从 1 到 10：
    //         把 i push_back 进 v；
    //         打印 size、capacity、第一个元素的地址。
    // 提示：v.data() 返回第一个元素的指针。
    // 打印地址可以用：(void*)v.data()
    for(int i=1;i<=10;i++){
        v.push_back(i);
        std::cout<<"size:"<<v.size()<<" "<<"capacity:"<<v.capacity()<<"\nfirst element's address:"<<(void*)v.data()<<std::endl;
    }
    // TODO 3: 调用 v.reserve(20)，再打印 capacity。
    v.reserve(20);
    std::cout<<"capacity:"<<v.capacity()<<std::endl;
    // TODO 4: 再 push_back 5 个元素，观察还会不会搬家。
    for(int j=11;j<=15;j++){
        v.push_back(j);
    }
    std::cout<<"first element's address:"<<(void*)v.data()<<std::endl;
    return 0;
}