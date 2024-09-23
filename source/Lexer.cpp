#include "Lexer.hpp"

void Lexer::print(){
    for(auto el : tokens){
        std::cout << "Token Type: " << TokenType_Dict.at(el.type) << ", Name = \"" << el.token << '\"' << std::endl; 
    }
}

Lexer::Lexer(const char* file_name){
    program = fopen(file_name, "r");
}

bool Lexer::IsWhiteSpace(char c){
    return c == ' ' || c == '\n' || c == '\t';
}

bool Lexer::IsAlpha(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::IsNumeric(char c){
    return c >= '0' && c <= '9';
}

bool Lexer::IsOperator(char c){
    return Operators.contains(c);
}

bool Lexer::IsOpenBraces(char c){
    return c == '(' || c == '{' || c == '[';
}

bool Lexer::IsCloseBraces(char c){
    return c == ')' || c == '}' || c == ']';
}

void Lexer::tokenize(){
    char c = fgetc(program);

    while(!feof(program)){
        if(IsWhiteSpace(c)){
            c = fgetc(program);
            continue;
        }

        if(IsNumeric(c)){
            ungetc(c, program);
            extract_NumberLiteral();
            c = fgetc(program);
            continue;
        }

        if(c == '\"'){
            extract_StringLiteral();
            c = fgetc(program);
            continue;
        }

        if(c == '\''){
            extract_CharLiteral();
            c = fgetc(program);
            continue;
        }

        if(IsAlpha(c) || c == '_'){
            ungetc(c, program);
            extract_Identifier();
            c = fgetc(program);
            continue;
        }

        if(IsOpenBraces(c)){
            std::string s; s += c;
            tokens.push_back(CppToken(s, OpenBraces));
            c = fgetc(program);
            continue;
        }

        if(IsCloseBraces(c)){
            std::string s; s += c;
            tokens.push_back(CppToken(s, CloseBraces));
            c = fgetc(program);
            continue;
        }

        if(IsOperator(c)){
            // std::string s; s += c;
            // tokens.push_back(CppToken(s, Operator));
            // c = fgetc(program);
            // continue;
            ungetc(c, program);
            extract_Operator();
            c = fgetc(program);
            continue;
        }

        throw std::runtime_error("Unexpected Token"); 
    }
}

void Lexer::extract_NumberLiteral(){
    char c = fgetc(program);
    std::string number;
    while(!feof(program) && IsNumeric(c)){
        number += c;
        c = fgetc(program);
    }
    if(!feof(program)){
        if(c == '.'){
            number += c;
            if(!feof(program)) 
                c = fgetc(program);
            while(!feof(program) && IsNumeric(c)){
                number += c;
                c = fgetc(program);
            }
            CppToken new_token(number, Number);
            tokens.push_back(CppToken(number, Number));
            if(!feof(program)) ungetc(c, program);
        }else{
            ungetc(c, program);
            tokens.push_back(CppToken(number, Number));
        }
    }
}

void Lexer::extract_CharLiteral(){
    if(!feof(program)){
        char c = fgetc(program);
        std::string s; s += c;
        CppToken new_token(s, CharLiteral);
        tokens.push_back(new_token);
        if(!feof(program)){
            c = fgetc(program);
            if(c != '\''){
                throw std::runtime_error("expected \"\'\" for char literal");
            }
        }
    }else{
        throw std::runtime_error("expected \"\'\" for char literal");
    }
}

void Lexer::extract_StringLiteral(){
    char c = fgetc(program);
    std::string s;
    int f = 0;

    while(!feof(program)){
        s += c;
        c = fgetc(program);
        if(c == '\"'){ f = 1; break; }
    }

    if(!f){
        throw std::runtime_error("Expected \" for string literal object");
    }else{
        CppToken new_token(s, StringLiteral);
        tokens.push_back(new_token);
    }
}

void Lexer::extract_Identifier(){
    char c = fgetc(program);
    std::string s;
    while(!feof(program) && (IsAlpha(c) || IsNumeric(c) || c == '_')){
        s += c;
        c = fgetc(program);
    }

    if(!feof(program)) ungetc(c, program);

    if(Keywords.contains(s)){
        CppToken new_token(s, Keyword);
        tokens.push_back(new_token);
    }else{
        if(Types.contains(s)){
            CppToken new_token(s, Type);
            tokens.push_back(new_token);
        }else{
            if(Type_Qualifs.contains(s)){
                CppToken new_token(s, Type_Qual);
                tokens.push_back(new_token);
            }else{
                CppToken new_token(s, Identifier);
                tokens.push_back(new_token);
            }
        }
    }
}

void Lexer::extract_Operator(){
    std::string s;
    char c = fgetc(program);
    s += c;

    if(c == '/'){
        if(feof(program)){
            tokens.push_back(CppToken(s, Operator));
            return;
        }

        char ch = fgetc(program);
        if(ch == '/'){
            while(!feof(program) && ch != '\n'){
                ch = fgetc(program);
            }
            return;
        }

        if(ch == '*'){
            if(feof(program))
                return;
            
            ch = fgetc(program);

            while(!feof(program)){
                if(ch == '*'){
                    ch = fgetc(program);
                    if(ch == '/')
                        return;
                    else continue;
                }
                ch = fgetc(program);
            }
            return;
        }
        if(IsOperator(ch)){
            s += ch;
            tokens.push_back(CppToken(s, Operator));
            return;
        }else{
            ungetc(ch, program);
            tokens.push_back(CppToken(s, Operator));
            return;
        }
    }

    if(feof(program)){
        tokens.push_back(CppToken(s, Operator));
        return;
    }
    c = fgetc(program);
    if(IsOperator(c)){
        s += c;
        tokens.push_back(CppToken(s, Operator));
        return;
    }
    tokens.push_back(CppToken(s, Operator));
    ungetc(c, program);
}
