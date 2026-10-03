#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "mini_infer/tensor.hpp"
using mini_infer::Tensor;

TEST_CASE("Tensor basic","[tensor]"){
    Tensor t(2,3,1.5f);

    REQUIRE(t.rows()==2);
    CHECK(t.cols()==3);
    CHECK(t.size()==6);
    CHECK(t.at(0,0)==1.5f);
}

TEST_CASE("add two tensors","[tensor]"){
    Tensor a(2,2);
    Tensor b(2,2);

    a.at(0,0)=1.0f;
    a.at(0,1)=2.0f;
    a.at(1,0)=3.0f;
    a.at(1,1)=4.0f;

    b.at(0,0)=2.0f;
    b.at(0,1)=3.0f;
    b.at(1,0)=6.0f;
    b.at(1,1)=10.0f;

    Tensor c=a.add(b);

    REQUIRE(c.rows()==2);
    CHECK(c.at(0,0)==3.0f);
    CHECK(c.at(1,1)==14.0f);
}

TEST_CASE("softmax rows sum to one","[tensor]"){
    Tensor t(2,3);

    t.at(0,0)=1.0f;
    t.at(0,1)=2.0f;
    t.at(0,2)=3.0f;
    t.at(1,0)=4.0f;
    t.at(1,1)=5.0f;
    t.at(1,2)=6.0f;

    Tensor y=t.softmax();

    REQUIRE(y.rows()==2);
    CHECK(y.cols()==3);

    const float row0=y.at(0,0)+y.at(0,1)+y.at(0,2);
    const float row1=y.at(1,0)+y.at(1,1)+y.at(1,2);

    CHECK(row0>0.9999f);
    CHECK(row0<1.0001f);
    CHECK(row1>0.9999f);
    CHECK(row1<1.0001f);
}

TEST_CASE("matmul two tensors","[tensor]"){
    Tensor x(2,3);
    Tensor w(3,2);

    x.at(0,0)=1.0f;
    x.at(0,1)=2.0f;
    x.at(0,2)=3.0f;
    x.at(1,0)=4.0f;
    x.at(1,1)=5.0f;
    x.at(1,2)=6.0f;

    w.at(0,0)=3.0f;
    w.at(0,1)=2.0f;
    w.at(1,0)=1.0f;
    w.at(1,1)=4.0f;
    w.at(2,0)=2.0f;
    w.at(2,1)=1.0f;

    Tensor m=x.matmul(w);

    REQUIRE(m.rows()==2);
    CHECK(m.cols()==2);
    CHECK(m.at(0,0)==11.0f);
    CHECK(m.at(1,1)==34.0f);
}

TEST_CASE("linear three tensors:x,w,b","[tensor]"){
    Tensor x(2,3);
    Tensor w(2,3);
    Tensor b(1,2);

    x.at(0,0)=1.0f;
    x.at(0,1)=2.0f;
    x.at(0,2)=3.0f;
    x.at(1,0)=4.0f;
    x.at(1,1)=5.0f;
    x.at(1,2)=6.0f;

    w.at(0,0)=1.0f;
    w.at(0,1)=3.0f;
    w.at(0,2)=4.0f;
    w.at(1,0)=2.0f;
    w.at(1,1)=5.0f;
    w.at(1,2)=1.0f;

    b.at(0,0)=0.5f;
    b.at(0,1)=-0.5f;

    Tensor r=linear(x,w,b);
    REQUIRE(r.rows()==2);
    CHECK(r.cols()==2);
    CHECK(r.at(0,0)==19.5f);
    CHECK(r.at(1,1)==38.5f);
}