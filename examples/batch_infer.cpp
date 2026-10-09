#include "mini_infer/mlp.hpp"
#include "mini_infer/tensor.hpp"
#include "mini_infer/tensor_io.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>

using mini_infer::Mlp;
using mini_infer::Tensor;

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

    Mlp model(inputs.at("W1"), inputs.at("b1"),
              inputs.at("W2"), inputs.at("b2"));

    Tensor y_cpp = model.forward(inputs.at("x"));

    std::cout << "y shape: " << y_cpp.rows() << " x " << y_cpp.cols() << '\n';
    for (std::size_t r = 0; r < y_cpp.rows(); ++r) {
        float sum = 0.0f;
        for (std::size_t c = 0; c < y_cpp.cols(); ++c) {
            sum += y_cpp.at(r, c);
        }
        std::cout << "row " << r << " sum = " << sum << '\n';
    }

    const float y_max = max_abs_error(y_cpp, refs.at("y"));
    std::cout << "y max error = " << y_max << '\n';

    if (y_max < 1e-5f) {
        std::cout << "M4 batch inference PASS\n";
        return 0;
    }

    std::cout << "M4 batch inference FAIL\n";
    return 1;
}