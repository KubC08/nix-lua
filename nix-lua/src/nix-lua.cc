#include <nix/primops.hh>
#include <nix/eval-inline.hh>

#include <sol/sol.hpp>

#include "lua-vm.hpp"

namespace nix {
    static void prim_fromLUA(EvalState& state, const PosIdx pos, Value** args, Value& val) {
        auto luaScript = state.forceStringNoCtx(*args[0], pos, "while evaluating the argument passed to builtins.fromLUA");

        sol::state lua;
        sol::table script_table = luaVM::execute_and_get(lua, luaScript);

        std::function<void(Value&, sol::object)> handleTable = [&](Value& nixValue, sol::object luaValue) {
            switch(luaValue.get_type()) {
                case sol::type::nil:
                    nixValue.mkNull();
                    break;
                case sol::type::string:
                    nixValue.mkString(luaValue.as<std::string>());
                    break;
                case sol::type::boolean:
                    nixValue.mkBool(luaValue.as<bool>());
                    break;
                case sol::type::number:
                    if (luaValue.is<NixFloat>()) {
                        nixValue.mkFloat(luaValue.as<NixFloat>());
                    } else if (luaValue.is<int64_t>()) {
                        nixValue.mkInt(luaValue.as<int64_t>());
                    }
                    break;
                case sol::type::table:
                    sol::table tableValue = luaValue.as<sol::table>();
                    size_t tableSize = tableValue.size();

                    if (tableSize > 0) { // Most likely an array
                        state.mkList(nixValue, tableSize);
                        tableValue.for_each([&](sol::object key, sol::object value) {
                            if (!key.is<int64_t>()) return;
                            int64_t index = key.as<int64_t>();

                            handleTable(*(nixValue.listElems()[index] = state.allocValue()), value);
                        });
                    } else { // Most likely an object
                        size_t objectSize = 0;
                        tableValue.for_each([&](sol::object, sol::object) {
                            objectSize++;
                        });

                        BindingsBuilder attrs = state.buildBindings(objectSize);
                        tableValue.for_each([&](sol::object key, sol::object value) {
                            if (!key.is<std::string>()) return;

                            handleTable(attrs.alloc(key.as<std::string>()), value);
                        });

                        nixValue.mkAttrs(attrs);
                    }
                    break;
            }
        };

        handleTable(val, script_table);
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