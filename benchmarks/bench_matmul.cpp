#include "mini_infer/tensor.hpp"

#include <algorithm>
#include <chrono>
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

double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

int main() {
    const std::size_t n = 512;
    Tensor a = make_matrix(n, 0.001f);
    Tensor b = make_matrix(n, 0.002f);

    // Warm up.
    {
        Tensor c = a.matmul(b);
        volatile float sink = c.at(0, 0);
        (void)sink;
    }

    std::vector<double> times;
    float checksum = 0.0f;

    for (int run = 0; run < 5; ++run) {
        const auto start = Clock::now();
        Tensor c = a.matmul(b);
        const auto end = Clock::now();

        const double ms =
            std::chrono::duration<double, std::milli>(end - start).count();
        times.push_back(ms);
        checksum += c.at(0, 0);

        std::cout << "run " << run + 1 << ": " << ms << " ms\n";
    }

    std::cout << "size: " << n << "x" << n << '\n';
    std::cout << "median: " << median(times) << " ms\n";
    std::cout << "checksum: " << checksum << '\n';

    return 0;
}