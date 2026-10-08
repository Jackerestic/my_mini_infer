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

Tensor matmul_ijk(const Tensor& a, const Tensor& b) {
    const std::size_t m = a.rows();
    const std::size_t kdim = a.cols();
    const std::size_t n = b.cols();

    const float* A = &a.at(0, 0);
    const float* B = &b.at(0, 0);

    Tensor c(m, n);
    float* C = &c.at(0, 0);

    for (std::size_t r = 0; r < m; ++r) {
        for (std::size_t col = 0; col < n; ++col) {
            C[r * n + col] = 0.0f;
            for (std::size_t k = 0; k < kdim; ++k) {
                C[r * n + col] += A[r * kdim + k] * B[k * n + col];
            }
        }
    }

    return c;
}

Tensor matmul_ikj(const Tensor& a, const Tensor& b) {
    const std::size_t m = a.rows();
    const std::size_t kdim = a.cols();
    const std::size_t n = b.cols();

    const float* A = &a.at(0, 0);
    const float* B = &b.at(0, 0);

    Tensor c(m, n);
    float* C = &c.at(0, 0);

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

    float checksum_ijk = 0.0f;
    float checksum_ikj = 0.0f;

    // Warm up both implementations.
    time_ms([&] { return matmul_ijk(a, b); }, checksum_ijk);
    time_ms([&] { return matmul_ikj(a, b); }, checksum_ikj);

    if (std::fabs(checksum_ijk - checksum_ikj) > 1e-3f) {
        std::cout << "checksum mismatch after warm-up\n";
        return 1;
    }

    std::vector<double> ijk_times;
    std::vector<double> ikj_times;
    std::vector<double> ratios;

    for (int round = 0; round < 20; ++round) {
        double ijk_ms = 0.0;
        double ikj_ms = 0.0;

        if (round % 2 == 0) {
            ijk_ms = time_ms([&] { return matmul_ijk(a, b); }, checksum_ijk);
            ikj_ms = time_ms([&] { return matmul_ikj(a, b); }, checksum_ikj);
        } else {
            ikj_ms = time_ms([&] { return matmul_ikj(a, b); }, checksum_ikj);
            ijk_ms = time_ms([&] { return matmul_ijk(a, b); }, checksum_ijk);
        }

        ijk_times.push_back(ijk_ms);
        ikj_times.push_back(ikj_ms);
        ratios.push_back(ijk_ms / ikj_ms);

        std::cout << "round " << round + 1
                  << ": ijk=" << ijk_ms
                  << " ms, ikj=" << ikj_ms
                  << " ms, ratio=" << ijk_ms / ikj_ms
                  << '\n';
    }

    std::cout << "size: " << n << "x" << n << '\n';
    std::cout << "ijk median: " << median(ijk_times) << " ms\n";
    std::cout << "ikj median: " << median(ikj_times) << " ms\n";
    std::cout << "median per-round speedup: " << median(ratios) << "x\n";
    std::cout << "checksum: " << checksum_ikj << '\n';

    return 0;
}