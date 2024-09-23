#pragma once

#include <string>
#include <vector>
#include <unordered_map>

enum CppTokenType{
    Number,
    StringLiteral, // "asd"
    CharLiteral, // 'a'
    Identifier, // x
    Keyword, // int
    Operator, // +
    OpenBraces, // (
    CloseBraces, // )
    Type, // int
    Type_Qual // const 
};


const std::unordered_map<CppTokenType, std::string> TokenType_Dict = {
    {Number, "Number"},
    {StringLiteral, "String Literal"},
    {CharLiteral, "Char Literal"},
    {Identifier, "Identifier"},
    {Keyword, "Keyword"},
    {Operator, "Operator"},
    {OpenBraces, "Open Braces"},
    {CloseBraces, "Close Braces"},
    {Type, "Type"}
};

struct CppToken{
    std::string token;
    CppTokenType type;

    CppToken() = default;
    CppToken(const std::string& s, CppTokenType t) : token(s), type(t) {}

    bool operator==(const CppToken& other) const{
        return token == other.token && type == other.type;
    }

    bool operator!=(const CppToken& other) const{
        return !(*this == other);
    }

    operator std::string() const{
        return toString();
    }

    std::string toString() const{
        return token;
    }
};