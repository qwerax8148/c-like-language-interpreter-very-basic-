#include "Parser.hpp"

Expression_Node Parser::parse_PrimaryExpression(){
    auto Possible_Constant = Expect_Token(Number);

    if(Possible_Constant.has_value()){
        return std::make_shared<Number_Expression>(Possible_Constant.value().token);
    }
    Possible_Constant = Expect_Token(CharLiteral);

    if(Possible_Constant.has_value())
        return std::make_shared<CharLiteral_Expression>(Possible_Constant.value().token[0]);

    auto Possible_StringLiteral = Expect_Token(StringLiteral);

    if(Possible_StringLiteral.has_value())
        return std::make_shared<StringLiteral_Expression>(Possible_StringLiteral.value().token);
    
    auto Possible_Identifier = Expect_Token(Identifier);

    if(Possible_Identifier.has_value()){
        if(Expect_Token(OpenBraces, "(").has_value()){
            if(Expect_Token(CloseBraces, ")").has_value()){
                return std::make_shared<FunctionCall_Expression>(Possible_Identifier.value().token);
            }

            std::vector<Expression_Node> args;
            auto arg_expr = Expect_Expression();

            if(arg_expr.has_value()){
                args.push_back(arg_expr.value());

                while(Current_Token != End_Token && Expect_Token(Operator, ",").has_value()){
                    arg_expr = Expect_Expression();

                    if(!arg_expr.has_value())
                        throw std::runtime_error("Expected \")\" for function call \"" + Possible_Identifier.value().token + "\"");

                    args.push_back(arg_expr.value());

                    if(Expect_Token(CloseBraces, ")").has_value())
                        return std::make_shared<FunctionCall_Expression>(Possible_Identifier.value().token, args);
                }
                if(Current_Token == End_Token)
                    throw std::runtime_error("Expected \")\" for function call \"" + Possible_Identifier.value().token + "\"");

                if(Expect_Token(CloseBraces, ")").has_value()){
                    return std::make_shared<FunctionCall_Expression>(Possible_Identifier.value().token, args);
                }else{
                    throw std::runtime_error("unknown error during parsing");
                }
            }

            if(Expect_Token(CloseBraces, ")"))
                return std::make_shared<FunctionCall_Expression>();
            
            throw std::runtime_error("Expected \")\" for function call \"" + Possible_Identifier.value().token + "\"");
        }else{
            auto Possible_Postfix = Expect_Token(Operator);

            if(Possible_Postfix.has_value()){
                if(Unary_Operators.contains(Possible_Postfix.value().token) && Possible_Postfix.value().token != "+" && Possible_Postfix.value().token != "-"){
                    return std::make_shared<Unary_Expression>(std::make_shared<Identifier_Expression>(Possible_Identifier.value().token), EMPTY_UNARY, Unary_Operators.at(Possible_Postfix.value().token));
                }else{
                    --Current_Token;
                    return std::make_shared<Identifier_Expression>(Possible_Identifier.value().token);
                }
            }else 
                return std::make_shared<Identifier_Expression>(Possible_Identifier.value().token);
        }
    }

    if(Expect_Token(OpenBraces, "(")){
        auto Possible_Expr = Expect_Expression();

        if(Possible_Expr.has_value()){
            if(Expect_Token(CloseBraces, ")").has_value()){
                auto Possible_Postfix = Expect_Token(Operator);

                if(Possible_Postfix.has_value()){
                    if(Unary_Operators.contains(Possible_Postfix.value().token))
                        return std::make_shared<Unary_Expression>(Possible_Expr.value(), EMPTY_UNARY, Unary_Operators.at(Possible_Postfix.value().token));

                    else{
                        Current_Token--;
                        return std::make_shared<Parenthesized_Expression>(Possible_Expr.value());
                    }
                }
                return std::make_shared<Parenthesized_Expression>(Possible_Expr.value());
            }

        }else{
            throw std::runtime_error("Expected Expression after \"(\"");
        }
    }

    auto Possible_UnaryOp = Expect_Token(Operator);

    if(Possible_UnaryOp.has_value()){
        if(Unary_Operators.contains(Possible_UnaryOp.value().token)){
            Expression_Node expr = parse_PrimaryExpression();

            if(expr)
                return std::make_shared<Unary_Expression>(expr, Unary_Operators.at(Possible_UnaryOp.value().token), EMPTY_UNARY);
            else 
                throw std::runtime_error("Expected Expression after \"" + Possible_UnaryOp.value().token + "\"");
        }

        throw std::runtime_error("Expected Token for Expression");

    }else{
        if(Current_Token == End_Token)
            throw std::runtime_error("Expected Token for Expression");

        throw std::runtime_error("Unexpected Token \"" + Current_Token -> token + "\" For Expression ");
    }
}


Expression_Node Parser::parse_MultiplicativeExpression(){
   if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_PrimaryExpression();

    if(lhs == nullptr){
        Current_Token = start_token;
        return nullptr;
    }

    auto Possible_MulOperator = Expect_Token(Operator);

    if(Possible_MulOperator.has_value()){
        if(Multiplicative_Operators.contains(Possible_MulOperator.value().token)){
            auto rhs = parse_MultiplicativeExpression();
            return std::make_shared<Multiplicative_Expression>(lhs, rhs, Multiplicative_Operators.at(Possible_MulOperator.value().token));
        }else{
            --Current_Token;
            return std::make_shared<Multiplicative_Expression>(lhs, nullptr, EMPTY_MUL);
        }
    }

    return std::make_shared<Multiplicative_Expression>(lhs, nullptr, EMPTY_MUL); 
}


Expression_Node Parser::parse_AdditiveExpression(){
    if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_MultiplicativeExpression();

    if(lhs == nullptr){
        Current_Token = start_token;
        return nullptr;
    }

    auto Possible_AddOperator = Expect_Token(Operator);

    if(Possible_AddOperator.has_value()){
        if(Additive_Operators.contains(Possible_AddOperator.value().token)){
            auto rhs = parse_AdditiveExpression();
            return std::make_shared<Additive_Expression>(lhs, rhs, Additive_Operators.at(Possible_AddOperator.value().token));
        }else{
            --Current_Token;
            return std::make_shared<Additive_Expression>(lhs, nullptr, EMPTY_ADD);
        }
    }

    return std::make_shared<Additive_Expression>(lhs, nullptr, EMPTY_ADD);
}

Expression_Node Parser::parse_RelationalExpression(){
    if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_AdditiveExpression();

    if(lhs == nullptr){
        Current_Token = start_token;
        return nullptr;
    }

    auto Possible_RelOperator = Expect_Token(Operator);

    if(Possible_RelOperator.has_value()){
        if(Relational_Operators.contains(Possible_RelOperator.value().token)){
            auto rhs = parse_RelationalExpression();
            return std::make_shared<Relational_Expression>(lhs, rhs, Relational_Operators.at(Possible_RelOperator.value().token));
        }else{
            --Current_Token;
            return std::make_shared<Relational_Expression>(lhs, nullptr, EMPTY_REL);
        }
    }

    return std::make_shared<Relational_Expression>(lhs, nullptr, EMPTY_REL);
}


Expression_Node Parser::parse_EqualityExpression(){
    if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_RelationalExpression();

    if(lhs == nullptr){
        Current_Token = start_token;
        return nullptr;
    }

    auto Possible_EqOperator = Expect_Token(Operator);

    if(Possible_EqOperator.has_value()){
        if(Equality_Operators.contains(Possible_EqOperator.value().token)){
            auto rhs = parse_EqualityExpression();
            return std::make_shared<Equality_Expression>(lhs, rhs, Equality_Operators.at(Possible_EqOperator.value().token));
        }else{
            --Current_Token;
            return std::make_shared<Equality_Expression>(lhs, nullptr, EMPTY_EQ);
        }
    }

    return std::make_shared<Equality_Expression>(lhs, nullptr, EMPTY_EQ);
}



Expression_Node Parser::parse_OrExpression(){
    if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_AndExpression();

    if(lhs == nullptr){
        Current_Token = start_token;
        return nullptr;
    }

    auto Possible_Or = Expect_Token(Operator);

    if(Possible_Or.has_value()){
        if(Possible_Or.value().token == "||"){
            auto rhs = parse_OrExpression();
            return std::make_shared<Or_Expression>(lhs, rhs);
        }else{
            --Current_Token;
            return std::make_shared<Or_Expression>(lhs, nullptr);
        }
    }

    return std::make_shared<Or_Expression>(lhs, nullptr);
}


Expression_Node Parser::parse_AndExpression(){
    if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_EqualityExpression();

    if(lhs == nullptr){
        Current_Token = start_token;
        return nullptr;
    }

    auto Possible_And = Expect_Token(Operator);

    if(Possible_And.has_value()){
        if(Possible_And.value().token == "&&"){
            auto rhs = parse_AndExpression();
            return std::make_shared<And_Expression>(lhs, rhs);
        }else{
            --Current_Token;
            return std::make_shared<And_Expression>(lhs, nullptr);
        }
    }

    return std::make_shared<And_Expression>(lhs, nullptr);
}

Expression_Node Parser::parse_AssignmentExpression(){
    if(Current_Token == End_Token)
        return nullptr;

    auto start_token = Current_Token;

    auto lhs = parse_OrExpression();

    if(!lhs){
        Current_Token = start_token;
        return nullptr;
    }
    
    auto Possible_AssignOperator = Expect_Token(Operator);

    if(Possible_AssignOperator.has_value()){
        if(Assign_Operators.contains(Possible_AssignOperator.value().token)){
            auto rhs = parse_AssignmentExpression();
            return std::make_shared<Assignment_Expression>(lhs, rhs, Assign_Operators.at(Possible_AssignOperator.value().token));
        }else{
            --Current_Token;
            return std::make_shared<Assignment_Expression>(lhs, nullptr, EMPTY_ASSIGN);
        }
    }

    return std::make_shared<Assignment_Expression>(lhs, nullptr, EMPTY_ASSIGN);
}

std::optional<Expression_Node> Parser::Expect_Expression(){
    if(Current_Token == End_Token)
        return std::nullopt;

    auto start_token = Current_Token;

    Expression_Node res = parse_AssignmentExpression();
    if(res == nullptr){
        Current_Token = start_token;
        return std::nullopt;
    }
    else
        return res;
}

std::optional<std::shared_ptr<Variable_Declaration>> Parser::Expect_VariableDeclaration(){
    if(Current_Token == End_Token)
        return std::nullopt;

    std::optional<CppToken> Possible_Qualifier = Expect_Token(Type_Qual); 

    std::optional<CppToken> Possible_Type = Expect_Token(Type);

    if(Possible_Qualifier.has_value() && !Possible_Type.has_value())
        throw std::runtime_error("Expected Type Specifier for Variable Declaration");

    if(Possible_Type.has_value()){
        std::optional<CppToken> Possible_Name = Expect_Token(Identifier);


        if(Possible_Name.has_value()){
            if(Expect_Token(Operator, ";").has_value()){
                if(Possible_Qualifier.has_value()){
                    return std::make_shared<Variable_Declaration>(CONST, Possible_Type.value().token, Possible_Name.value().token);
                }else{
                    return std::make_shared<Variable_Declaration>(EMPTY_QUALIFIER, Possible_Type.value().token, Possible_Name.value().token);
                }
            }

            if(Expect_Token(Operator, "=").has_value()){
                std::optional<Expression_Node> Possible_Expression = Expect_Expression();

                if(Possible_Expression.has_value()){
                    if(!Expect_Token(Operator, ";").has_value()){
                        throw std::runtime_error("Expected \";\" for variable declaration");
                    }
                    if(Possible_Qualifier.has_value()){
                        return std::make_shared<Variable_Declaration>(CONST, Possible_Type.value().token, Possible_Name.value().token, Possible_Expression.value(), true);
                    }else{
                        return std::make_shared<Variable_Declaration>(EMPTY_QUALIFIER, Possible_Type.value().token, Possible_Name.value().token, Possible_Expression.value());
                    }
                }else{
                    throw std::runtime_error("Expected expression for declaration");
                }
            }else{
                throw std::runtime_error("Expected \";\" for variable declaration");
            }
        }else{
            throw std::runtime_error("Expected Identifier for variable declaration");
        }
    }else{
        return std::nullopt;
    }
}

std::optional<std::shared_ptr<Expression_Statement>> Parser::Expect_ExpressionStatement(){
    if(Current_Token == End_Token)  
        return std::nullopt;

    auto start_token = Current_Token;

    if(Expect_Token(Operator, ";").has_value()){
        return std::make_shared<Expression_Statement>();
    }

    std::optional<Expression_Node> Possible_Expression = Expect_Expression();

    auto Semi = Expect_Token(Operator, ";");

    if(Possible_Expression.has_value()){
        if(Semi.has_value())
            return std::make_shared<Expression_Statement>(Possible_Expression.value());
        else
            throw std::runtime_error("Expected \";\" for Expression Statement");
    }

    Current_Token = start_token;
    return std::nullopt;
}

std::optional<std::shared_ptr<Iteration_Statement>> Parser::Expect_IterationStatement(){
    if(Current_Token == End_Token)
        return std::nullopt;

    auto start_token = Current_Token;

    if(Expect_Token(Keyword, "while").has_value()){
        if(Expect_Token(OpenBraces, "(").has_value()){
            std::optional<Expression_Node> Possible_Expression = Expect_Expression();

            if(Possible_Expression.has_value()){
                if(Expect_Token(CloseBraces, ")").has_value()){
                    std::optional<Statement_Node> Possible_Statement = Expect_Statement();

                    if(Possible_Statement.has_value()){
                        return std::make_shared<Iteration_Statement>(Possible_Expression.value(), Possible_Statement.value());
                    }else{
                        throw std::runtime_error("Expected a statement for \"while\"");
                    }
                }else{
                    throw std::runtime_error("Expected \")\" for \"while\"");
                }
            }else{
                throw std::runtime_error("Expected an expression for \"while\"");
            }
        }else{
            throw std::runtime_error("Expected \"(\" for \"while\"");
        }
    }else{
        Current_Token = start_token;
        return std::nullopt;
    }
}

std::optional<std::shared_ptr<Break_Jump>> Parser::Expect_BreakJump(){
    if(Current_Token == End_Token)
        return std::nullopt;

    if(Current_Token -> token != "break")
        return std::nullopt;

    Current_Token++;

    if(Current_Token == End_Token || Current_Token -> token != ";")
        throw std::runtime_error("expected \";\" after keyword \"break\"");

    return std::make_shared<Break_Jump>();
}

std::optional<std::shared_ptr<Continue_Jump>> Parser::Expect_ContinueJump(){
    if(Current_Token == End_Token)
        return std::nullopt;

    if(Current_Token -> token != "continue")
        return std::nullopt;

    Current_Token++;

    if(Current_Token == End_Token || Current_Token -> token != ";")
        throw std::runtime_error("expected \";\" after keyword \"break\"");

    return std::make_shared<Continue_Jump>();
}

std::optional<std::shared_ptr<Return_Jump>> Parser::Expect_ReturnJump(){
    if(Current_Token == End_Token)
        return std::nullopt;

    if(!Expect_Token(Keyword, "return").has_value())
        return std::nullopt;

    if(Current_Token == End_Token)
        throw std::runtime_error("expected expression after \"return\" keyword");

    if(Expect_Token(Operator, ";")){
        return std::make_shared<Return_Jump>(nullptr);
    }

    std::optional<Expression_Node> Possible_Expression = Expect_Expression();

    if(Possible_Expression.has_value()){
        return std::make_shared<Return_Jump>(Possible_Expression.value());
    }

    throw std::runtime_error("expected expression after \"return\" keyword");
}


std::optional<std::shared_ptr<Compound_Statement>> Parser::Expect_CompoundStatement(){
    if(Current_Token == End_Token || !Expect_Token(OpenBraces, "{").has_value())
        return std::nullopt;


    std::vector<Statement_Node> resulting_statements = {};

    while(Current_Token != End_Token){
        if(Expect_Token(CloseBraces, "}").has_value()){
            return std::make_shared<Compound_Statement>(resulting_statements);
        }

        std::optional<Statement_Node> Possible_Statement = Expect_Statement();

        if(Possible_Statement.has_value()){
            resulting_statements.push_back(Possible_Statement.value());
        }else{
            throw std::runtime_error("Expected Statemenet for Compound Statement");
        }
    }

    throw std::runtime_error("Error | Expect Compound Statement");
}

std::optional<std::shared_ptr<Selection_Statement>> Parser::Expect_SelectionStatement(){
    if(Current_Token == End_Token || !Expect_Token(Keyword, "if").has_value())
        return std::nullopt;


    if(!Expect_Token(OpenBraces, "(").has_value())
        throw std::runtime_error("Expected \"(\" after \"if\"");

    std::optional<Expression_Node> Possible_IfExpression = Expect_Expression();

    if(Possible_IfExpression.has_value()){
        if(!Expect_Token(CloseBraces, ")"))
            throw std::runtime_error("Expected \")\" for \"if\"");

        std::optional<Statement_Node> Possible_IfStatement = Expect_Statement();

        if(Possible_IfStatement.has_value()){
            if(Expect_Token(Keyword, "else").has_value()){
                std::optional<Statement_Node> Possible_ElseStatement = Expect_Statement();

                if(Possible_ElseStatement.has_value()){
                    return std::make_shared<Selection_Statement>(Possible_IfExpression.value(), Possible_IfStatement.value(), Possible_ElseStatement.value());
                }else{
                    throw std::runtime_error("Expected statement for \"else\"");
                }
            }else{
                return std::make_shared<Selection_Statement>(Possible_IfExpression.value(), Possible_IfStatement.value());
            }
        }else{
            throw std::runtime_error("Expected statement for \"if\"");
        }
    }else{
        throw std::runtime_error("Expected expression for \"if\"");
    }
}  


std::optional<Statement_Node> Parser::Expect_Statement(){
    auto start_token = Current_Token;

    if(Current_Token == End_Token)
        return std::nullopt;

    if(Expect_Token(Type_Qual).has_value() || Expect_Token(Type).has_value()){
        Current_Token--;
        std::optional<std::shared_ptr<Variable_Declaration>> Possible_VarDecl = Expect_VariableDeclaration();

        if(Possible_VarDecl.has_value())
            return std::make_shared<Declaration_Statement>(Possible_VarDecl.value());
        else
            throw std::runtime_error("Expected Variable Declaration"); 
    }

    if(Expect_Token(Identifier).has_value()){
        Current_Token--;
        return Expect_ExpressionStatement();
    }

    if(Expect_Token(StringLiteral).has_value()){
        Current_Token--;
        return Expect_ExpressionStatement();
    }

    std::optional<CppToken> Possible_Keyword = Expect_Token(Keyword);

    if(Possible_Keyword.has_value()){
        Current_Token--;

        std::string key_name = Possible_Keyword.value().token;

        if(Jumps.contains(key_name)){
            if(key_name == "break")
                return Expect_BreakJump();

            if(key_name == "continue")
                return Expect_ContinueJump();

            if(key_name == "return")
                return Expect_ReturnJump();
        }

        if(Loops.contains(key_name))
            return Expect_IterationStatement();

        if(Conditionals.contains(key_name))
            return Expect_SelectionStatement();

        throw std::runtime_error("Unexpected Statement");
    }

    if(Expect_Token(OpenBraces, "{").has_value()){
        Current_Token--;
        return Expect_CompoundStatement();
    }

    if(Expect_Token(OpenBraces, "(").has_value()){
        Current_Token--;
        return Expect_ExpressionStatement();
    }

    if(Expect_Token(Identifier).has_value() || Expect_Token(Number).has_value() || Expect_Token(Operator).has_value()){
        Current_Token--;
        return Expect_ExpressionStatement();
    }

    return std::nullopt;
}


void Parser::parse_FunctionBody(FunctionDefinition& function){
    std::optional<std::shared_ptr<Compound_Statement>> func_body = Expect_CompoundStatement();
    if(!func_body.has_value())
        throw std::runtime_error("Expected Compound Statement For Functuon Body");
    
    function.body = func_body.value();
}


void Parser::parse_FunctionParamList(FunctionDefinition& function){
    if(Expect_Token(Type, "void")){
        if(Expect_Token(CloseBraces, ")")){
            function.void_flag = 1;
            return;
        }
        else
            throw std::runtime_error("Expected \")\" for function \"" + function.name + "\"");
    }

    while(Expect_Token(CloseBraces, ")") == std::nullopt){
        std::optional<CppToken> Possible_Type = Expect_Token(Type);

        if(Possible_Type == std::nullopt)
            throw std::runtime_error("Expected Type In Param List instead of \"" + std::string(*Current_Token) + "\"");

        std::optional<CppToken> Possible_VariableName = Expect_Token(Identifier);

        if(Possible_VariableName == std::nullopt)
            throw std::runtime_error("Expected Variable Name In Param List instead of \"" + std::string(*Current_Token) + "\"");

        function.Parameter_List.push_back(ParameterDefinition(Possible_VariableName.value().token, Possible_Type.value().token));

        if(Expect_Token(Operator, ",") != std::nullopt)
            continue;

        if(Expect_Token(CloseBraces, ")") == std::nullopt)
            throw std::runtime_error("expected closing bracket in function param list instead of \"" + std::string(*Current_Token) + "\"");

        break;
    }
}

std::optional<std::shared_ptr<FunctionDefinition>> Parser::Expect_FunctionDefinition(){
    auto start_token = Current_Token;
    std::optional<CppToken> Possible_Type = Expect_Token(Type);

    FunctionDefinition function;

        if(Possible_Type.has_value()){
            std::optional<CppToken> Possible_Name = Expect_Token(Identifier);

            function.return_type = Possible_Type.value().token;

            if(Possible_Name.has_value()){
                std::optional<CppToken> Possible_OpenBraces = Expect_Token(OpenBraces, "(");
                
                function.name = Possible_Name.value().token;

                if(Possible_OpenBraces.has_value()){

                    parse_FunctionParamList(function);
                    parse_FunctionBody(function);

                    return std::make_shared<FunctionDefinition>(function);
                }else{
                    Current_Token = Current_Token;
                }
            }else{
                Current_Token = Current_Token;
            }
        }
        return std::nullopt;
}

void Parser::parse(std::vector<CppToken> tokens){
    Current_Token = tokens.begin();
    End_Token = tokens.end();
    auto start_token = Current_Token;

    while(Current_Token != End_Token){
        auto function = Expect_FunctionDefinition();

        if(function.has_value()){
            Global_Scope.push_back(std::make_pair(std::nullopt, *function.value()));
            start_token = Current_Token;
        }else{
            Current_Token = start_token;

            auto variable = Expect_VariableDeclaration();

            if(variable.has_value()){
                if(!Expect_Token(Operator, ";").has_value())
                    throw std::runtime_error("Expected \";\" for variable declaration");

                Global_Scope.push_back(std::make_pair(*variable.value(), std::nullopt));
                start_token = Current_Token;
            }else{
                if(Current_Token == End_Token)
                    break;
                else
                    throw std::runtime_error("Expected Function or Variable Declaration");    
            }
        }
    }
}

std::optional<CppToken> Parser::Expect_Token(CppTokenType t, const std::string& name){
    if(Current_Token == End_Token || Current_Token -> type != t)
        return std::nullopt;

    CppToken return_token = *Current_Token;

    if(name == "" || name == std::string(*Current_Token)){
        Current_Token++;
        return return_token;
    }
    else
        return std::nullopt;
} 