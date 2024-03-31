#ifndef LUA_NIX_UTILS
#define LUA_NIX_UTILS

#include <vector>
#include <string_view>
#include <string>

#include <sol/sol.hpp>
#include <nix/primops.hh>

namespace nix_utils {
    std::vector<std::string> split_string(std::string_view target, char seperator);

    void lua_object_to_nix(nix::EvalState& state, nix::Value& nixValue, sol::object& luaValue);
    std::any nix_object_to_lua(nix::Value& val);

    std::string generate_uuid_v4();
}
#endif