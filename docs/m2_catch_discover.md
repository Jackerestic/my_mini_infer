# M2：让 CTest 逐个报告 Catch2 用例

## 1. 现在的问题

现在 `ctest` 只显示：

```text
test_tensor_catch2
```

虽然 Catch2 里面有 8 个 TEST_CASE，但 CTest 只看到 1 个测试程序。

M2 要求 ctest 能逐个报告测试用例，所以要用 Catch2 官方的 CMake 脚本：

- `Catch.cmake`
- `CatchAddTests.cmake`

## 2. 下载两个 CMake 脚本

放到：

```text
third_party/catch2/
├─ catch.hpp
├─ Catch.cmake
└─ CatchAddTests.cmake
```

下载命令：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer

Invoke-WebRequest `
  -Uri "https://ghproxy.net/https://raw.githubusercontent.com/catchorg/Catch2/v2.13.10/contrib/Catch.cmake" `
  -OutFile "third_party\catch2\Catch.cmake"

Invoke-WebRequest `
  -Uri "https://ghproxy.net/https://raw.githubusercontent.com/catchorg/Catch2/v2.13.10/contrib/CatchAddTests.cmake" `
  -OutFile "third_party\catch2\CatchAddTests.cmake"
```

如果镜像不行，就把 `ghproxy.net/` 去掉，直接换官方地址。

## 3. 修改 CMakeLists.txt

在 `enable_testing()` 之前或之后加：

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/third_party/catch2")
include(Catch)
```

然后把：

```cmake
add_test(NAME test_tensor_catch2 COMMAND $<TARGET_FILE:test_tensor_catch2>)
```

替换成：

```cmake
catch_discover_tests(test_tensor_catch2)
```

注意：`catch_discover_tests` 必须写在 `add_executable(test_tensor_catch2 ...)` 之后。

## 4. 重新配置和构建

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## 5. 查看 CTest 是否逐个列出

```powershell
ctest --test-dir build -N
```

期望输出类似：

```text
Test #1: Tensor basic
Test #2: add two tensors
Test #3: matmul two tensors
Test #4: relu three situation
...
Test #8: test_tensor_catch2:softmax
```

最后跑：

```powershell
ctest --test-dir build --output-on-failure
```

如果每个 TEST_CASE 都变成单独的 CTest 用例，这一步就完成了。