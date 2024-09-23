#pragma once

#include <unordered_map>
#include "Ast.hpp"
#include <string>
#include "Scope.hpp"
#include <optional>
#include <cstdlib>
#include <cstdio>

struct Visitor{
    virtual ~Visitor() = default;
    virtual void Visit(FunctionDefinition&) = 0;
    virtual void Visit(Variable_Declaration&) = 0;

    virtual void Visit(Compound_Statement&) = 0;
    virtual void Visit(Expression_Statement&) = 0;
    virtual void Visit(Selection_Statement&) = 0;
    virtual void Visit(Iteration_Statement&) = 0;
    virtual void Visit(Break_Jump&) = 0;
    virtual void Visit(Continue_Jump&) = 0;
    virtual void Visit(Return_Jump&) = 0;
    virtual void Visit(Declaration_Statement&) = 0;

    virtual void Visit(Assignment_Expression&) = 0;
    virtual void Visit(And_Expression&) = 0;
    virtual void Visit(Or_Expression&) = 0;
    virtual void Visit(Equality_Expression&) = 0;
    virtual void Visit(Relational_Expression&) = 0;
    virtual void Visit(Additive_Expression&) = 0;
    virtual void Visit(Multiplicative_Expression&) = 0;
    virtual void Visit(Identifier_Expression&) = 0;
    virtual void Visit(Number_Expression&) = 0;
    virtual void Visit(CharLiteral_Expression&) = 0;
    virtual void Visit(StringLiteral_Expression&) = 0;
    virtual void Visit(Parenthesized_Expression&) = 0;
    virtual void Visit(Unary_Expression&) = 0;
    virtual void Visit(FunctionCall_Expression&) = 0;
};



struct Analyzer : Visitor {  
    Analyzer(std::vector<std::pair<std::optional<Variable_Declaration>, std::optional<FunctionDefinition>>> Global_Scope) : Global_Scope(Global_Scope) {}

    void analyze();

    void Visit(FunctionDefinition&) override;
    void Visit(Variable_Declaration&) override;

    void Visit(Compound_Statement&) override;
    void Visit(Expression_Statement&) override;
    void Visit(Selection_Statement&) override;
    void Visit(Iteration_Statement&) override;
    void Visit(Break_Jump&) override;
    void Visit(Continue_Jump&) override;
    void Visit(Declaration_Statement&) override;
    void Visit(Return_Jump&) override;


    void Visit(Assignment_Expression&) override;
    void Visit(Or_Expression&) override;
    void Visit(And_Expression&) override;
    void Visit(Equality_Expression&) override;
    void Visit(Relational_Expression&) override;
    void Visit(Additive_Expression&) override;
    void Visit(Multiplicative_Expression&) override;
    void Visit(Identifier_Expression&) override;
    void Visit(Number_Expression&) override;
    void Visit(CharLiteral_Expression&) override;
    void Visit(StringLiteral_Expression&) override;
    void Visit(Parenthesized_Expression&) override;
    void Visit(Unary_Expression&) override;
    void Visit(FunctionCall_Expression&) override;

    bool Is_Number_Type(BuiltinType);

    ScopeManager scope_manager;
    std::vector<std::pair<std::optional<Variable_Declaration>, std::optional<FunctionDefinition>>> Global_Scope;
    std::unordered_map<std::string, std::shared_ptr<FunctionDefinition>> function_dictionaty;

    std::string identifier = "";

    bool is_lvalue = false;
    bool is_convertable_to_number = false;
    bool is_string = false;
    BuiltinType return_type;

};



struct Expression_Printer : Visitor {
    void Visit(FunctionDefinition&) override {}
    void Visit(Variable_Declaration&) override {}

    void Visit(Compound_Statement&) override {}
    void Visit(Expression_Statement&) override{}
    void Visit(Selection_Statement&) override{}
    void Visit(Iteration_Statement&) override{}
    void Visit(Break_Jump&) override{}
    void Visit(Continue_Jump&) override{}
    void Visit(Return_Jump&) override {}
    void Visit(Declaration_Statement&) override{}

    void print(Expression_Node root){
        root -> Accept(*this);
        std::cout << std::endl;
    }

    void Visit(Assignment_Expression&) override;
    void Visit(Or_Expression&) override;
    void Visit(And_Expression&) override;
    void Visit(Equality_Expression&) override;
    void Visit(Relational_Expression&) override;
    void Visit(Additive_Expression&) override;
    void Visit(Multiplicative_Expression&) override;
    void Visit(Identifier_Expression&) override;
    void Visit(Number_Expression&) override;
    void Visit(CharLiteral_Expression&) override;
    void Visit(StringLiteral_Expression&) override;
    void Visit(Parenthesized_Expression&) override;
    void Visit(Unary_Expression&) override;
    void Visit(FunctionCall_Expression&) override;
};


struct Executer : Visitor {  
    Executer(std::vector<std::pair<std::optional<Variable_Declaration>, std::optional<FunctionDefinition>>> global_scope);

    void execute();

    void Visit(FunctionDefinition&) override;
    void Visit(Variable_Declaration&) override;

    void Visit(Compound_Statement&) override;
    void Visit(Expression_Statement&) override;
    void Visit(Selection_Statement&) override;
    void Visit(Iteration_Statement&) override;
    void Visit(Break_Jump&) override;
    void Visit(Continue_Jump&) override;
    void Visit(Declaration_Statement&) override;
    void Visit(Return_Jump&) override;


    void Visit(Assignment_Expression&) override;
    void Visit(Or_Expression&) override;
    void Visit(And_Expression&) override;
    void Visit(Equality_Expression&) override;
    void Visit(Relational_Expression&) override;
    void Visit(Additive_Expression&) override;
    void Visit(Multiplicative_Expression&) override;
    void Visit(Identifier_Expression&) override;
    void Visit(Number_Expression&) override;
    void Visit(CharLiteral_Expression&) override;
    void Visit(StringLiteral_Expression&) override;
    void Visit(Parenthesized_Expression&) override;
    void Visit(Unary_Expression&) override;
    void Visit(FunctionCall_Expression&) override;

    void print(FunctionCall_Expression&);
    void scan(FunctionCall_Expression&);

    ScopeManager scope_manager;
    std::unordered_map<std::string, std::shared_ptr<FunctionDefinition>> function_table;
    //std::any current_result;
    long double current_numeric_result;
    std::string current_string_result;

    std::string identifier_name;

    Scope global_scope;

    bool break_flag = false;
    bool continue_flag = false;
    bool return_flag = false;
    bool floating_point = false;
};