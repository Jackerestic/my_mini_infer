# M4 总体计划：最小可发布推理引擎

## 1. M4 的目标

M4 不是加更多算子，而是把现在这个项目变成一个别人能看懂、能跑起来、能展示的推理引擎。

计划定的验收标准：

- README 能让陌生人 30 分钟内跑通；
- 支持批处理推理；
- 有一个最小 HTTP 推理服务；
- 仓库公开且干净；
- 最终版本有清楚的架构 / 使用示例 / 局限说明。

## 2. M4 要交付什么

### 交付物 1：正式模型接口

不要一直写临时代码。要有一个类：

```cpp
class Mlp {
public:
    Mlp(Tensor W1, Tensor b1, Tensor W2, Tensor b2);
    Tensor forward(const Tensor& x) const;
};
```

输入：
- x 形状是 (N, in_features)

输出：
- 形状是 (N, classes)
- 每行是 softmax 后的概率

### 交付物 2：批处理推理

一次输入多条样本，一次算出多行结果。

例如：

```text
输入 2 条样本：
[[1, 2],
 [3, 4]]

输出 2 行概率：
[[...],
 [... ]]
```

### 交付物 3：最小 HTTP 服务

用 C++ 起一个本地服务：

- GET /health：返回服务正常；
- POST /predict：输入一批样本，返回概率。

目标不是生产级服务，而是证明：

- 模型能被服务化调用；
- 批处理能通过 HTTP 触发；
- 有一个可演示的 AI Infra 最小闭环。

### 交付物 4：README 重写

README 至少要有：

- 这个项目解决什么问题；
- 架构图；
- 快速开始；
- 如何跑测试；
- 如何起 HTTP 服务；
- 如何发一个预测请求；
- 当前局限。

## 3. M4 不建议做的事

先不要做：

- 多线程服务；
- 动态批处理；
- 鉴权；
- 数据库；
- Docker/K8s；
- CUDA；
- 量化。

这些都属于以后再说的选项，不是本学期 M4 的最小版本。

## 4. M4 阶段顺序

1. 建立 Mlp 类和批处理推理接口；
2. 用 Catch2 给 Mlp::forward 补测试；
3. 把模型加载逻辑做成可复用模块；
4. 引入单头文件 HTTP 库（推荐 cpp-httplib）；
5. 实现 /health 和 /predict；
6. 写一个最小客户端示例；
7. 重写 README；
8. 更新 CI，保证服务也能编译；
9. 最后做一次陌生人 30 分钟跑通验收。

## 5. M4 第一小步

只做模型接口：

- include/mini_infer/mlp.hpp
- src/mlp.cpp
- examples/batch_infer.cpp
- Catch2 测试：tests/test_mlp.cpp

验收：
- 输入 N 条样本，输出 N 行概率；
- 每行概率和约等于 1；
- 用 data/mlp.txt 的权重跑出的结果和 M1 验证一致。