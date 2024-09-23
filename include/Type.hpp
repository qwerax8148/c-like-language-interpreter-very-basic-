#pragma once

#include <string>
#include <unordered_map>

enum BuiltinTypes{
    INT,
    FLOAT,
    DOUBLE,
    LONG,
    VOID,
    LONG_LONG,
    LONG_DOUBLE,
    SHORT,
    BOOL,
    CHAR
};

enum Type_Qualifier{
    CONST,
    EMPTY_QUALIFIER
};

const std::unordered_map<std::string, BuiltinTypes> Types_Dict = {
    {"int", INT},
    {"float", FLOAT},
    {"double", DOUBLE},
    {"long", LONG},
    {"void", VOID},
    {"long long", LONG_LONG},
    {"long double", LONG_DOUBLE},
    {"short", SHORT},
    {"bool", BOOL},
    {"char", CHAR}
};

struct BuiltinType{
    BuiltinType() = default;
    BuiltinType(std::string name) : name(name), type(Types_Dict.at(name)) {}

    std::string name;
    BuiltinTypes type;
};
