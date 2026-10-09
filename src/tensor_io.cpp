#include "mini_infer/tensor_io.hpp"

#include <fstream>
#include <sstream>
#include <utility>

namespace mini_infer {

bool load_tensors(const std::string& path,
                  std::map<std::string, Tensor>& out) {
    std::ifstream fin(path);
    if (!fin) {
        return false;
    }

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream header(line);
        std::string name;
        std::size_t rows = 0;
        std::size_t cols = 0;
        if (!(header >> name >> rows >> cols)) {
            return false;
        }

        Tensor t(rows, cols);

        for (std::size_t r = 0; r < rows; ++r) {
            if (!std::getline(fin, line)) {
                return false;
            }

            std::istringstream row_stream(line);
            for (std::size_t c = 0; c < cols; ++c) {
                float value = 0.0f;
                if (!(row_stream >> value)) {
                    return false;
                }
                t.at(r, c) = value;
            }
        }

        out.emplace(name, std::move(t));
    }

    return true;
}

}  // namespace mini_infer