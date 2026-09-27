#include "mini_infer/tensor.hpp"

#include <iostream>

int main() {
    mini_infer::Tensor t(2, 3, 0.0f);

    t.at(0, 0) = 1.0f;
    t.at(0, 1) = 2.0f;
    t.at(0, 2) = 3.0f;
    t.at(1, 0) = 4.0f;
    t.at(1, 1) = 5.0f;
    t.at(1, 2) = 6.0f;

    std::cout << "Mini Inference Engine skeleton\n";
    t.print();

    std::cout << "\nNext tasks:\n";
    std::cout << "1. Implement Tensor::add\n";
    std::cout << "2. Implement Tensor::matmul\n";
    std::cout << "3. Add unit tests for each operation\n";
    return 0;
}
