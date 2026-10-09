#pragma once

#include "mini_infer/tensor.hpp"

namespace mini_infer {

class Mlp {
public:
    Mlp(Tensor W1, Tensor b1, Tensor W2, Tensor b2);

    // x: (N, in_features)
    // return: (N, classes)，每一行是 softmax 概率
    Tensor forward(const Tensor& x) const;

private:
    Tensor W1_;
    Tensor b1_;
    Tensor W2_;
    Tensor b2_;
};

}  // namespace mini_infer