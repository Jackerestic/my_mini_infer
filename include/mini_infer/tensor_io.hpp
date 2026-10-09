#pragma once

#include "mini_infer/tensor.hpp"

#include <map>
#include <string>

namespace mini_infer {

// 读取形如:
//   name rows cols
//   后续 rows 行，每行 cols 个 float
// 的文本文件，把每个矩阵读成 Tensor。
bool load_tensors(const std::string& path,
                  std::map<std::string, Tensor>& out);

}  // namespace mini_infer