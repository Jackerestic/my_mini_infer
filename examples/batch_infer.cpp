#include<iostream>
#include "mini_infer/mlp.hpp"

using mini_infer::Tensor;
using mini_infer::Mlp;

int main()
{
    Tensor W1(2,2,1.0f);
    Tensor b1(1,2,0.0f);
    Tensor W2(2,2,1.0f);
    Tensor b2(1,2,0.0f);

    Mlp model(W1,b1,W2,b2);

    Tensor x(3,2,1.0f);

    Tensor y=model.forward(x);

    std::cout<<"y shape: "<<y.rows()<<" x "<<y.cols()<<"\n";

    for(std::size_t r=0;r<y.rows();r++){
        float sum=0.0f;
        for(std::size_t c=0;c<y.cols();c++){
            sum+=y.at(r,c);
        }
        std::cout<<"row "<<r<<" sum = "<<sum<<"\n";
    }

    return 0;
}