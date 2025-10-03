#include <set>
#include <filesystem>
#include <iostream>

std::set<std::string> scan(const std::string& dir) {
    std::set<std::string> extensions;
    for (const auto& p: std::filesystem::recursive_directory_iterator(dir)) {
        if (std::filesystem::is_regular_file(p)) {
            extensions.insert(p.path().extension().string());
        }
    }
    return extensions;
}

template <typename Container>
void print(const Container& container) {
    for (const auto& element: container) {
        std::cout << element << std::endl;
    }
}

int main() {
    print(scan("."));
    return 0;
}