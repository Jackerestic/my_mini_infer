#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream fin("data/mlp.txt");
    if (!fin) {
        std::cout << "open failed: data/mlp.txt\n";
        return 1;
    }

    std::string line;
    while (std::getline(fin, line)) {
        std::cout << line << '\n';
    }

    return 0;
}