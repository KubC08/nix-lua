#include "lua-funcs.hpp"
#include "utils.hpp"

#include "lua-vm.hpp"

namespace LuaFuncs_Nix {
    static std::map<std::string, sol::bytecode> builtin_funcs = {};
    nix::PrimOp* build_function(ulong argNum, sol::function func) {
        std::string name = "__" + nix_utils::generate_uuid_v4();
        builtin_funcs[name] = func.dump();
        printf("Function count: %d\n", builtin_funcs.size());

        return new nix::PrimOp {
            .name = name,
            .arity = argNum,
            .fun = [](nix::EvalState& state, const nix::PosIdx pos, nix::Value** args, nix::Value& val) {
                if (val.primOp == nullptr) {
                    throw "PrimOp does not exist?";
                }
                ulong argNum = val.primOp->arity;
                std::string name = val.primOp->name;

                auto func_finder = builtin_funcs.find(name);
                if (func_finder == builtin_funcs.end()) {
                    throw "Function not found";
                }
                sol::bytecode func = func_finder->second;

                sol::state lua;
                luaVM::load_lua(lua);

                sol::table luaArgs = lua.create_table(argNum);
                for (ulong i = 0; i < argNum; i++) {
                    state.forceValue(*args[i], pos);
                    luaArgs.add(nix_utils::nix_object_to_lua(state, lua, *args[i]));
                }

                sol::load_result func_script = lua.load(func.as_string_view());
                sol::protected_function_result func_result = func_script(luaArgs);
                sol::object result = luaVM::handle_result(lua, func_result);

                nix_utils::lua_object_to_nix(state, val, result);
            }
        };
    }
}