# M3 第二步：把 at() 变成真正可内联的访问

> 这一步比继续调循环顺序更根本。
> 本次任务由你实现，我只给验收标准和检查。

## 1. 问题

`include/mini_infer/tensor.hpp` 里只有：

```cpp
float& at(std::size_t row, std::size_t col);
const float& at(std::size_t row, std::size_t col) const;
```

实现写在 `src/tensor.cpp` 里。

后果：

- `at()` 是跨编译单元调用；
- 没有 `-flto` 时，编译器无法内联它；
- 每次访问元素都要付一次函数调用开销；
- M4 的批处理推理会大量调用 `at()`，影响很大。

## 2. 快速验证

我临时写了一个对照实验，512x512，同样 i-k-j 顺序：

```text
通过 at()         262.798 ms
通过裸 float*      18.7376 ms
ratio              14.0252x
checksum           两边一致

```

结论：`at()` 跨 TU 调用的开销非常大，而且和 cache 优化无关。

## 3. 要改什么

把小函数放进头文件，让它们可以内联：

- `rows()`
- `cols()`
- `size()`
- `shape()`
- `at()` 两个版本

推荐直接在类内部定义，这样它们自动是 inline：

```cpp
std::size_t rows() const {
    return shape_[0];
}

std::size_t cols() const {
    return shape_[1];
}

float& at(std::size_t row, std::size_t col) {
    assert(row < rows() && col < cols());
    return data_[row * cols() + col];
}
```

`assert` 需要：

```cpp
#include <cassert>
```

然后把 `src/tensor.cpp` 里对应的旧定义删掉，否则会重复定义。

## 4. 可选：提供 data()

如果以后要写高性能循环，可以再提供：

```cpp
float* data() { return data_.data(); }
const float* data() const { return data_.data(); }
```

但当前第一步只要先把 `at()` 内联做好。

## 5. 你自己要写的 benchmark

新建：

```text
benchmarks/bench_at.cpp
```

要求：

- 实现 `matmul_at_ikj`：完全用 `at()` 访问。
- 实现 `matmul_ptr_ikj`：用裸 `float*` 访问。
- 两种写法结果必须一致。
- 输出耗时和比值。

建议：

- 先做 512x512；
- 各跑 3 到 5 次；
- 取中位数；
- 不要只看单轮。

预期：

- 改之前：`at()` 版本明显慢，可能 10 倍以上；
- 改之后：`at()` 版本和裸指针版本接近，理想情况在 2 倍以内。

这个 benchmark 的作用是证明 inline 改动真的有效。

## 6. CMake 目标

在 CMakeLists.txt 末尾加：

```cmake
add_executable(bench_at
    benchmarks/bench_at.cpp
)
target_link_libraries(bench_at PRIVATE mini_infer)
```

## 7. 验收标准

- [ ] `at()` 和 `rows/cols/size/shape` 已经放到头文件，能内联；
- [ ] `src/tensor.cpp` 里旧定义已经删除；
- [ ] `bench_at` 能编译运行，checksum 一致；
- [ ] inline 后 `at()` 版本明显接近裸指针版本；
- [ ] `ctest` 仍然 11/11 全过；
- [ ] `-Wall -Wextra -Wpedantic` 零警告。

## 8. 提交

```powershell
git add include/mini_infer/tensor.hpp src/tensor.cpp benchmarks/bench_at.cpp CMakeLists.txt
git commit -m "perf(m3): make tensor accessors inline"
```

完成后把 benchmark 的 before/after 数字发我，我帮你判断是否真的消除了跨 TU 调用开销。