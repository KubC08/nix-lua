#include <nix/primops.hh>
#include <nix/eval-inline.hh>

#include <sol/sol.hpp>

#include "lua-vm.hpp"
#include "utils.hpp"
#include "lua-funcs.hpp"

namespace nix {
    static void prim_fromLUA(EvalState& state, const PosIdx pos, Value** args, Value& val) {
        auto luaScript = state.forceStringNoCtx(*args[0], pos, "while evaluating the argument passed to builtins.fromLUA");

        sol::state lua;

        sol::object script_result = luaVM::execute_and_get(lua, luaScript);
        nix_utils::lua_object_to_nix(state, val, script_result);
    }

    static RegisterPrimOp primop_fromLUA({
        .name = "fromLUA",
        .args = {"e"},
        .doc = R"(
            Execute LUA provided lua script and convert the given output to a Nix value. For example,

            ```nix
            builtins.fromLUA ''
                return {
                    x = 1,
                    s = "a",
                    example = {
                        y = 2
                    }
                }
            ''
            ```
        )",
        .fun = prim_fromLUA
    });
}