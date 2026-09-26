#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

std::vector<float> matmul(
    const std::vector<float>& A, std::size_t M, std::size_t K,
    const std::vector<float>& B, std::size_t K2, std::size_t N) {
    // TODO 1: 如果 K != K2，就 assert 失败。
    assert(K==K2);
    // TODO 2: 创建 result，大小为 M * N，初值全为 0。
    std::vector<float> result(M*N,0.0f);
    // TODO 3: 三层循环计算：
    // for i in 0..M-1
    //   for j in 0..N-1
    //     for k in 0..K-1
    //       result[i * N + j] += A[i * K + k] * B[k * N + j];
    for(std::size_t i=0;i<M;i++){
        for(std::size_t j=0;j<N;j++){
            for(std::size_t k=0;k<K;k++){
                result[i*N+j]+=A[i*K+k]*B[k*N+j];
            }
        }
    }
    // TODO 4: 返回 result。
    return result;
}

int main() {
    std::vector<float> A = {1, 2, 3, 4, 5, 6};
    std::vector<float> B = {7, 8, 9, 10, 11, 12};

    std::vector<float> C = matmul(A, 2, 3, B, 3, 2);

    for (float x : C) {
        std::cout << x << ' ';
    }
    std::cout << '\n';

    return 0;
}