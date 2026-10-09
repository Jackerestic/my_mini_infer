# M4 第三步：最小 HTTP 推理服务

> 这一步由你实现，我只负责给要求、接口和验收标准。
> 不要一次性加很多功能，先做最小版本。

## 1. 目标

用 C++ 起一个本地 HTTP 服务：

- `GET /health`：返回服务正常；
- `POST /predict`：接收一批样本，调用 `Mlp::forward`，返回概率。

当前阶段不要求：

- 多线程；
- 鉴权；
- JSON 解析库；
- 动态批处理；
- 高并发。

## 2. 依赖：cpp-httplib

使用单头文件 `cpp-httplib`。

放到：

```text
third_party/httplib/httplib.h
```

下载命令在你自己终端执行：

```powershell
cd C:\Users\文涵超\Desktop\实习日志\my_mini_infer
New-Item -ItemType Directory -Force -Path third_party\httplib | Out-Null

Invoke-WebRequest `
  -Uri "https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.18.3/httplib.h" `
  -OutFile "third_party\httplib\httplib.h"
```

如果官方地址慢，可以用镜像，例如：

```powershell
Invoke-WebRequest `
  -Uri "https://ghproxy.net/https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.18.3/httplib.h" `
  -OutFile "third_party\httplib\httplib.h"
```

确认文件存在且大小合理：

```powershell
Get-Item third_party\httplib\httplib.h | Select-Object FullName,Length
```

## 3. 要新建的文件

```text
examples/http_server.cpp
```

启动时：

1. 用 `mini_infer::load_tensors("data/mlp.txt", inputs)` 读取权重；
2. 用 `inputs.at("W1")`、`b1`、`W2`、`b2` 构造 `Mlp`；
3. 启动 HTTP 服务，监听 `127.0.0.1:8080`。

## 4. 协议约定

### GET /health

返回：

```text
ok
```

### POST /predict

请求体使用最简单的文本格式：

```text
每行一个样本，特征之间用空格分隔
```

例如请求两条 4 维样本：

```text
1 2 3 4
5 6 7 8
```

服务端：

1. 逐行读取请求体；
2. 每行解析成若干 float；
3. 拼成一个 `(N, 4)` 的 `Tensor`；
4. 调用 `model.forward(x)`；
5. 返回一个文本结果：每行一个样本，概率用空格分隔。

例如：

```text
0.1 0.2 0.7
0.3 0.3 0.4
```

如果请求为空、行宽不一致、或者维度不匹配，返回 400。

## 5. 需要自己实现的步骤

- [ ] 下载 `httplib.h`
- [ ] 写 `examples/http_server.cpp`
- [ ] `GET /health` 能返回 ok
- [ ] `POST /predict` 能解析文本
- [ ] 能构造 `Tensor` 并调用 `Mlp::forward`
- [ ] 能格式化输出概率
- [ ] 能处理错误请求并返回 400

## 6. CMake 目标

在 `CMakeLists.txt` 末尾加：

```cmake
add_executable(infer_server
    examples/http_server.cpp
)
target_include_directories(infer_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/httplib
)
target_link_libraries(infer_server PRIVATE mini_infer)

if (WIN32)
    target_link_libraries(infer_server PRIVATE ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(infer_server PRIVATE Threads::Threads)
endif()
```

## 7. 验收命令

启动服务：

```powershell
.\build\infer_server.exe
```

另开一个 PowerShell：

```powershell
curl.exe http://127.0.0.1:8080/health
```

再请求预测：

```powershell
$body = "0.30 -1.04 0.75 0.94`n-1.95 -1.30 0.13 -0.32"
curl.exe -X POST -H "Content-Type: text/plain" --data-binary $body http://127.0.0.1:8080/predict
```

应该输出两行概率，每行加起来约等于 1。

自己检查：

- 服务能起来；
- health 返回 ok；
- predict 返回两行概率；
- 错误请求返回 400。

## 8. 完成后的提交

```powershell
git add third_party/httplib/httplib.h examples/http_server.cpp CMakeLists.txt
git commit -m "feat(m4): add minimal HTTP inference server"
```

完成后把代码和 curl 输出发我，我只做 review 和修 bug，不直接替你实现这一步。