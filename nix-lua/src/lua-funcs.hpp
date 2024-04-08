#ifndef LUA_NIX_FUNCS
#define LUA_NIX_FUNCS

#include <sol/sol.hpp>
#include <nix/eval.hh>

namespace LuaFuncs_Nix {
    nix::PrimOp* build_function(ulong argNum, sol::function func);
}
#endif