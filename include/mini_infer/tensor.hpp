#pragma once

#include <cstddef>
#include <vector>
#include <cassert>

namespace mini_infer {

class Tensor {
public:
    Tensor(std::size_t rows, std::size_t cols, float value = 0.0f)
        :shape_{rows,cols},data_(rows*cols,value){
            assert(rows>0&&cols>0);
        }

    const std::vector<std::size_t>& shape() const{
        return shape_;
    }
    std::size_t rows() const{
        return shape_[0];
    }
    std::size_t cols() const{
        return shape_[1];
    }
    std::size_t size() const{
        return data_.size();
    }
    

    float& at(std::size_t row, std::size_t col){
        assert(row<rows()&&col<cols());
        return data_[row*cols()+col];
    }
    const float& at(std::size_t row, std::size_t col) const{
        assert(row<rows()&&col<cols());
        return data_[row*cols()+col];
    }

    float* data(){
        return data_.data();
    }

    const float* data() const{
        return data_.data();
    }

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
