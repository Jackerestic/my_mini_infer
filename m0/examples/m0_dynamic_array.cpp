#include "../include/dynamic_array.hpp"

#include <iostream>
#include <utility>

int main() {
    DynamicArray a;

    a.push_back(1);
    a.push_back(2);

    DynamicArray b = std::move(a);
    std::cout << "a.size=" << a.size() << '\n';
    std::cout << "b.size=" << b.size() << '\n';

    DynamicArray c;
    c.push_back(9);

    c = std::move(b);
    std::cout << "b.size=" << b.size() << '\n';
    std::cout << "c.size=" << c.size() << '\n';

    return 0;
}