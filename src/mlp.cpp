#include "mini_infer/mlp.hpp"

#include <utility>

namespace mini_infer {

Mlp::Mlp(Tensor W1, Tensor b1, Tensor W2, Tensor b2)
    : W1_(std::move(W1)),
      b1_(std::move(b1)),
      W2_(std::move(W2)),
      b2_(std::move(b2)) {}

Tensor Mlp::forward(const Tensor& x) const {
    Tensor h = linear(x, W1_, b1_);
    h = h.relu();

    Tensor o = linear(h, W2_, b2_);
    o = o.relu();

    return o.softmax();
}

}  // namespace mini_infer