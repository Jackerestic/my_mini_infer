// Tensor 的单元测试。
//
// 运行方式：
//   build.bat test                     （或 ctest --test-dir build）
//
// 注意：这里刻意**不用** assert 来判定结果。
// assert 在 Release 下会被 NDEBUG 整个关掉，用 assert 写的测试会静默失效
// （编译能过、跑起来也"通过"，其实一条都没检查）。
// 所以用下面的 CHECK 宏自己计数，最后用返回值告诉 CTest 有没有失败。
//
// 所有期望值都是手算出来的，不是从程序里抄的。

#include "mini_infer/tensor.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

using mini_infer::Tensor;

namespace {

int g_failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::cout << "FAIL  " << #cond << "  (line " << __LINE__ << ")\n"; \
            ++g_failures;                                                      \
        }                                                                      \
    } while (false)

// 浮点不能直接用 == 比：0.1f 这种值存不进 float。
// 这里用相对误差，能同时照顾大数和 0。
bool nearly(float got, float want) {
    return std::fabs(got - want) <= 1e-5f * (1.0f + std::fabs(want));
}

// 按手算好的二维表逐格比对，顺便检查 shape 对不对。
void expect(const char* what, const Tensor& got,
            const std::vector<std::vector<float>>& want) {
    if (got.rows() != want.size() || got.cols() != want[0].size()) {
        std::cout << "FAIL  " << what << ": shape " << got.rows() << "x"
                  << got.cols() << ", want " << want.size() << "x"
                  << want[0].size() << '\n';
        ++g_failures;
        return;
    }

    bool ok = true;
    for (std::size_t r = 0; r < got.rows(); ++r) {
        for (std::size_t c = 0; c < got.cols(); ++c) {
            if (!nearly(got.at(r, c), want[r][c])) {
                std::cout << "FAIL  " << what << " (" << r << "," << c
                          << "): got " << got.at(r, c) << ", want " << want[r][c]
                          << '\n';
                ok = false;
                ++g_failures;
            }
        }
    }
    if (ok) {
        std::cout << "  ok  " << what << '\n';
    }
}

// 用二维表建 Tensor，写测试时不用手敲一堆 at()。
Tensor make(const std::vector<std::vector<float>>& rows) {
    Tensor t(rows.size(), rows[0].size());
    for (std::size_t r = 0; r < rows.size(); ++r) {
        for (std::size_t c = 0; c < rows[r].size(); ++c) {
            t.at(r, c) = rows[r][c];
        }
    }
    return t;
}

void test_basic() {
    std::cout << "--- 构造 / at / fill ---\n";

    Tensor t(2, 3, 1.5f);
    CHECK(t.rows() == 2);
    CHECK(t.cols() == 3);
    CHECK(t.size() == 6);
    CHECK(t.shape().size() == 2);
    CHECK(t.at(0, 0) == 1.5f);
    CHECK(t.at(1, 2) == 1.5f);  // 构造函数用 value 把整个 Tensor 填满

    // value 的默认值是 0.0f
    Tensor z(2, 2);
    CHECK(z.at(0, 0) == 0.0f);
    CHECK(z.at(1, 1) == 0.0f);

    t.at(0, 0) = 9.0f;
    CHECK(t.at(0, 0) == 9.0f);

    t.fill(2.0f);
    CHECK(t.at(0, 0) == 2.0f);  // fill 是全部覆盖
    CHECK(t.at(1, 2) == 2.0f);
}

void test_add() {
    std::cout << "--- add ---\n";

    Tensor a = make({{1, 2}, {3, 4}});
    Tensor b = make({{5, 6}, {7, 8}});

    expect("add 2x2", a.add(b), {{6, 8}, {10, 12}});

    // 加法满足交换律
    expect("add 交换律 b+a", b.add(a), {{6, 8}, {10, 12}});

    // 非方阵
    Tensor p = make({{1, 2, 3}, {4, 5, 6}});
    Tensor q = make({{10, 20, 30}, {40, 50, 60}});
    expect("add 2x3", p.add(q), {{11, 22, 33}, {44, 55, 66}});

    // 零元：t + 0 == t
    expect("add 零元", a.add(Tensor(2, 2, 0.0f)), {{1, 2}, {3, 4}});

    // add 是 const 且返回新对象，原对象不能被改动
    CHECK(a.at(0, 0) == 1.0f);
    CHECK(b.at(0, 0) == 5.0f);
}

void test_matmul() {
    std::cout << "--- matmul ---\n";

    Tensor a = make({{1, 2}, {3, 4}});
    Tensor b = make({{5, 6}, {7, 8}});

    // 1*5+2*7=19   1*6+2*8=22
    // 3*5+4*7=43   3*6+4*8=50
    expect("matmul 2x2", a.matmul(b), {{19, 22}, {43, 50}});

    // 非方阵 2x3 * 3x4 -> 2x4。
    // 这个用例很关键：2x2 * 2x2 这种方阵用例，即使把行列写反了也照样能过，
    // 只有非方阵才能把 bug 逼出来。
    Tensor p = make({{1, 2, 3}, {4, 5, 6}});
    Tensor q = make({{1, 0, 2, 1}, {0, 1, 3, 0}, {2, 1, 0, 1}});
    expect("matmul 2x3 * 3x4", p.matmul(q), {{7, 5, 8, 4}, {16, 11, 23, 10}});

    // 1x3 * 3x1 -> 1x1：1*4 + 2*5 + 3*6 = 32
    expect("matmul 1x3 * 3x1",
           make({{1, 2, 3}}).matmul(make({{4}, {5}, {6}})), {{32}});

    // 单位矩阵：A * I == I * A == A
    Tensor id = make({{1, 0}, {0, 1}});
    expect("A * I == A", a.matmul(id), {{1, 2}, {3, 4}});
    expect("I * A == A", id.matmul(a), {{1, 2}, {3, 4}});

    // 零矩阵
    expect("A * 0 == 0", a.matmul(Tensor(2, 2, 0.0f)), {{0, 0}, {0, 0}});

    // 矩阵乘法不满足交换律：b*a 和 a*b 不一样
    // 5*1+6*3=23   5*2+6*4=34
    // 7*1+8*3=31   7*2+8*4=46
    expect("matmul 不交换 b*a", b.matmul(a), {{23, 34}, {31, 46}});

    // 原对象不能被改动
    CHECK(a.at(0, 0) == 1.0f);
    CHECK(b.at(0, 0) == 5.0f);
}

void test_relu(){
    std::cout<<"--- relu ---\n";

    expect("relu 混合", make({{-1, 2}, {-3, 4}}).relu(), {{0, 2}, {0, 4}});
    expect("relu 全负", make({{-1, -2}, {-3, -4}}).relu(), {{0, 0}, {0, 0}});
    expect("relu 全正", make({{1, 2}, {3, 4}}).relu(), {{1, 2}, {3, 4}});

    // 形状不变 —— 这正是 relu 和 matmul 最大的区别
    Tensor r = make({{-1, 2, -3}, {4, -5, 6}}).relu();
    CHECK(r.rows() == 2);
    CHECK(r.cols() == 3);

    // 原对象不能被改动
    Tensor a = make({{-1, 2}});
    a.relu();
    CHECK(a.at(0, 0) == -1.0f);
}

void test_transpose(){
    std::cout<<"--- transpose ---\n";

    expect("transpose 2x3->3x2",make({{1,2,3},{4,5,6}}).transpose(),{{1,4},{2,5},{3,6}});

    //expect内部已经查了，此处显示再写一遍
    Tensor t=make({{1,2,3},{4,5,6}}).transpose();
    CHECK(t.rows()==3);
    CHECK(t.cols()==2);

    //边界:1xN和Nx1
    expect("transpose 1x3->3x1",make({{1,2,3}}).transpose(),{{1},{2},{3}});
    expect("transpose 3x1->1x3",make({{1},{2},{3}}).transpose(),{{1,2,3}});
    
    //性质测试
    expect("(A.T).T=A",make({{1,2,3},{4,5,6}}).transpose().transpose(),{{1,2,3},{4,5,6}});

    //原对象不能被改动
    Tensor a=make({{1,2,3},{4,5,6}});
    a.transpose();
    CHECK(a.rows()==2);
    CHECK(a.cols()==3);
    CHECK(a.at(0,2)==3.0f);
}

void test_softmax(){
    std::cout<<"--- softmax ---\n";

    expect("softmax{2,1}",make({{2,1}}).softmax(),{{0.7310586,0.2689414}});
    expect("两个0",make({{0,0}}).softmax(),{{0.5,0.5}});
    expect("三个数",make({{1,1,1}}).softmax(),{{0.3333333,0.3333333,0.3333333}});
    expect("减去最大值测试",make({{1000,1001}}).softmax(),{{0.2689414,0.7310586}});

    //性质测试
    Tensor t=make({{3,4}}).softmax();
    CHECK(std::fabs(t.at(0,0)+t.at(0,1)-1.0)<1e-5);
}

void test_linear(){
    expect("linear normal test",linear(make({{1,2,3},{4,5,6}}),make({{1,3,4},{2,5,1}}),make({{0.5,-0.5}})),{{19.5,14.5},{43.5,38.5}});
}

void test_mlp(){
    std::cout<<"--- mlp ---\n";

    Tensor x=make({{1,2}});
    Tensor W1=make({{1,2},{3,4}});
    Tensor b1=make({{1,-1}});
    Tensor W2=make({{1,-1},{-1,1}});
    Tensor b2=make({{0,0}});

    //每层验证，出错迅速判断
    Tensor h=linear(x,W1,b1);
    expect("mlp第一层linear",h,{{6,10}});

    h=h.relu();
    expect("mlp第一层relu",h,{{6,10}});

    Tensor o=linear(h,W2,b2);
    expect("mlp第二层linear",o,{{-4,4}});

    o=o.relu();
    expect("mlp第二层relu",o,{{0,4}});

    Tensor p=o.softmax();
    expect("mlp softmax 输出",p,{{0.0179862,0.9820138}});
}

} // namespace

int main() {
    test_basic();
    test_add();
    test_matmul();
    test_relu();
    test_transpose();
    test_softmax();
    test_linear();
    test_mlp();

    if (g_failures == 0) {
        std::cout << "\ntest_tensor passed\n";
        return 0;
    }
    std::cout << "\ntest_tensor FAILED: " << g_failures << " check(s)\n";
    return 1;
}
