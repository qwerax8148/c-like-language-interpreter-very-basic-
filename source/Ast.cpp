#pragma once

#include "Visitor.hpp"


void Compound_Statement::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Expression_Statement::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Selection_Statement::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Iteration_Statement::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Break_Jump::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Continue_Jump::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Return_Jump::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Declaration_Statement::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Variable_Declaration::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void FunctionDefinition::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Assignment_Expression ::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void And_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Or_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Equality_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Relational_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Additive_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Multiplicative_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Identifier_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Number_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void CharLiteral_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void StringLiteral_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Parenthesized_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void Unary_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}

void FunctionCall_Expression::Accept(Visitor& visitor){
    visitor.Visit(*this);
}   

