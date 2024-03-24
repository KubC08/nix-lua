#ifndef LUA_NIX_VM
#define LUA_NIX_VM

#include <sol/sol.hpp>

namespace luaVM {
    sol::table execute_and_get(sol::state& lua, std::string_view luaScript);

    sol::table fix_table(sol::state& lua, sol::table& target);
    sol::table key_to_table(sol::state& lua, std::string_view key, sol::object value);
    void merge_to_table(sol::state& lua, sol::table& target, sol::table& mergable);
}
#endif