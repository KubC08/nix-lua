#ifndef LUA_NIX_UTILS
#define LUA_NIX_UTILS

#include <vector>
#include <string_view>
#include <string>

namespace nix_utils {
    std::vector<std::string> split_string(std::string_view target, char seperator);
}
#endif