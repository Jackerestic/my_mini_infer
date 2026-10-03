#include "mini_infer/tensor.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>

using mini_infer::Tensor;
using mini_infer::linear;

bool load_tensors(const std::string& path, std::map<std::string, Tensor>& out) {
    std::ifstream fin(path);
    if (!fin) {
        std::cout << "open failed: " << path << '\n';
        return false;
    }

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
            std::cout << "bad header in " << path << ": " << line << '\n';
            return false;
        }

        Tensor t(rows, cols);

        for (std::size_t r = 0; r < rows; ++r) {
            if (!std::getline(fin, line)) {
                std::cout << "unexpected end of file: " << path << '\n';
                return false;
            }

            std::istringstream row_stream(line);
            for (std::size_t c = 0; c < cols; ++c) {
                float value = 0.0f;
                if (!(row_stream >> value)) {
                    std::cout << "bad value in " << path << ": " << line << '\n';
                    return false;
                }
                t.at(r, c) = value;
            }
        }

        out.emplace(name, std::move(t));
    }

    return true;
}

float max_abs_error(const Tensor& got, const Tensor& want) {
    assert(got.rows() == want.rows() && got.cols() == want.cols());

    float max_error = 0.0f;
    for (std::size_t r = 0; r < got.rows(); ++r) {
        for (std::size_t c = 0; c < got.cols(); ++c) {
            const float error = std::fabs(got.at(r, c) - want.at(r, c));
            if (error > max_error) {
                max_error = error;
            }
        }
    }
    return max_error;
}

int main() {
    std::map<std::string, Tensor> inputs;
    if (!load_tensors("data/mlp.txt", inputs)) {
        return 1;
    }

    std::map<std::string, Tensor> refs;
    if (!load_tensors("data/mlp_numpy_ref.txt", refs)) {
        return 1;
    }

    const char* input_names[] = {"x", "W1", "b1", "W2", "b2"};
    for (const char* name : input_names) {
        const Tensor& t = inputs.at(std::string(name));
        std::cout << name << " " << t.rows() << " " << t.cols() << '\n';
    }

    const char* ref_names[] = {"h", "o", "y"};
    for (const char* name : ref_names) {
        const Tensor& t = refs.at(std::string(name));
        std::cout << name << " " << t.rows() << " " << t.cols() << '\n';
    }

    const Tensor& x = inputs.at("x");
    const Tensor& W1 = inputs.at("W1");
    const Tensor& b1 = inputs.at("b1");
    const Tensor& W2 = inputs.at("W2");
    const Tensor& b2 = inputs.at("b2");

    Tensor h_cpp = linear(x, W1, b1);
    h_cpp = h_cpp.relu();

    Tensor o_cpp = linear(h_cpp, W2, b2);
    o_cpp = o_cpp.relu();

    Tensor y_cpp = o_cpp.softmax();

    const float h_max = max_abs_error(h_cpp, refs.at("h"));
    const float o_max = max_abs_error(o_cpp, refs.at("o"));
    const float y_max = max_abs_error(y_cpp, refs.at("y"));

    std::cout << "h max error = " << h_max << '\n';
    std::cout << "o max error = " << o_max << '\n';
    std::cout << "y max error = " << y_max << '\n';

    if (h_max < 1e-5f && o_max < 1e-5f && y_max < 1e-5f) {
        std::cout << "M1 PASS\n";
        return 0;
    }

    std::cout << "M1 FAIL\n";
    return 1;
}