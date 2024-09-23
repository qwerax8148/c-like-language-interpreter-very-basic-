#include <iostream>
#include "Parser.hpp"

int main(int argc, char** argv){
    if(argc == 1){
        std::cout << "need file name\n";
        return 0;
    }
    Lexer lexer(argv[1]);
    lexer.tokenize();

    Parser parser;
    parser.parse(lexer.tokens);

    Analyzer analyzer(parser.Global_Scope);
    analyzer.analyze();

    Executer executer(parser.Global_Scope);
    executer.execute();
}   