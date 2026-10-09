#include "mini_infer/tensor.hpp"
#include "mini_infer/tensor_io.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>

using mini_infer::Tensor;
using mini_infer::linear;

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
    if (!mini_infer::load_tensors("data/mlp.txt", inputs)) {
        std::cout << "failed to load data/mlp.txt\n";
        return 1;
    }

    std::map<std::string, Tensor> refs;
    if (!mini_infer::load_tensors("data/mlp_numpy_ref.txt", refs)) {
        std::cout << "failed to load data/mlp_numpy_ref.txt\n";
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