#pragma once

#include "Type.hpp"
#include <vector>
#include <memory>
#include <iostream>

enum Eq_Op{
        EQUAL,
        NOT_EQUAL,
        EMPTY_EQ
    };

    enum Rel_Op{
        LESS,
        GREATER,
        LESS_OR_EQUAL,
        GREATER_OR_EQUAL,
        EMPTY_REL
    };

    enum Add_Op{
        MINUS,
        PLUS,
        EMPTY_ADD
    };

    enum Mul_Op{
        DIVISION,
        MULTIPLICATION,
        MODULO,
        EMPTY_MUL
    };

    enum Unary_Op{
        INC,
        DEC,
        UNARY_MINUS,
        UNARY_PLUS,
        EMPTY_UNARY
    };

    enum Assign_Op{
        ASSIGN,
        // MUL_ASSIGN,
        // DIV_ASSIGN,
        // ADD_ASSIGN,
        // MOD_ASSIGN,
        // SUB_ASSIGN,
        EMPTY_ASSIGN
    };

    enum Conditional_Op{
        OR,
        AND,
        EMPTY_CONDITIONAL
    };

    static const std::unordered_map<Assign_Op, std::string> Assign_Dict {
        {ASSIGN, "="},
        // {MUL_ASSIGN, "*="},
        // {DIV_ASSIGN, "/="},
        // {ADD_ASSIGN, "+="},
        // {MOD_ASSIGN, "%="},
        // {SUB_ASSIGN, "-="}
    };

    static const std::unordered_map<Conditional_Op, std::string> Consitional_Dict {
        {OR, "||"},
        {AND, "&&"}
    };

    static const std::unordered_map<Unary_Op, std::string> Unary_Dict {
        {DEC, "--"},
        {INC, "++"},
        {UNARY_MINUS, "-"},
        {UNARY_PLUS, "+"}
    };

    static const std::unordered_map<Mul_Op, std::string> Mutiplication_Dict {
        {MULTIPLICATION, "*"},
        {DIVISION, "/"},
        {MODULO, "%"}
    };

    static const std::unordered_map<Add_Op, std::string> Addition_Dict {
        {MINUS, "-"},
        {PLUS, "+"}
    };

    static const std::unordered_map<Rel_Op, std::string> Relation_Dict {
        {LESS, "<"},
        {GREATER, ">"},
        {LESS_OR_EQUAL, "<="},
        {GREATER_OR_EQUAL, ">="}
    };

    static const std::unordered_map<Eq_Op, std::string> Equality_Dict {
        {EQUAL, "=="},
        {NOT_EQUAL, "!="}
    };
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Visitor;
struct Variable_Declaration;
struct Function_Definition;
struct ParameterDefinition;
struct Expression;
struct Statement;
struct Ast;
struct Declaration;
struct Assignment_Expression;
struct Equality_Expression;
struct Additive_Expression;
struct Multiplicative_Expression;
struct Conditional_Expression;
struct Relational_Expression;
struct Primary_Expression;
struct Constant_Expression;
struct StringLiterat_Expression;
struct Identifier_Expression;
struct Parenthesized_Expression;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Statement_Node = std::shared_ptr<Statement>;
using Declaration_Node = std::shared_ptr<Declaration>;
using Ast_Node = std::shared_ptr<Ast>;
using Expression_Node = std::shared_ptr<Expression>;


struct Ast{
    virtual ~Ast() = default;
    virtual void Accept(Visitor&) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Declaration : Ast {
    virtual ~Declaration() = default;
    virtual void Accept(Visitor&) = 0;
};

struct Statement : Ast {
    virtual ~Statement() = default;
    virtual void Accept(Visitor&) = 0;
};

struct Expression : Ast {
    virtual ~Expression() = default;
    virtual void Accept(Visitor&) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Compound_Statement : Statement {
    Compound_Statement() = default;
    Compound_Statement(std::vector<Statement_Node> st) : statements(st) {}

    void Accept(Visitor& visitor);

    std::vector<Statement_Node> statements;
};

struct Expression_Statement : Statement {
    Expression_Statement(Expression_Node expr = nullptr) : expression(expr) {} 

    void Accept(Visitor& visitor);

    Expression_Node expression;
};

struct Selection_Statement : Statement {
    Selection_Statement() : if_expression(nullptr), if_statement(nullptr), else_statement(nullptr) {}
    Selection_Statement(Expression_Node if_expr, Statement_Node if_st, Statement_Node else_st = nullptr) : if_expression(if_expr), if_statement(if_st), else_statement(else_st) {}

    void Accept(Visitor& visitor);

    Expression_Node if_expression;
    Statement_Node if_statement;
    Statement_Node else_statement;
};

struct Iteration_Statement : Statement {
    Iteration_Statement() : while_expression(nullptr), while_statement(nullptr) {} 
    Iteration_Statement(Expression_Node wh_expr, Statement_Node wh_st) : while_expression(wh_expr), while_statement(wh_st) {}

    void Accept(Visitor& visitor);

    Expression_Node while_expression;
    Statement_Node while_statement;
};

struct Jump_Statement : Statement {
    virtual ~Jump_Statement() = default;
    virtual void Accept(Visitor&) = 0;
};

struct Break_Jump : Jump_Statement {
    Break_Jump() = default;

    void Accept(Visitor& visitor);
};

struct Continue_Jump : Jump_Statement {
    Continue_Jump() = default;

    void Accept(Visitor& visitor);
};

struct Return_Jump : Jump_Statement {
    Return_Jump(Expression_Node expr = nullptr) : return_expression(expr) {}

    void Accept(Visitor& visitor);

    Expression_Node return_expression;
};



struct Declaration_Statement : Statement {
    Declaration_Statement(std::shared_ptr<Variable_Declaration> vd = nullptr) : var_decl(vd) {}

    void Accept(Visitor& visitor);

    std::shared_ptr<Variable_Declaration> var_decl;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ParameterDefinition {
    ParameterDefinition() = default;
    ParameterDefinition(std::string name, std::string t) : name(name), t(t) {}

    std::string name;
    BuiltinType t;
};

struct Variable_Declaration : public Declaration {
    Variable_Declaration() = default;
    Variable_Declaration(Type_Qualifier t_q, BuiltinType t, std::string name, Expression_Node expr = nullptr, bool const_decl = false) : type_specifyer(t), name(name), assign_expression(expr), type_qualifier(t_q), const_declaration(const_decl) {}
    Variable_Declaration(ParameterDefinition par) : type_specifyer(par.t), name(par.name), assign_expression(nullptr), type_qualifier(EMPTY_QUALIFIER) {}

    void print(){
        std::cout << type_specifyer.name << ' ' << name << std::endl;  
    }
    void Accept(Visitor& visitor);
  
    Type_Qualifier type_qualifier;
    BuiltinType type_specifyer;
    std::string name;
    Expression_Node assign_expression;
    bool const_declaration = false;
};

struct FunctionDefinition : public Declaration {
    void print(){
        std::cout << return_type.name << ' ' << name << '(';
        if(!void_flag){
            for(int i = 0; i < Parameter_List.size() - 1; ++i){
                std::cout << Parameter_List[i].t.name << ' ' 
                << Parameter_List[i].name << ", ";
            }
            std::cout << Parameter_List[Parameter_List.size() - 1].t.name
            << ' ' << Parameter_List[Parameter_List.size() - 1].name << ")\n";
        }else{
            std::cout << "void)\n";
        }
    }

    void Accept(Visitor& visitor);

    int void_flag = 0;
    std::string name;
    BuiltinType return_type;
    std::vector<ParameterDefinition> Parameter_List;
    std::shared_ptr<Compound_Statement> body;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//unary += unary = unary -= unary = conditional
struct Assignment_Expression : Expression {
    Assignment_Expression(){
        lhs = nullptr; 
        rhs = nullptr; 
    }
    Assignment_Expression(Expression_Node lhs, Expression_Node rhs, Assign_Op op) : lhs(lhs), rhs(rhs), op(op) {}

    void Accept(Visitor& visitor);

    Expression_Node lhs, rhs;
    Assign_Op op;
};



struct Conditional_Expression : Expression {
    virtual ~Conditional_Expression() = default;
    virtual void Accept(Visitor&) = 0;
};

struct And_Expression : Conditional_Expression {
    And_Expression(Expression_Node lhs = nullptr, Expression_Node rhs = nullptr) : lhs(lhs), rhs(rhs) {}

    void Accept(Visitor&);

    Expression_Node lhs, rhs;
};

struct Or_Expression : Conditional_Expression {
    Or_Expression(Expression_Node lhs = nullptr, Expression_Node rhs = nullptr) : lhs(lhs), rhs(rhs) {}

    void Accept(Visitor&);

    Expression_Node lhs, rhs;
};



struct Equality_Expression : Expression {
    Equality_Expression(){
        lhs = nullptr; 
        rhs = nullptr; 
        op = EMPTY_EQ;
    }
    Equality_Expression(Expression_Node lhs, Expression_Node rhs, Eq_Op op) : lhs(lhs), rhs(rhs), op(op) {}

    void Accept(Visitor&);

    Expression_Node lhs, rhs;
    Eq_Op op;
};

struct Relational_Expression : Expression {
    Relational_Expression(){
        lhs = nullptr; 
        rhs = nullptr; 
        op = EMPTY_REL;
    }
    Relational_Expression(Expression_Node lhs, Expression_Node rhs, Rel_Op op) : lhs(lhs), rhs(rhs), op(op) {}

    void Accept(Visitor&);

    Expression_Node lhs, rhs;
    Rel_Op op;
};

struct Additive_Expression : Expression {
    Additive_Expression(){
        lhs = nullptr; 
        rhs = nullptr; 
        op = EMPTY_ADD;
    }
    Additive_Expression(Expression_Node lhs, Expression_Node rhs, Add_Op op) : lhs(lhs), rhs(rhs), op(op) {}

    void Accept(Visitor&);

    Expression_Node lhs, rhs;
    Add_Op op;
};

struct Multiplicative_Expression : Expression {
    Multiplicative_Expression(){
        lhs = nullptr; 
        rhs = nullptr; 
        op = EMPTY_MUL; 
    }
    Multiplicative_Expression(Expression_Node lhs, Expression_Node rhs, Mul_Op op) : lhs(lhs), rhs(rhs), op(op) {}

    void Accept(Visitor&);

    Expression_Node lhs, rhs;
    Mul_Op op;
};

struct Primary_Expression : Expression {
    virtual ~Primary_Expression() = default;
    virtual void Accept(Visitor&) = 0;
};

struct Identifier_Expression : Primary_Expression {
    Identifier_Expression(const std::string& value = "") : value(value) {}

    void Accept(Visitor&);

    std::string value;
};

struct Constant_Expression : Primary_Expression {
    virtual ~Constant_Expression() = default;
    virtual void Accept(Visitor&) = 0;
};

struct Number_Expression : Constant_Expression {
    Number_Expression(const std::string name = "") : name(name) {}

    void Accept(Visitor&);

    std::string name;
}; 

struct CharLiteral_Expression : Constant_Expression {
    CharLiteral_Expression() = default;
    CharLiteral_Expression(char c) : character(c) {}

    void Accept(Visitor&);

    char character;
};

struct StringLiteral_Expression : Constant_Expression {
    StringLiteral_Expression(const std::string& value = "") : value(value) {}

    void Accept(Visitor&);

    std::string value;
};



struct Parenthesized_Expression : Primary_Expression {
    Parenthesized_Expression(Expression_Node node = nullptr) : node(node) {}

    void Accept(Visitor&);

    Expression_Node node;
};

struct Unary_Expression : Primary_Expression {
    Unary_Expression(){
        node = nullptr; 
        prefix = EMPTY_UNARY; 
        postfix = EMPTY_UNARY; 
    }
    Unary_Expression(Expression_Node node, Unary_Op prefix, Unary_Op postfix) : node(node), prefix(prefix), postfix(postfix) {}

    void Accept(Visitor&);

    Expression_Node node;
    Unary_Op prefix, postfix; 
};

struct FunctionCall_Expression : Primary_Expression {
    FunctionCall_Expression() = default;
    FunctionCall_Expression(std::string name, std::vector<Expression_Node> al = {}) : name(name), arg_list(al) {}

    void Accept(Visitor&);

    std::vector<Expression_Node> arg_list;
    std::string name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////