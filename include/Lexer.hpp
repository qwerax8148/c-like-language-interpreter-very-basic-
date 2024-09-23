#pragma once

#include <iostream>
#include <unordered_set>
#include <stdexcept>

#include "Token.hpp"


const std::unordered_set<std::string> Keywords = {"return", "while", "for", "do", "else", "if", "continue", "break"};

const std::unordered_set<std::string> Types = {"int", "void", "long", "double", "long long", "long double", "float", "short", "bool", "char"};

const std::unordered_set<std::string> Type_Qualifs = {"const"};

const std::unordered_set<std::string> Jumps = {"return", "continue", "break"};

const std::unordered_set<std::string> Loops = {"for", "while", "do"};

const std::unordered_set<std::string> Conditionals = {"if", "else"};

const std::unordered_set<char> Operators = {'+', '-', '/', '*', ';', ',', '=', '.', '>', '<', '&', '|', '!', '%'};

class Lexer{
public:
    Lexer() = default;
    Lexer(const char*);

    void tokenize();
    
    void print();

    void extract_NumberLiteral();
    void extract_StringLiteral();
    void extract_CharLiteral();
    void extract_Identifier();
    void extract_Operator();

    bool IsWhiteSpace(char); 
    bool IsAlpha(char);
    bool IsNumeric(char);
    bool IsOperator(char);
    bool IsOpenBraces(char);   
    bool IsCloseBraces(char); 

    
    FILE* program;
    std::vector<CppToken> tokens;
};