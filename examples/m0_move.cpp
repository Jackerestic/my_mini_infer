#include <iostream>
#include <vector>
#include <utility>

int main() {
    // TODO 1: 创建 vector a，5 个元素，每个都是 7。
    // 提示：std::vector<int> a(5, 7);
    std::vector<int> a(5,7);
    // TODO 2: 打印 a.size() 和第一个元素的地址。
    // 提示：(void*)a.data()
    std::cout<<"size:"<<a.size()<<std::endl;
    std::cout<<"address of first element:"<<(void*)a.data()<<std::endl;
    // TODO 3: 复制 a 到 b，再打印 a.data() 和 b.data()。
    // 提示：std::vector<int> b = a;
    std::vector<int> b=a;
    std::cout<<"a.data():"<<(void*)a.data()<<std::endl;
    std::cout<<"b.data():"<<(void*)b.data()<<std::endl;
    // TODO 4: 用 std::move 把 a 移动到 c。
    // 提示：std::vector<int> c = std::move(a);
    // 打印 a.size()、c.size()、c.data()。
    std::vector<int> c=std::move(a);
    std::cout<<"a.size():"<<a.size()<<"\nc.size():"<<c.size()<<"\nc.data():"<<(void*)c.data()<<std::endl;
    // TODO 5: 用一句话写下：
    // 为什么 b.data() 和 a.data() 不同，而 c.data() 可能和原来 a.data() 相同？
    //本质在于b和c的初始化方式不同。b为拷贝构造，只是将a的值复制给b，所以a，b相当于两个独立的个体只是有着相同的值
    //而c由a移动构造而成，相当于将a的内容打包转移给c了
    return 0;
}