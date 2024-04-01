#include "utils.hpp"

#include <span>
#include <random>

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

    void lua_object_to_nix(nix::EvalState& state, nix::Value& nixValue, sol::object& luaValue) {
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
                if (luaValue.is<nix::NixFloat>()) {
                    nixValue.mkFloat(luaValue.as<nix::NixFloat>());
                } else if (luaValue.is<int64_t>()) {
                    nixValue.mkInt(luaValue.as<int64_t>());
                } else {
                    nixValue.mkNull();
                }
                break;
            case sol::type::table:
                { // Because "transfer of control" problems
                    sol::table tableValue = luaValue.as<sol::table>();
                    size_t tableSize = tableValue.size();

                    if (tableSize > 0) { // Most likely an array
                        state.mkList(nixValue, tableSize);
                        tableValue.for_each([&](sol::object key, sol::object value) {
                            if (!key.is<int64_t>()) return;
                            int64_t index = key.as<int64_t>();

                            lua_object_to_nix(state, *(nixValue.listElems()[index] = state.allocValue()), value);
                        });
                    } else { // Most likely an object
                        size_t objectSize = 0;
                        tableValue.for_each([&](sol::object, sol::object) {
                            objectSize++;
                        });

                        nix::BindingsBuilder attrs = state.buildBindings(objectSize);
                        tableValue.for_each([&](sol::object key, sol::object value) {
                            if (!key.is<std::string>()) return;

                            lua_object_to_nix(state, attrs.alloc(key.as<std::string>()), value);
                        });

                        nixValue.mkAttrs(attrs);
                    }
                }
                break;
            case sol::type::userdata:
                if (luaValue.is<nix::PrimOp*>()) {
                    nixValue.mkPrimOp(luaValue.as<nix::PrimOp*>());
                } else {
                    nixValue.mkNull();
                }
                break;
            default:
                nixValue.mkNull();
                break;
        }
    }

    sol::object nix_object_to_lua(nix::EvalState& state, sol::state& lua, nix::Value& val) {
        switch(val.type()) {
            case nix::nInt:
                return sol::make_object(lua, val.integer);
            case nix::nBool:
                return sol::make_object(lua, val.boolean);
            case nix::nString:
                return sol::make_object(lua, val.str());
            case nix::nPath:
                return sol::make_object(lua, val.path().to_string());
            case nix::nNull:
                return sol::nil;
            case nix::nAttrs:
                {
                    nix::Bindings* nixAttrs = val.attrs;
                    if (nixAttrs == nullptr) break;

                    size_t attrsSize = nixAttrs->size();
                    sol::table table = lua.create_table(0, attrsSize);
                    for (size_t i = 0; i < attrsSize; i++) {
                        nix::Attr attr = (*nixAttrs)[i];
                        std::string key = state.symbols[attr.name];

                        if (attr.value == nullptr) table[key] = sol::nil;
                        else table[key] = nix_object_to_lua(state, lua, *attr.value);
                    }
                    return table;
                }
                break;
            case nix::nList:
                {
                    nix::Value** nixListItems = val.listElems();
                    const size_t nixListSize = val.listSize();
                    sol::table items = lua.create_table(nixListSize);

                    for (size_t i = 0; i < nixListSize; i++) {
                        if (nixListItems[i] != nullptr)
                            items.add(nix_object_to_lua(state, lua, *nixListItems[i]));
                    }
                    return items;
                }
                break;
            case nix::nFunction:
                break; // TODO: Add function cross-compatibility
            case nix::nFloat:
                return sol::make_object(lua, val.fpoint);
        }
        return sol::nil;
    }

    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}