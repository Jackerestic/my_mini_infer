#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "mini_infer/mlp.hpp"
#include <cmath>

using mini_infer::Tensor;
using mini_infer::Mlp;

TEST_CASE("Mlp test","[tensor]"){
    Tensor x(3,2,1.0f);
    Tensor W1(2,2,1.0f);
    Tensor b1(1,2,0.0f);
    Tensor W2(2,2,1.0f);
    Tensor b2(1,2,0.0f);

    Mlp t(W1,b1,W2,b2);
    Tensor y=t.forward(x);

    REQUIRE(y.rows()==x.rows());
    CHECK(y.cols()==2);

    for(std::size_t r=0;r<y.rows();r++){
        float sum=0.0f;
        bool judge=1;
        for(std::size_t c=0;c<y.cols();c++){
            float tmp=y.at(r,c);
            if(!(tmp>=0.0f&&tmp<=1.0f)){
                judge=0;
            }
            sum+=tmp;
        }
        CHECK(std::fabs(sum-1.0f)<1e-5f);
        CHECK(judge);
    }

}