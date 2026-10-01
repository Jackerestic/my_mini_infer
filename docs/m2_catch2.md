# M2 入门：Catch2 是什么

## 1. 一句话

Catch2 是一个 C++ 测试框架。

你写测试时不用自己维护失败计数器、不用手写 CHECK 宏，直接用 Catch2 提供的宏：

- `TEST_CASE`
- `REQUIRE`
- `CHECK`
- `SECTION`

它会自动统计哪些检查通过、哪些失败、失败在哪一行。

## 2. 和现在的手写测试有什么区别

现在 `tests/test_tensor.cpp` 里是：

```cpp
int g_failures = 0;

#define CHECK(cond) ...
```

这能跑，也能发现错误，但有几个缺点：

- 每条检查只是打印 FAIL，没有统一格式；
- 测试之间没有名字和分组；
- 失败时不能方便地只跑某一个用例；
- 断言数量一多，输出会很难看；
- CTest 只看到整个程序一个退出码，不能逐个报告用例。

Catch2 把这些东西都做好了。

## 3. 最小例子

```cpp
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

int add(int a, int b) {
    return a + b;
}

TEST_CASE("add works") {
    REQUIRE(add(1, 2) == 3);
    CHECK(add(0, 0) == 0);
    CHECK(add(-1, 1) == 0);
}
```

- `TEST_CASE` 定义一个测试用例；
- `REQUIRE` 失败就停止当前用例；
- `CHECK` 失败会继续执行后面的检查；
- `CATCH_CONFIG_MAIN` 会自动生成 main 函数。

## 4. 为什么 M2 要换成 Catch2

M2 的验收标准里有：

```text
ctest 输出里 >= 30 个独立断言用例
```

Catch2 可以让：

- 每个 `TEST_CASE` 有名字；
- 每条 `REQUIRE` / `CHECK` 单独报告；
- 失败时显示文件和行号；
- CTest 可以按用例逐个运行和报告。

这样才更像工业级项目。

## 5. Catch2 版本选择

零基础建议用 Catch2 v2 的单头文件版：

```text
catch.hpp
```

只需要一个头文件，放进项目里就能用，不需要额外安装库。

Catch2 v3 更现代，但拆成了库，配置步骤更多。现在先用 v2 单头文件版。

## 6. 在 CMake 里的接法

大概形式：

```cmake
add_executable(test_tensor_catch2
    tests/test_tensor_catch2.cpp
    src/tensor.cpp
)
target_include_directories(test_tensor_catch2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2
)
add_test(NAME test_tensor_catch2 COMMAND $<TARGET_FILE:test_tensor_catch2>)
```

后面可以再用 CMake 的 `catch_discover_tests` 做到每个 TEST_CASE 单独成为一个 CTest 用例。

## 7. 下一步

先不迁移全部测试。

第一步只做：

1. 拿到 `catch.hpp`；
2. 新建 `tests/test_tensor_catch2.cpp`；
3. 迁移 5 条最关键断言；
4. 编译并运行；
5. 确认 CTest 能看到这个测试。