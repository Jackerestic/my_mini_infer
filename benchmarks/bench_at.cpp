#include "mini_infer/tensor.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

using mini_infer::Tensor;
using Clock = std::chrono::steady_clock;

Tensor make_matrix(std::size_t n, float scale) {
    Tensor t(n, n);
    for (std::size_t r = 0; r < n; ++r) {
        for (std::size_t c = 0; c < n; ++c) {
            const std::size_t k = (r * 131 + c * 17) % 997 + 1;
            t.at(r, c) = scale * static_cast<float>(k);
        }
    }
    return t;
}


Tensor matmul_at_ikj(const Tensor& a, const Tensor& b) {
    const std::size_t m = a.rows();
    const std::size_t kdim = a.cols();
    const std::size_t n = b.cols();

    Tensor c(m, n);
    c.fill(0.0f);

    for (std::size_t r = 0; r < m; ++r) {
        for (std::size_t k = 0; k < kdim; ++k) {
            const float av = a.at(r,k);
            for (std::size_t col = 0; col < n; ++col) {
                c.at(r,col)+= av * b.at(k,col);
            }
        }
    }

    return c;
}

Tensor matmul_ptr_ikj(const Tensor& a,const Tensor& b){
    const std::size_t m = a.rows();
    const std::size_t kdim = a.cols();
    const std::size_t n = b.cols();

    const float* A = a.data();
    const float* B = b.data();

    Tensor c(m, n);
    c.fill(0.0f);
    float* C = c.data();

    for (std::size_t r = 0; r < m; ++r) {
        for (std::size_t k = 0; k < kdim; ++k) {
            const float av = A[r * kdim + k];
            for (std::size_t col = 0; col < n; ++col) {
                C[r * n + col] += av * B[k * n + col];
            }
        }
    }

    return c;
}

template <typename Fn>
double time_ms(Fn fn, float& checksum) {
    const auto start = Clock::now();
    Tensor c = fn();
    const auto end = Clock::now();
    checksum += c.at(0, 0);
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

 int main() {
    const std::size_t n = 1024;
    Tensor a = make_matrix(n, 0.001f);
    Tensor b = make_matrix(n, 0.002f);

    float checksum_at_ikj = 0.0f;
    float checksum_ptr_ikj = 0.0f;

    // Warm up both implementations.
    time_ms([&] { return matmul_at_ikj(a, b); }, checksum_at_ikj);
    time_ms([&] { return matmul_ptr_ikj(a, b); }, checksum_ptr_ikj);

    if (std::fabs(checksum_at_ikj - checksum_ptr_ikj) > 1e-3f) {
        std::cout << "checksum mismatch after warm-up\n";
        return 1;
    }

    std::vector<double> at_ikj_times;
    std::vector<double> ptr_ikj_times;
    std::vector<double> ratios;

    for (int round = 0; round < 20; ++round) {
        double at_ikj_ms = 0.0;
        double ptr_ikj_ms = 0.0;

        if (round % 2 == 0) {
            at_ikj_ms = time_ms([&] { return matmul_at_ikj(a, b); }, checksum_at_ikj);
            ptr_ikj_ms = time_ms([&] { return matmul_ptr_ikj(a, b); }, checksum_ptr_ikj);
        } else {
            ptr_ikj_ms = time_ms([&] { return matmul_ptr_ikj(a, b); }, checksum_ptr_ikj);
            at_ikj_ms = time_ms([&] { return matmul_at_ikj(a, b); }, checksum_at_ikj);
        }

        at_ikj_times.push_back(at_ikj_ms);
        ptr_ikj_times.push_back(ptr_ikj_ms);
        ratios.push_back(at_ikj_ms / ptr_ikj_ms);

        std::cout << "round " << round + 1
                  << ": at=" << at_ikj_ms
                  << " ms, ptr=" << ptr_ikj_ms
                  << " ms, ratio=" << at_ikj_ms / ptr_ikj_ms
                  << '\n';
    }

    std::cout << "size: " << n << "x" << n << '\n';
    std::cout << "at_ikj median: " << median(at_ikj_times) << " ms\n";
    std::cout << "ptr_ikj median: " << median(ptr_ikj_times) << " ms\n";
    std::cout << "median per-round speedup: " << median(ratios) << "x\n";
    std::cout << "checksum: " << checksum_ptr_ikj << '\n';

    return 0;
}