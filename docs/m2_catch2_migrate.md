# M2 第一步：把测试迁移到 Catch2

## 1. 你现在已有的开头

`tests/test_tensor_catch2.cpp` 开头应该是：

```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
```

注意：
- `#define CATCH_CONFIG_MAIN` 必须在 `#include "catch.hpp"` 前面；
- 一个项目里只允许一个文件定义 `CATCH_CONFIG_MAIN`。

## 2. 加一个最小的 TEST_CASE

在开头下面继续写：

```cpp
#include "mini_infer/tensor.hpp"

using mini_infer::Tensor;

TEST_CASE("Tensor basic", "[tensor]") {
    Tensor t(2, 3, 1.5f);

    REQUIRE(t.rows() == 2);
    CHECK(t.cols() == 3);
    CHECK(t.size() == 6);
    CHECK(t.at(0, 0) == 1.5f);
}
```

`REQUIRE` 和 `CHECK` 的区别：
- `REQUIRE`：失败就停止这个测试用例；
- `CHECK`：失败也继续执行后面的检查。

## 3. 第一批建议迁移的 5 个用例

不要一次迁移全部。先迁移这 5 类：

1. 构造 / rows / cols / size
2. add 同形状相加
3. matmul 非方阵
4. softmax 每行和为 1
5. linear 的结果

每个用例用：

```cpp
TEST_CASE("用例名字", "[tensor]") {
    // 准备数据
    // REQUIRE / CHECK
}
```

例子：add

```cpp
TEST_CASE("add two tensors", "[tensor]") {
    Tensor a(2, 2);
    Tensor b(2, 2);

    a.at(0, 0) = 1.0f;
    a.at(0, 1) = 2.0f;
    a.at(1, 0) = 3.0f;
    a.at(1, 1) = 4.0f;

    b.at(0, 0) = 5.0f;
    b.at(0, 1) = 6.0f;
    b.at(1, 0) = 7.0f;
    b.at(1, 1) = 8.0f;

    Tensor c = a.add(b);

    REQUIRE(c.rows() == 2);
    CHECK(c.at(0, 0) == 6.0f);
    CHECK(c.at(1, 1) == 12.0f);
}
```

例子：softmax 行和为 1

```cpp
TEST_CASE("softmax rows sum to one", "[tensor]") {
    Tensor t(2, 3);

    t.at(0, 0) = 1.0f;
    t.at(0, 1) = 2.0f;
    t.at(0, 2) = 3.0f;

    t.at(1, 0) = 4.0f;
    t.at(1, 1) = 5.0f;
    t.at(1, 2) = 6.0f;

    Tensor y = t.softmax();

    REQUIRE(y.rows() == 2);
    CHECK(y.cols() == 3);

    const float row0 = y.at(0, 0) + y.at(0, 1) + y.at(0, 2);
    const float row1 = y.at(1, 0) + y.at(1, 1) + y.at(1, 2);

    CHECK(row0 > 0.9999f);
    CHECK(row0 < 1.0001f);
    CHECK(row1 > 0.9999f);
    CHECK(row1 < 1.0001f);
}
```

## 4. CMake 里加目标

在 `CMakeLists.txt` 末尾加上：

```cmake
add_executable(test_tensor_catch2
    tests/test_tensor_catch2.cpp
)
target_include_directories(test_tensor_catch2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2
)
target_link_libraries(test_tensor_catch2 PRIVATE mini_infer)
add_test(NAME test_tensor_catch2 COMMAND $<TARGET_FILE:test_tensor_catch2>)
```

## 5. 构建和运行

如果 CMake 在你的终端正常：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

应该能看到 `test_tensor_catch2` 和原来的测试一起通过。

如果 CMake 构建暂时还有环境问题，可以先手工编译：

```powershell
New-Item -ItemType Directory -Force -Path build\tmp | Out-Null
$env:TEMP = "$PWD\build\tmp"
$env:TMP = "$env:TEMP"

g++ -std=c++17 -Wall -Wextra -Iinclude -Ithird_party/catch2 tests\test_tensor_catch2.cpp src\tensor.cpp -o build\test_tensor_catch2.exe
.\build\test_tensor_catch2.exe
```

## 6. 完成后

先把这 5 个用例写进 `tests/test_tensor_catch2.cpp`，编译通过后提交：

```powershell
git add third_party/catch2/catch.hpp tests/test_tensor_catch2.cpp CMakeLists.txt
git commit -m "test(m2): migrate first tensor tests to Catch2"
```

之后再慢慢扩展到 30 条以上。