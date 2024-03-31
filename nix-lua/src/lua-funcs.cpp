#include "lua-funcs.hpp"
#include "utils.hpp"

#include <boost/uuid/uuid_generators.hpp>

namespace LuaFuncs_Nix {

    static std::map<std::string, sol::function&> builtin_funcs = {};
    nix::PrimOp* build_function(ulong argNum, sol::function& func) {
        std::string name = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
        builtin_funcs[name] = func;

        return new nix::PrimOp {
            .name = "__" + name,
            .arity = argNum,
            .fun = [](nix::EvalState& state, const nix::PosIdx pos, nix::Value** args, nix::Value& val) {
                if (val.primOp == nullptr) {
                    throw "PrimOp does not exist?";
                }
                ulong argNum = val.primOp->arity;
                std::string name = val.primOp->name;
                sol::function& func = builtin_funcs[name];

                std::vector<std::any> luaArgs;
                for (ulong i = 0; i < argNum; i++) {
                    state.forceValue(*args[i], pos);
                    luaArgs.push_back(nix_utils::nix_object_to_lua(*args[i]));
                }

                sol::object result = func.call(luaArgs);
                nix_utils::lua_object_to_nix(state, val, result);
            }
        };
    }
}