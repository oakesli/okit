#include <unordered_map>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "cmdline.h"

namespace fs = std::filesystem;
typedef std::unordered_map<std::string, std::string> Map;
typedef std::pair<std::string, std::string> Result;
typedef std::vector<Result> Results;

Map load(const std::string& map_file) {
    std::ifstream in(map_file);
    if (! in) {
        throw std::runtime_error(map_file + ": failed to open file.");
    }
    Map map;
    std::string line;
    while (std::getline(in, line)) {
        std::size_t pos = line.find(',');
        if (pos == std::string::npos) {
            throw std::runtime_error(map_file + ": invalid map file, should be a two-column, comma-separated text file.");
        }
        map.emplace(line.substr(0, pos), line.substr(pos+1));
    }
    in.close();
    return map;
}

Results classify(const std::string& dir, const Map& map) {
    Results results;
    for (const auto& p: fs::recursive_directory_iterator(dir)) {
        if (std::filesystem::is_regular_file(p)) {
            std::string category = p.path().parent_path().filename().string();
            if (auto it = map.find(category); it != map.end()) {
                category = it->second;
            }
            results.emplace_back(p.path().string(), std::move(category));
        }
    }
    return results;
}

void save(const Results& results, const std::string& delimiter, std::ostream& out) {
    for (const auto& result : results) {
        out << result.first << delimiter << result.second << '\n';
    }
}

int main(int argc, char** argv) {
    cmdline::parser parser;
    parser.add<std::string>("indir", 'i', "input directory", false, ".");
    parser.add<std::string>("delimiter", 'd', "delimiter", false, ",");
    parser.add<std::string>("map-file", 'm', "A two-column, comma-separated text file where the first column is the original folder name and the second column is the mapped category name.", false);
    parser.add<std::string>("outfile", 'o', "output file", false);
    parser.parse_check(argc, argv);

    std::string indir = parser.get<std::string>("indir");
    std::string delimiter = parser.get<std::string>("delimiter");
    std::string map_file = parser.get<std::string>("map-file");
    std::string outfile = parser.get<std::string>("outfile");
    try {
        if (! fs::exists(indir)) {
            throw std::runtime_error(indir + ": no such directory.");
        }
        // load
        Map map;
        if (! map_file.empty()) {
            map = load(map_file);
        }
        // classify
        Results results = classify(fs::canonical(indir).string(), map);
        // save
        if (! outfile.empty()) {
            std::ofstream out(outfile);
            if (! out) {
                throw std::runtime_error(outfile + ": failed to open file.");
            }
            save(results, delimiter, out);
            out.close();
        } else {
            save(results, delimiter, std::cout);
        }
    } catch (const std::runtime_error& exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }
    return 0;
}
