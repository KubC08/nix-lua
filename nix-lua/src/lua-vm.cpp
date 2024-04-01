#include "lua-vm.hpp"
#include "utils.hpp"

#include "lua-funcs.hpp"

namespace luaVM {
    void load_lua(sol::state& lua) {
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::utf8);
        lua["nix_function"] = LuaFuncs_Nix::build_function;
    }

    sol::object execute_and_get(sol::state& lua, std::string_view luaScript) {
        load_lua(lua);

        sol::load_result script = lua.load(luaScript);
        sol::protected_function_result script_result = script();

        return handle_result(lua, script_result);
    }

    sol::object handle_result(sol::state& lua, sol::protected_function_result& script_result) {
        if (!script_result.valid()) {
            throw "invalid formatted script";
        }
        if (script_result.return_count() == 0) {
            return sol::nil;
        }
        sol::object result = script_result[0];
        sol::type resultType = result.get_type();

        switch (resultType)
        {
        case sol::type::table:
            return fix_table(lua, result.as<sol::table>());
        case sol::type::userdata:
            return result;
        }
        return sol::nil;
    }

    sol::table fix_table(sol::state& lua, sol::table target) {
        sol::table result = lua.create_table();

        std::function<void(sol::table&, sol::object, sol::object)> runTable = [&](sol::table& parent, sol::object key, sol::object value) {
            if (!key.is<std::string>()) return;
            std::string keyString = key.as<std::string>();
            sol::table keyTable = key_to_table(lua, keyString, value);

            merge_to_table(lua, parent, keyTable);
        };
        target.for_each([&](sol::object key, sol::object value) {
            runTable(result, key, value);
        });
        return result;
    }

    sol::table key_to_table(sol::state& lua, std::string_view key, sol::object value) {
        sol::table result = lua.create_table();
        std::vector<std::string> keyParts = nix_utils::split_string(key, '.');
        size_t keyPartCount = keyParts.size();

        sol::table current = result;
        for (size_t i = 0; i < keyPartCount; i++) {
            std::string keyPart = keyParts[i];

            if (i+1 < keyPartCount) {
                current[keyPart] = lua.create_table();
                current = current[keyPart];
            } else {
                current[keyPart] = value;
            }
        }
        return result;
    }

    void merge_to_table(sol::state& lua, sol::table& target, sol::table& mergable) {
        std::function<void(sol::table&, sol::object, sol::object)> handleTable = [&](sol::table& current, sol::object key, sol::object value) {         
            sol::optional<sol::object> existingObjOpt = current[key];
            if (existingObjOpt == sol::nullopt) {
                current.set(key, value);
                return;
            }
            sol::object existingObj = existingObjOpt.value();

            if (value.is<sol::table>()) {
                if (existingObj.is<sol::table>()) {
                    value.as<sol::table>().for_each([&](sol::object deepKey, sol::object deepValue) {
                        sol::table existingTable = existingObj.as<sol::table>();
                        handleTable(existingTable, deepKey, deepValue);
                    });
                } else {
                    throw "Object mismatch";
                }
            } else if (existingObj.is<sol::table>()) {
                throw "Object mismatch";
            } else {
                current.set(key, value);
            }
        };
        mergable.for_each([&](sol::object key, sol::object value) {
            handleTable(target, key, value);
        });
    }
}