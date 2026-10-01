#include "mini_infer/tensor.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include<cmath>
#include<algorithm>
#include<cassert>

using mini_infer::Tensor;
using mini_infer::linear;

float max_abs_error(const Tensor& got,const Tensor& want){
    assert(got.rows()==want.rows()&&got.cols()==want.cols());
    float max=0.0f;
    for(std::size_t i=0;i<got.rows();i++){
        for(std::size_t j=0;j<got.cols();j++){
            float comp=std::fabs(got.at(i,j)-want.at(i,j));
            if(comp>max)max=comp;
        }
    }
    return max;
}

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

                row_stream >> value;
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

    const Tensor& x=tensors.at("x");
    const Tensor& W1=tensors.at("W1");
    const Tensor& b1 = tensors.at("b1");

    //第一层
    Tensor h_cpp=linear(x,W1,b1);
    h_cpp=h_cpp.relu();

    //第二层
    const Tensor& W2=tensors.at("W2");
    const Tensor& b2=tensors.at("b2");

    Tensor o_cpp=linear(h_cpp,W2,b2);
    o_cpp=o_cpp.relu();

    //最后
    Tensor y_cpp=o_cpp.softmax();

    //比较
    const Tensor& h=tensors.at("h");
    const Tensor& o=tensors.at("o");
    const Tensor& y=tensors.at("y");
    float h_max=max_abs_error(h_cpp,h);
    float o_max=max_abs_error(o_cpp,o);
    float y_max=max_abs_error(y_cpp,y);

    //print
    std::cout<<"h max error = "<<h_max<<"\n";
    std::cout<<"o max error = "<<o_max<<"\n";
    std::cout<<"y max error = "<<y_max<<"\n";

    if (h_max < 1e-5f && o_max < 1e-5f && y_max < 1e-5f) {
        std::cout << "M1 PASS\n";
        return 0;
    }

    std::cout << "M1 FAIL\n";
    return 1;
}