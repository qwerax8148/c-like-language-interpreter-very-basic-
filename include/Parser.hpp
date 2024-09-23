#pragma once

#include "Lexer.hpp"
#include <optional>
#include "Type.hpp"
#include "Visitor.hpp"
#include <stdexcept>
#include "utility"

class Parser{
    
    std::unordered_map<std::string, Assign_Op> Assign_Operators = {
        {"=", ASSIGN},
        // {"+=", ADD_ASSIGN},
        // {"-=", SUB_ASSIGN},
        // {"*=", MUL_ASSIGN},
        // {"/=", DIV_ASSIGN},
        // {"%=", MOD_ASSIGN}
    };

    std::unordered_map<std::string, Conditional_Op> Conditional_Operators = {
        {"&&", AND},
        {"||", OR}
    };

    std::unordered_map<std::string, Eq_Op> Equality_Operators = {
        {"==", EQUAL},
        {"!=", NOT_EQUAL}
    };

    std::unordered_map<std::string, Rel_Op> Relational_Operators = {
        {">", GREATER},
        {">=", GREATER_OR_EQUAL},
        {"<", LESS},
        {"<=", LESS_OR_EQUAL}
    };


    std::unordered_map<std::string, Add_Op> Additive_Operators = {
        {"+", PLUS},
        {"-", MINUS}
    };

    std::unordered_map<std::string, Mul_Op> Multiplicative_Operators = {
        {"*", MULTIPLICATION},
        {"/", DIVISION},
        {"%", MODULO}
    };

    std::unordered_map<std::string, Unary_Op> Unary_Operators = {
        {"+", UNARY_PLUS},
        {"-", UNARY_MINUS},
        {"++", INC},
        {"--", DEC}
    };


public:
    void parse(std::vector<CppToken>);
    void parse_FunctionBody(FunctionDefinition&);
    void parse_FunctionParamList(FunctionDefinition&);

    Expression_Node parse_PrimaryExpression();
    Expression_Node parse_MultiplicativeExpression();
    Expression_Node parse_AdditiveExpression();
    Expression_Node parse_RelationalExpression();
    Expression_Node parse_EqualityExpression();
    Expression_Node parse_AssignmentExpression();
    Expression_Node parse_AndExpression();
    Expression_Node parse_OrExpression();

    std::optional<Expression_Node> Expect_Expression();


    std::optional<std::shared_ptr<Selection_Statement>> Expect_SelectionStatement();
    std::optional<std::shared_ptr<Iteration_Statement>> Expect_IterationStatement();
    std::optional<std::shared_ptr<Break_Jump>> Expect_BreakJump();
    std::optional<std::shared_ptr<Continue_Jump>> Expect_ContinueJump();
    std::optional<std::shared_ptr<Return_Jump>> Expect_ReturnJump();
    std::optional<std::shared_ptr<Expression_Statement>> Expect_ExpressionStatement();
    std::optional<std::shared_ptr<Variable_Declaration>> Expect_VariableDeclaration();
    std::optional<Statement_Node> Expect_Statement();
    std::optional<std::shared_ptr<Compound_Statement>> Expect_CompoundStatement();

    std::optional<std::shared_ptr<FunctionDefinition>> Expect_FunctionDefinition();

    std::optional<CppToken> Expect_Token(CppTokenType, const std::string& = "");

    std::vector<std::pair<std::optional<Variable_Declaration>, std::optional<FunctionDefinition>>> Global_Scope;

private:
    std::vector<CppToken>::iterator Current_Token;
    std::vector<CppToken>::iterator End_Token;
};