#pragma once

#include <cstddef>
#include <vector>

namespace mini_infer {

class Tensor {
public:
    Tensor(std::size_t rows, std::size_t cols, float value = 0.0f);

    std::size_t rows() const;
    std::size_t cols() const;
    std::size_t size() const;
    const std::vector<std::size_t>& shape() const;

    float& at(std::size_t row, std::size_t col);
    const float& at(std::size_t row, std::size_t col) const;

    void fill(float value);
    void print() const;

    // 逐元素加法，要求两个 Tensor 形状相同。
    Tensor add(const Tensor& other) const;

    // 矩阵乘法，要求 this->cols() == other.rows()。
    Tensor matmul(const Tensor& other) const;

    //转置矩阵
    Tensor transpose() const;

    //转化为概率
    Tensor softmax() const;

    //逐元素rule。通俗来说就是取x和0中的较大值
    Tensor relu() const;

private:
    std::vector<std::size_t> shape_;
    std::vector<float> data_;
};

Tensor linear(const Tensor& x,const Tensor& W,const Tensor& b);

} // namespace mini_infer
