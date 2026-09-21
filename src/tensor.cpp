#include "mini_infer/tensor.hpp"

#include <cassert>
#include <iostream>
#include <cmath>

namespace mini_infer {

Tensor::Tensor(std::size_t rows, std::size_t cols, float value)
    : shape_{rows, cols}, data_(rows * cols, value) {
    assert(rows > 0 && cols > 0);
}

std::size_t Tensor::rows() const {
    return shape_.at(0);
}

std::size_t Tensor::cols() const {
    return shape_.at(1);
}

std::size_t Tensor::size() const {
    return data_.size();
}

const std::vector<std::size_t>& Tensor::shape() const {
    return shape_;
}

float& Tensor::at(std::size_t row, std::size_t col) {
    assert(row < rows() && col < cols());
    return data_[row * cols() + col];
}

const float& Tensor::at(std::size_t row, std::size_t col) const {
    assert(row < rows() && col < cols());
    return data_[row * cols() + col];
}

void Tensor::fill(float value) {
    for (float& x : data_) {
        x = value;
    }
}

void Tensor::print() const {
    std::cout << "Tensor(" << rows() << ", " << cols() << ")\n";
    for (std::size_t r = 0; r < rows(); ++r) {
        for (std::size_t c = 0; c < cols(); ++c) {
            std::cout << at(r, c) << ' ';
        }
        std::cout << '\n';
    }
}

Tensor Tensor::add(const Tensor& other) const {
    assert(rows() == other.rows() && cols() == other.cols());

    Tensor result(rows(), cols());
    for (std::size_t r = 0; r < rows(); ++r) {
        for (std::size_t c = 0; c < cols(); ++c) {
            result.at(r, c) = at(r, c) + other.at(r, c);
        }
    }
    return result;
}

Tensor Tensor::matmul(const Tensor& other) const {
    assert(cols() == other.rows());

    Tensor result(rows(), other.cols());
    for (std::size_t r = 0; r < rows(); ++r) {
        for (std::size_t c = 0; c < other.cols(); ++c) {
            result.at(r, c) = 0.0f;
            for (std::size_t k = 0; k < cols(); ++k) {
                result.at(r, c) += at(r, k) * other.at(k, c);
            }
        }
    }
    return result;
}

Tensor Tensor::relu() const{
    Tensor result(rows(),cols());
    for(std::size_t r=0;r<rows();r++){
        for(std::size_t c=0;c<cols();c++){
            const float x=at(r,c);
            result.at(r,c)=(x>0.0f)?x:0.0f;
        }
    }
    return result;
}

Tensor Tensor::transpose() const{
    Tensor result(cols(),rows());
    for(std::size_t r=0;r<rows();r++){
        for(std::size_t c=0;c<cols();c++){
            result.at(c,r)=at(r,c);
        }
    }
    return result;
}

Tensor Tensor::softmax() const{
    Tensor result(rows(),cols());
    for(std::size_t r=0;r<rows();r++){
        float max=at(r,0);
        for(std::size_t c=0;c<cols();c++){
            float x=at(r,c);
            max=(max>x?max:x);
        }
        float sum=0.0f;
        for(std::size_t c=0;c<cols();c++){
            result.at(r,c)=std::exp(at(r,c)-max);
            sum+=result.at(r,c);
        }
        for(std::size_t c=0;c<cols();c++){
            result.at(r,c)=result.at(r,c)/sum;
        }
    }
    return result;
}

Tensor linear(const Tensor& x,const Tensor& W,const Tensor& b){
    assert(W.rows()==b.cols());

    Tensor result=x.matmul(W.transpose());
    for(std::size_t r=0;r<x.rows();r++){
        for(std::size_t c=0;c<W.rows();c++){
            result.at(r,c)+=b.at(0,c);
        }
    }
    return result;
}

} // namespace mini_infer
