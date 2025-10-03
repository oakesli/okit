#include <fstream>
#include <filesystem>
#include <iostream>

void usage() {
    std::cerr << "Usage: tofa file1 [file2 ...]" << std::endl;
}

void tofa(const std::string& file) {
    std::filesystem::path path(file);
    if (! std::filesystem::exists(path)) {
        throw std::runtime_error(file + ": no such file");
    }
    if (! std::filesystem::is_regular_file(path)) {
        throw std::runtime_error(file + ": not a regular file");
    }
    std::ifstream in(file);
    if (! in) {
        throw std::runtime_error(file + ": failed to open file for reading");
    }
    std::ofstream out(file + ".fasta");
    if (! out) {
        throw std::runtime_error(file + ": failed to open file for writing");
    }
    out << ">" << path.stem().string() << "\n";
    out << in.rdbuf();
    in.close();
    out.close();
}

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            usage();
            return 1;
        }
        for (int i=1; i<argc; ++i) {
            tofa(argv[i]);
        }
        return 0;
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return 2;
    }
}
