#include "utils.hpp"

namespace nix_utils {
    std::vector<std::string> split_string(std::string_view target, char seperator) {
        std::vector<std::string> result;

        size_t startIndex = 0;
        size_t targetSize = target.size();
        for (size_t i = 0; i <= targetSize; i++) {
            if (target[i] != seperator && i != targetSize) continue;

            std::string str;
            str.append(target, startIndex, i - startIndex);
            result.push_back(str);
            startIndex = i + 1;
        }
        return result;
    }
}