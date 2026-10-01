#include "mini_infer/tensor.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

using mini_infer::Tensor;

int main() {
    std::ifstream fin("data/mlp.txt");
    if (!fin) {
        std::cout << "open failed: data/mlp.txt\n";
        return 1;
    }

    std::map<std::string, Tensor> tensors;
    std::string line;

    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream header(line);
        std::string name;
        std::size_t rows = 0;
        std::size_t cols = 0;

        if (!(header >> name >> rows >> cols)) {
            std::cout << "bad header: " << line << '\n';
            return 1;
        }

        Tensor t(rows, cols);

        for (std::size_t r = 0; r < rows; ++r) {
            if (!std::getline(fin, line)) {
                std::cout << "unexpected end of file\n";
                return 1;
            }

            std::istringstream row_stream(line);
            for (std::size_t c = 0; c < cols; ++c) {
                float value = 0.0f;

                // TODO 1: 从 row_stream 读一个 float 到 value。
                // 提示：row_stream >> value;
                row_stream>>value;
                t.at(r, c) = value;
            }
        }

        tensors.emplace(name, std::move(t));
    }

    const char* names[] = {"x", "W1", "b1", "W2", "b2", "h", "o", "y"};
    for (const char* name : names) {
        const Tensor& t = tensors.at(std::string(name));
        std::cout << name << " " << t.rows() << " " << t.cols() << '\n';
    }

    return 0;
}