#pragma once

#include "Visitor.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Expression_Printer::Visit(Assignment_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.op != EMPTY_ASSIGN){
        std::cout << ' ' << Assign_Dict.at(expr.op) << ' ';
        expr.rhs -> Accept(*this);
    }
}

void Expression_Printer::Visit(Or_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.rhs){
        std::cout << " || ";
        expr.rhs -> Accept(*this);
    }
}

void Expression_Printer::Visit(And_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.rhs){
        std::cout << " && ";
        expr.rhs -> Accept(*this);
    }
}


void Expression_Printer::Visit(Equality_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.rhs){
        std::cout << ' ' << Equality_Dict.at(expr.op) << ' ';
        expr.rhs -> Accept(*this);
    }
}

void Expression_Printer::Visit(Relational_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.rhs){
        std::cout << ' ' << Relation_Dict.at(expr.op) << ' ';
        expr.rhs -> Accept(*this);
    }
}

void Expression_Printer::Visit(Additive_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.rhs){
        std::cout << ' ' << Addition_Dict.at(expr.op) << ' ';
        expr.rhs -> Accept(*this);
    }
}   

void Expression_Printer::Visit(Multiplicative_Expression& expr){
    expr.lhs -> Accept(*this);
    if(expr.rhs){
        std::cout << ' ' << Mutiplication_Dict.at(expr.op) << ' ';
        expr.rhs -> Accept(*this);
    }
}

void Expression_Printer::Visit(Identifier_Expression& expr){
    std::cout << expr.value;
}

void Expression_Printer::Visit(StringLiteral_Expression& expr){
    std::cout << '\"' << expr.value << '\"';
}

void Expression_Printer::Visit(Parenthesized_Expression& expr){
    std::cout << "(";
    expr.node -> Accept(*this);
    std::cout << ")";
}

void Expression_Printer::Visit(Unary_Expression& expr){
    if(expr.prefix != EMPTY_UNARY) std::cout << Unary_Dict.at(expr.prefix);
    expr.node -> Accept(*this);
    if(expr.postfix != EMPTY_UNARY) std::cout << Unary_Dict.at(expr.postfix);
}

void Expression_Printer::Visit(FunctionCall_Expression& expr){
    std::cout << expr.name << "(";
    for(int i = 0; i < expr.arg_list.size(); ++i){   
        expr.arg_list[i] -> Accept(*this);
        std::cout << ", ";
    }
    std::cout << ")";
}

void Expression_Printer::Visit(Number_Expression& expr){
    std::cout << expr.name;
}

void Expression_Printer::Visit(CharLiteral_Expression& expr){
    std::cout << expr.character;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool Analyzer::Is_Number_Type(BuiltinType type){
    std::string s = type.name;

    return s == "int" || s == "double" || s == "long" || s == "bool" || 
    s == "char" || s == "long long" || s == "long double" || s == "short" || s == "float";
}

void Analyzer::analyze(){
    function_dictionaty["print"] = nullptr;
    function_dictionaty["scan"] = nullptr;

    scope_manager.enterScope(Scope(REGULAR_SCOPE, nullptr));

    bool has_main = false;

    for(auto el : Global_Scope){
        std::optional<Variable_Declaration> Possible_VarDec = el.first;
        std::optional<FunctionDefinition> Possible_Function = el.second;

         if(Possible_VarDec.has_value()){
            Variable_Declaration var = Possible_VarDec.value();

            if(scope_manager.scopes.top() -> symbol_table.contains(var.name))
                throw std::runtime_error("Redeclration of Variable \"" + var.name + "\"");

            var.Accept(*this);
            continue;
        }

        if(Possible_Function.has_value()){
            FunctionDefinition function = Possible_Function.value();

            if(function.name == "main")
                has_main = true;

            if(function_dictionaty.contains(function.name))
                throw std::runtime_error("Redefinition of function \"" + function.name + "\"");
            
            function.Accept(*this);
        }else{
            throw std::runtime_error("Expected Function or Variable Declaration");                 
        }
    }

    if(!has_main)
        throw std::runtime_error("no main function in global scope");
}

void Analyzer::Visit(FunctionDefinition& function){
    scope_manager.enterScope(Scope(FUNCTION_SCOPE, scope_manager.scopes.top()));
    auto current_scope = scope_manager.scopes.top();

    function_dictionaty[function.name] = std::make_shared<FunctionDefinition>(function);

    return_type = function.return_type;

    if(!function.void_flag){
        for(auto par : function.Parameter_List){
            Variable variable(EMPTY_QUALIFIER, par.t, par.name);
            scope_manager.scopes.top() -> add(par.name, variable);
        }
    }

    function.body -> Accept(*this);
    
    scope_manager.exitScope();
}

void Analyzer::Visit(Variable_Declaration& var){
    if(scope_manager.scopes.top() -> symbol_table.contains(var.name))
        throw std::runtime_error("Redeclaraion of variable \"" + var.name + "\"");

    bool const_decl = var.type_qualifier == CONST ? true : false;

    Variable variable(var.type_qualifier, var.type_specifyer, var.name);

    if(var.assign_expression){

        int f1 = Is_Number_Type(var.type_specifyer);

        var.assign_expression -> Accept(*this);

        int f2 = is_convertable_to_number;

        if((f1 && !f2) || (!f1 && f2)){
            throw std::runtime_error("assignment expression is not convertable to \"" + var.type_specifyer.name + "\"");
        }
        Variable variable(var.type_qualifier, var.type_specifyer, var.name, const_decl);

        scope_manager.scopes.top() -> symbol_table[var.name] = variable;
    }else{
        Variable variable(var.type_qualifier, var.type_specifyer, var.name, false);

        scope_manager.scopes.top() -> symbol_table[var.name] = variable;
    }

}

void Analyzer::Visit(Compound_Statement& compound_statement){
    scope_manager.enterScope(Scope(REGULAR_SCOPE, scope_manager.scopes.top()));

    for(auto statement : compound_statement.statements){
        statement -> Accept(*this);
    }

    scope_manager.exitScope();
}

void Analyzer::Visit(Expression_Statement& expression_statement){
    if(expression_statement.expression)
        expression_statement.expression -> Accept(*this);
}

void Analyzer::Visit(Selection_Statement& selection_statement){
    if(selection_statement.if_expression){
        selection_statement.if_expression -> Accept(*this);
        if(selection_statement.if_statement)
            selection_statement.if_statement -> Accept(*this);
        else
            throw std::runtime_error("Expected Statement for \"if\"");

        if(selection_statement.else_statement)
            selection_statement.else_statement -> Accept(*this);
    
    }else{
        throw std::runtime_error("Expected Expression for \"if\"");
    }
}

void Analyzer::Visit(Iteration_Statement& iteration_statement){
    scope_manager.enterScope(Scope(WHILE_SCOPE, scope_manager.scopes.top()));

    if(iteration_statement.while_expression)
        iteration_statement.while_expression -> Accept(*this);
    else
        throw std::runtime_error("Expected Expression for \"while\"");

    if(iteration_statement.while_statement)
        iteration_statement.while_statement -> Accept(*this);
    else
        throw std::runtime_error("Expected Statement For \"while\"");

    scope_manager.exitScope();
}

void Analyzer::Visit(Break_Jump& break_jump){
    auto current_scope = scope_manager.scopes.top();

    while(current_scope){
        if(current_scope -> type == WHILE_SCOPE)
            return;
        current_scope = current_scope -> parent;
    }

    throw std::runtime_error("\"break\" outside of \"while\" scope");
}

void Analyzer::Visit(Continue_Jump& continue_jump){
    auto current_scope = scope_manager.scopes.top();

    while(current_scope){
        if(current_scope -> type == WHILE_SCOPE)
            return;
        current_scope = current_scope -> parent;
    }

    throw std::runtime_error("\"continue\" outside \"while\" scope");
}

void Analyzer::Visit(Return_Jump& return_jump){
    if(return_jump.return_expression){
        if(return_type.name == "void")
            throw std::runtime_error("return type does not match function type");

        return_jump.return_expression -> Accept(*this);
        int f1 = Is_Number_Type(return_type);
        int f2 = is_convertable_to_number;

        if((f1 && !f2) || (f2 && !f1))
            throw std::runtime_error("return type does not match function type");
        
    }else if(return_type.name != "void")
            throw std::runtime_error("return type does not match function type");

    auto current_scope = scope_manager.scopes.top();
    while(current_scope && current_scope -> type != FUNCTION_SCOPE){
        current_scope = current_scope -> parent;
    }

    if(!current_scope)
        throw std::runtime_error("Unknown Error | return_jump");

}

void Analyzer::Visit(Declaration_Statement& declaration_statement){
        declaration_statement.var_decl -> Accept(*this);
}

void Analyzer::Visit(Assignment_Expression& assignment_expression){
    auto lhs = assignment_expression.lhs;
    if(lhs) 
        lhs -> Accept(*this);
    else
        throw std::runtime_error("empty \"assignment expression\"");

    auto rhs = assignment_expression.rhs;

    if(rhs){
        if(!is_lvalue)
            throw std::runtime_error("assigning to rvalue object");

        auto cur_scope = scope_manager.scopes.top();
        while(cur_scope && !cur_scope -> symbol_table.contains(identifier))
            cur_scope = cur_scope -> parent;

        if(!cur_scope)
            throw std::runtime_error("assigning to an undeclared variable");

        auto var = cur_scope -> symbol_table[identifier];
        auto save = identifier;

        if(var.type_qualifier == CONST)
            throw std::runtime_error("Assigning to a const variable");

        bool f1 = is_convertable_to_number;
        bool f2 = is_string;

        rhs -> Accept(*this);

        bool f3 = is_convertable_to_number;
        bool f4 = is_string;

        if((f1 && f4) || (f2 && f3))
            throw std::runtime_error("incompatible types in assign expression");
        
        identifier = save;
        is_lvalue = true;
    }
}

void Analyzer::Visit(Or_Expression& or_expression){
    auto lhs = or_expression.lhs;
    auto rhs = or_expression.rhs;

    if(lhs)
        lhs -> Accept(*this);
    else
        throw std::runtime_error("Empty \"or_Expression\"");

    if(rhs){
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");
        rhs -> Accept(*this);
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");

        identifier = "";
        is_lvalue = false;
    }
}

void Analyzer::Visit(And_Expression& and_expression){
    auto lhs = and_expression.lhs;
    auto rhs = and_expression.rhs;

    if(lhs)
        lhs -> Accept(*this);
    else
        throw std::runtime_error("Empty \"and_Expression\"");

    if(rhs){
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");
        rhs -> Accept(*this);
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");

        identifier = "";
        is_lvalue = false;
    }
}

void Analyzer::Visit(Equality_Expression& equality_expression){
    auto lhs = equality_expression.lhs;
    auto rhs = equality_expression.rhs;

    if(lhs)
        lhs -> Accept(*this);
    else
        throw std::runtime_error("Empty \"equality_Expression\"");

    if(rhs){
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");
        rhs -> Accept(*this);
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");

        identifier = "";
        is_lvalue = false;
    }
}

void Analyzer::Visit(Relational_Expression& relational_expression){
    auto lhs = relational_expression.lhs;
    auto rhs = relational_expression.rhs;

    if(lhs)
        lhs -> Accept(*this);
    else
        throw std::runtime_error("Empty \"relational_Expression\"");

    if(rhs){
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");
        rhs -> Accept(*this);
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");

        identifier = "";
        is_lvalue = false;
    }
}

void Analyzer::Visit(Additive_Expression& additive_expression){
    auto lhs = additive_expression.lhs;
    auto rhs = additive_expression.rhs;

    if(lhs)
        lhs -> Accept(*this);
    else
        throw std::runtime_error("Empty \"additive_Expression\"");

    if(rhs){
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");
        rhs -> Accept(*this);
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");

        identifier = "";
        is_lvalue = false;
    }
}

void Analyzer::Visit(Multiplicative_Expression& multiplicative_expression){
    auto lhs = multiplicative_expression.lhs;
    auto rhs = multiplicative_expression.rhs;

    if(lhs)
        lhs -> Accept(*this);
    else
        throw std::runtime_error("Empty \"multiplicative_Expression\"");

    if(rhs){
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");
        rhs -> Accept(*this);
        if(!is_convertable_to_number)
            throw std::runtime_error("cannot convert to number type");

        identifier = "";
        is_lvalue = false;
    }
}

void Analyzer::Visit(Identifier_Expression& identifier_expression){
    auto current_scope = scope_manager.scopes.top();

    bool is_visible = current_scope -> lookup(identifier_expression.value);

    if(!is_visible)
        throw std::runtime_error("variable \"" + identifier_expression.value + "\" is not declared in this scope");

    is_lvalue = true;
    is_convertable_to_number = true;
    is_string = false;
    identifier = identifier_expression.value;
}

void Analyzer::Visit(Number_Expression& number_expression){
    is_lvalue = false;
    is_convertable_to_number = true;
    is_string = false;
    identifier = "";
}

void Analyzer::Visit(CharLiteral_Expression& charliteral){
    is_lvalue = false;
    is_convertable_to_number = true;
    is_string = false;
    identifier = "";
}

void Analyzer::Visit(StringLiteral_Expression& stringLiteral){
    is_lvalue = false;
    is_convertable_to_number = false;
    is_string = true;
    identifier = "";
}

void Analyzer::Visit(Parenthesized_Expression& parenthesized_expression){
    parenthesized_expression.node -> Accept(*this);
}

void Analyzer::Visit(Unary_Expression& unary){
    unary.node -> Accept(*this);

    if(unary.postfix != EMPTY_UNARY){
        if(!is_lvalue)
            throw std::runtime_error("applying unary operator to an rvalue expression");
        
        auto var = scope_manager.scopes.top() -> symbol_table[identifier];
        if(var.type_qualifier == CONST){
            throw std::runtime_error("Applying unary operator to const variable");
        }

        is_lvalue = false;
        is_convertable_to_number = true;
        is_string = false;
        identifier = "";

        if(unary.prefix != EMPTY_UNARY)
            throw std::runtime_error("applying unary operator to an rvalue expression");

        return;
    }

    if(unary.prefix != EMPTY_UNARY){
        if(!is_lvalue)
            throw std::runtime_error("applying unary operator to an rvalue expression");

        auto var = scope_manager.scopes.top() -> symbol_table[identifier];
        if(var.type_qualifier == CONST){
            throw std::runtime_error("Applying unary operator to const variable");
        }

        is_lvalue = true;
        is_convertable_to_number = true;
        is_string = false;
    }
}

void Analyzer::Visit(FunctionCall_Expression& functionCall){
    if(functionCall.name == "scan"){
        auto arg_list = functionCall.arg_list;

        if(arg_list.size() < 1){
            throw std::runtime_error("function \"scan\" expects at least 1 argument");
        }

        int n = arg_list.size();

        for(int i = 1; i < n; ++i){
            arg_list[i] -> Accept(*this);

            if(!is_lvalue){
                throw std::runtime_error("\"scan\" expects lvalue arguments");
            }
        }

        is_lvalue = false;
        is_convertable_to_number = false;
        is_string = false;

        return;
    }

    if(functionCall.name == "print"){
        auto arg_list = functionCall.arg_list;

        if(arg_list.size() < 1){
            throw std::runtime_error("function \"print\" expects at least 1 argument");
        }

        arg_list[0] -> Accept(*this);

        if(!is_string){
            throw std::runtime_error("function \"print\" expects string literal as first argument");
        }

        int n = arg_list.size();

        for(int i = 1; i < n; ++i){
            arg_list[i] -> Accept(*this);

            if(!is_convertable_to_number){
                throw std::runtime_error("cannot convert to number argument if \"print\" function");
            }
        }

        is_lvalue = false;
        is_convertable_to_number = false;
        is_string = false;

        return;
    }



    if(!function_dictionaty.contains(functionCall.name))
        throw std::runtime_error("calling a not declared function");

    auto function = function_dictionaty[functionCall.name];

    auto arg_list = functionCall.arg_list;
    auto parameter_list = function_dictionaty[functionCall.name] -> Parameter_List; 

    if(arg_list.size() < parameter_list.size())
        throw std::runtime_error("too few arguments passed to function \"" + functionCall.name + "\"");
    
    if(arg_list.size() > parameter_list.size())
        throw std::runtime_error("too many arguments passed to function \"" + functionCall.name + "\"");

    int i = 0;
    for(auto arg : arg_list){
        arg -> Accept(*this);
        if((is_convertable_to_number && !Is_Number_Type(parameter_list[i].t)) || (is_string && Is_Number_Type(parameter_list[i].t)))
            throw std::runtime_error("invalid type of argument passed to function \"" + functionCall.name + "\"");
        ++i;
    }

    is_lvalue = false;
    if(function -> return_type.name == "void"){
        is_convertable_to_number = false;
        is_string = false;
    }else{
        is_convertable_to_number = true;
        is_string = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Executer::execute(){
    for(auto el : function_table){
        if(el.first == "main"){
            auto main_function = el.second;
            scope_manager.enterScope(Scope(FUNCTION_SCOPE, scope_manager.scopes.top()));
            main_function -> Accept(*this);
        }
    }
}


Executer::Executer(std::vector<std::pair<std::optional<Variable_Declaration>, std::optional<FunctionDefinition>>> global){
    for(auto decl : global){
        auto Possible_VarDecl = decl.first;
        auto Possible_Function = decl.second;

        if(Possible_VarDecl.has_value()){
            Variable_Declaration var_decl = Possible_VarDecl.value();

            Variable new_var(var_decl.type_qualifier, var_decl.type_specifyer, var_decl.name);

            if(var_decl.assign_expression){
                var_decl.assign_expression -> Accept(*this);

                BuiltinType type = var_decl.type_specifyer;

                if(type.name == "bool"){
                    new_var.value = (bool)current_numeric_result;
                }

                if(type.name == "char"){
                    new_var.value = (char)current_numeric_result;
                }

                if(type.name == "int"){
                    new_var.value = (int)current_numeric_result;
                }

                if(type.name == "long"){
                    new_var.value = (long)current_numeric_result;
                }

                if(type.name == "long long"){
                    new_var.value = (long long)current_numeric_result;
                }

                if(type.name == "float"){
                    new_var.value = (float)current_numeric_result;
                }

                if(type.name == "double"){
                    new_var.value = (double)current_numeric_result;
                }

                if(type.name == "long double"){
                    new_var.value = (long double)current_numeric_result;
                }
            }
            global_scope.add(var_decl.name, new_var);
            continue;
        }

        if(Possible_Function.has_value()){
            function_table[Possible_Function.value().name] = std::make_shared<FunctionDefinition>(Possible_Function.value());
            continue;
        }

        throw std::runtime_error("runtime error");
    }


    scope_manager.enterScope(global_scope);
}


void Executer::Visit(FunctionDefinition& function){
    function.body -> Accept(*this);
    return_flag = false;
    scope_manager.exitScope();
}

void Executer::Visit(Variable_Declaration& var_decl){
    if(var_decl.assign_expression){
        var_decl.assign_expression -> Accept(*this);
    }

    BuiltinType type = var_decl.type_specifyer;
    Variable new_var(var_decl.type_qualifier, type, var_decl.name);

    if(type.name == "bool"){
        new_var.value = (bool)current_numeric_result;
    }

    if(type.name == "char"){
        new_var.value = (char)current_numeric_result;
    }

    if(type.name == "int"){
        new_var.value = (int)current_numeric_result;
    }

    if(type.name == "long"){
        new_var.value = (long)current_numeric_result;
    }

    if(type.name == "long long"){
        new_var.value = (long long)current_numeric_result;
    }

    if(type.name == "float"){
        new_var.value = (float)current_numeric_result;
    }

    if(type.name == "double"){
        new_var.value = (double)current_numeric_result;
    }

    if(type.name == "long double"){
        new_var.value = (long double)current_numeric_result;
    }

    scope_manager.scopes.top() -> add(var_decl.name, new_var);
}

void Executer::Visit(Compound_Statement& comp_st){
    scope_manager.enterScope(Scope(REGULAR_SCOPE, scope_manager.scopes.top()));
    for(auto st : comp_st.statements){
        st -> Accept(*this);
        if(return_flag || break_flag || continue_flag)
            break;
    }
    scope_manager.exitScope();
}

void Executer::Visit(Expression_Statement& expression_statement){
    if(expression_statement.expression)
        expression_statement.expression -> Accept(*this);
}

void Executer::Visit(Selection_Statement& selection_statement){
    selection_statement.if_expression -> Accept(*this);

    if((bool)current_numeric_result){
        scope_manager.enterScope(Scope(IF_SCOPE, scope_manager.scopes.top()));
        selection_statement.if_statement -> Accept(*this);
        scope_manager.exitScope();
    }else{
        if(selection_statement.else_statement){
            scope_manager.enterScope(Scope(ELSE_SCOPE, scope_manager.scopes.top()));
            selection_statement.else_statement -> Accept(*this);
            scope_manager.exitScope();
        }
    }
}

void Executer::Visit(Iteration_Statement& it_st){
    scope_manager.enterScope(Scope(WHILE_SCOPE, scope_manager.scopes.top()));
    
    it_st.while_expression -> Accept(*this);

    while((bool)current_numeric_result && !return_flag && !break_flag){
        it_st.while_statement -> Accept(*this);
        continue_flag = false;
        it_st.while_expression -> Accept(*this);
    }
    if(break_flag) 
        break_flag = false;

    scope_manager.exitScope();
}

void Executer::Visit(Break_Jump& break_jump){
    break_flag = true;
}

void Executer::Visit(Continue_Jump& continue_jump){
    continue_flag = true;
}

void Executer::Visit(Return_Jump& return_jump){
    if(return_jump.return_expression){
        return_jump.return_expression -> Accept(*this);
    }
    return_flag = true;
}

void Executer::Visit(Declaration_Statement& decl_st){
    auto var = decl_st.var_decl;

    var -> Accept(*this);
}


void Executer::Visit(Assignment_Expression& assign_expression){
    if(assign_expression.rhs){
        assign_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;

        if(assign_expression.lhs)
            assign_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs value for assign expression");

        current_string_result = "";

        auto current_scope = scope_manager.scopes.top();

        while(!current_scope -> symbol_table.contains(identifier_name)){
            current_scope = current_scope -> parent;
        }

        Assign_Op op = assign_expression.op;

        if(op == ASSIGN){
            BuiltinType type = current_scope -> symbol_table[identifier_name].type;
            
            if(type.name == "bool"){
                current_scope -> symbol_table[identifier_name].value = (bool)rhs;
            }

            if(type.name == "char"){
                current_scope -> symbol_table[identifier_name].value = (char)rhs;
            }

            if(type.name == "int"){
                current_scope -> symbol_table[identifier_name].value = (int)rhs;
            }

            if(type.name == "long"){
                current_scope -> symbol_table[identifier_name].value = (long)rhs;
            }

            if(type.name == "long long"){
                current_scope -> symbol_table[identifier_name].value = (long long)rhs;
            }

            if(type.name == "float"){
                current_scope -> symbol_table[identifier_name].value = (float)rhs;
            }

            if(type.name == "double"){
                current_scope -> symbol_table[identifier_name].value = (double)rhs;
            }

            if(type.name == "long double"){
                current_scope -> symbol_table[identifier_name].value = (long double)rhs;
            }

            current_numeric_result = rhs;
            return;
        }

        //other assigns
    }else{
        if(assign_expression.lhs){
            assign_expression.lhs -> Accept(*this);
        }
    }
}

void Executer::Visit(Or_Expression& or_expression){
    if(or_expression.rhs){
        or_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;

        if(or_expression.lhs)
            or_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs for \"or\" expression");


        identifier_name = "";
        current_string_result = "";
        long double lhs = current_numeric_result;
        current_numeric_result = (bool)lhs || (bool)rhs;
    }else{
        if(or_expression.lhs)
            or_expression.lhs -> Accept(*this);
    }
}

void Executer::Visit(And_Expression& and_expression){
    if(and_expression.rhs){
        and_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;

        if(and_expression.lhs)
            and_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs for \"and\" expression");

        identifier_name = "";
        current_string_result = "";
        long double lhs = current_numeric_result;
        current_numeric_result = (bool)lhs && (bool)rhs;
    }else{
        if(and_expression.lhs)
            and_expression.lhs -> Accept(*this);
    }
}

void Executer::Visit(Equality_Expression& eq_expression){
    if(eq_expression.rhs){
        eq_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;

        if(eq_expression.lhs)
            eq_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs for \"equality\" expression");

        identifier_name = "";
        current_string_result = "";
        long double lhs = current_numeric_result;

        if(eq_expression.op == EQUAL)
            current_numeric_result = lhs == rhs;
        else if(eq_expression.op == NOT_EQUAL)
            current_numeric_result = lhs != rhs;
    }else{
        if(eq_expression.lhs)
            eq_expression.lhs -> Accept(*this);
    }
}

void Executer::Visit(Relational_Expression& rel_expression){
    if(rel_expression.rhs){
        rel_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;

        if(rel_expression.lhs)
            rel_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs for \"relational\" expression");
        
        identifier_name = "";
        current_string_result = "";
        long double lhs = current_numeric_result;

        Rel_Op op = rel_expression.op;

        if(op == GREATER){
            current_numeric_result = lhs > rhs;
        }

        if(op == LESS){
            current_numeric_result = lhs < rhs;
        }

        if(op == GREATER_OR_EQUAL){
            current_numeric_result = lhs >= rhs;
        }

        if(op == LESS_OR_EQUAL){
            current_numeric_result = lhs <= rhs;
        }
    }else if(rel_expression.lhs){
        rel_expression.lhs -> Accept(*this);
    }
}

void Executer::Visit(Additive_Expression& add_expression){
    if(add_expression.rhs){
        add_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;

        if(add_expression.lhs)
            add_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs for \"addition\" expression");
        
        identifier_name = "";
        current_string_result = "";
        long double lhs = current_numeric_result;

        Add_Op op = add_expression.op;

        if(op == PLUS){
            current_numeric_result = rhs + lhs;
        }

        if(op == MINUS){
            current_numeric_result = lhs - rhs;
        }
    }else if(add_expression.lhs){
        add_expression.lhs -> Accept(*this);
    }
}

void Executer::Visit(Multiplicative_Expression& mul_expression){
    if(mul_expression.rhs){
        bool is_float;
        mul_expression.rhs -> Accept(*this);
        long double rhs = current_numeric_result;
        is_float = floating_point;
        if(mul_expression.lhs)
            mul_expression.lhs -> Accept(*this);
        else
            throw std::runtime_error("no lhs for \"multiplicative\" expression");
        
        is_float = is_float || floating_point;

        identifier_name = "";
        current_string_result = "";
        long double lhs = current_numeric_result;

        Mul_Op op = mul_expression.op;

        if(op == MULTIPLICATION){
            current_numeric_result = rhs * lhs;
        }
        if(op == DIVISION){
            if(rhs == 0)
                throw std::runtime_error("division by zero");
            if(!is_float)
                current_numeric_result = (int)lhs / (int)rhs;
            else    
                current_numeric_result = lhs / rhs;
        }
        if(op == MODULO){
            if(rhs == 0)
                throw std::runtime_error("division by zero");

            if(is_float)
                throw std::runtime_error("operands are non integer type"); 

            floating_point = false;
            current_numeric_result = (int)lhs % (int)rhs;
        }
    }else if(mul_expression.lhs){
        mul_expression.lhs -> Accept(*this);
    }
}

void Executer::Visit(Identifier_Expression& ident_expression){
    current_string_result = "";
    identifier_name = ident_expression.value;

    auto current_scope = scope_manager.scopes.top();

    while(!(current_scope -> symbol_table.contains(identifier_name))){
        current_scope = current_scope -> parent;
    }

    std::any val = current_scope -> symbol_table[identifier_name].value;
    BuiltinType type = current_scope -> symbol_table[identifier_name].type;

    if(type.name == "char"){
        floating_point = false;
        current_numeric_result = std::any_cast<char>(val);
    }

    if(type.name == "bool"){
        floating_point = false;
        current_numeric_result = std::any_cast<bool>(val);
    }

    if(type.name == "int"){
        floating_point = false;
        current_numeric_result = std::any_cast<int>(val);
    }

    if(type.name == "long"){
        floating_point = false;
        current_numeric_result = std::any_cast<long>(val);
    }

    if(type.name == "long long"){
        floating_point = false;
        current_numeric_result = std::any_cast<long long>(val);
    }

    if(type.name == "float"){
        floating_point = true;
        current_numeric_result = std::any_cast<float>(val);
    }

    if(type.name == "double"){
        floating_point = true;
        current_numeric_result = std::any_cast<double>(val);
    }

    if(type.name == "long double"){
        floating_point = true;
        current_numeric_result = std::any_cast<long double>(val);
    }
}

void Executer::Visit(Number_Expression& num_expression){
    current_string_result = "";
    identifier_name = "";
    if(num_expression.name.contains('.')){
        floating_point = true;
        current_numeric_result = std::stod(num_expression.name);
    }else{
        floating_point = false;
        current_numeric_result = std::stoi(num_expression.name);
    }
}

void Executer::Visit(CharLiteral_Expression& char_expression){
    current_numeric_result = char_expression.character;
    current_string_result = "";
    identifier_name = "";
    floating_point = false;
}

void Executer::Visit(StringLiteral_Expression& str_expression){
    floating_point = false;
    current_string_result = str_expression.value;
    identifier_name = "";
}

void Executer::Visit(Parenthesized_Expression& par_expression){
    par_expression.node -> Accept(*this);
}

void Executer::Visit(Unary_Expression& unary_expression){
    unary_expression.node -> Accept(*this);

    Unary_Op postfix = unary_expression.postfix;
    Unary_Op prefix = unary_expression.prefix;

    if(postfix != EMPTY_UNARY || prefix != EMPTY_UNARY){
        auto current_scope = scope_manager.scopes.top();
        std::string function_name = scope_manager.scopes.top() -> function_name;

        while(!(current_scope -> symbol_table.contains(identifier_name))){
            current_scope = current_scope -> parent;
        }

        BuiltinType type = current_scope -> symbol_table[identifier_name].type;
        std::any val = current_scope -> symbol_table[identifier_name].value;

        if(postfix != EMPTY_UNARY){
            if(postfix == INC){
                std::any inc_value;

                if(type.name == "char"){
                    inc_value = (char)(std::any_cast<char>(val) + 1);
                    current_numeric_result = std::any_cast<char>(val);
                }

                if(type.name == "bool"){
                    inc_value = (bool)(std::any_cast<bool>(val) + 1);
                    current_numeric_result = std::any_cast<bool>(val);
                }

                if(type.name == "int"){
                    inc_value = (int)(std::any_cast<int>(val) + 1);
                    current_numeric_result = std::any_cast<int>(val);
                }

                if(type.name == "long"){
                    inc_value = (long)(std::any_cast<long>(val) + 1);
                    current_numeric_result = std::any_cast<long>(val);
                }

                if(type.name == "long long"){
                    inc_value = (long long)(std::any_cast<long long>(val) + 1);
                    current_numeric_result = std::any_cast<long long>(val);
                }

                if(type.name == "float"){
                    inc_value = (float)(std::any_cast<float>(val) + 1);
                    current_numeric_result = std::any_cast<float>(val);
                }

                if(type.name == "double"){
                    inc_value = (double)(std::any_cast<double>(val) + 1);
                    current_numeric_result = std::any_cast<double>(val);
                }

                if(type.name == "long double"){
                    inc_value = (long double)(std::any_cast<long double>(val) + 1);
                    current_numeric_result = std::any_cast<long double>(val);
                }

                current_scope -> symbol_table[identifier_name].value = inc_value;
                identifier_name = "";
                return;
            }
            if(postfix == DEC){
                std::any dec_value;

                if(type.name == "char"){
                    dec_value = (char)(std::any_cast<char>(val) - 1);
                    current_numeric_result = std::any_cast<char>(val);
                }

                if(type.name == "bool"){
                    dec_value = (bool)(std::any_cast<bool>(val) - 1);
                    current_numeric_result = std::any_cast<bool>(val);
                }

                if(type.name == "int"){
                    dec_value = (int)(std::any_cast<int>(val) - 1);
                    current_numeric_result = std::any_cast<int>(val);
                }

                if(type.name == "long"){
                    dec_value = (long)(std::any_cast<long>(val) - 1);
                    current_numeric_result = std::any_cast<long>(val);
                }

                if(type.name == "long long"){
                    dec_value = (long long)(std::any_cast<long long>(val) - 1);
                    current_numeric_result = std::any_cast<long long>(val);
                }

                if(type.name == "float"){
                    dec_value = (float)(std::any_cast<float>(val) - 1);
                    current_numeric_result = std::any_cast<float>(val);
                }

                if(type.name == "double"){
                    dec_value = (double)(std::any_cast<double>(val) - 1);
                    current_numeric_result = std::any_cast<double>(val);
                }

                if(type.name == "long double"){
                    dec_value = (long double)(std::any_cast<long double>(val) - 1);
                    current_numeric_result = std::any_cast<long double>(val);
                }

                current_scope -> symbol_table[identifier_name].value = dec_value;
                identifier_name = "";
                return;
            }
        }else if(prefix != EMPTY_UNARY){
            if(prefix == INC){
                std::any inc_value;

                if(type.name == "char"){
                    inc_value = (char)(std::any_cast<char>(val) + 1);
                    current_numeric_result = std::any_cast<char>(val);
                }

                if(type.name == "bool"){
                    inc_value = (bool)(std::any_cast<bool>(val) + 1);
                    current_numeric_result = std::any_cast<bool>(val) + 1;
                }

                if(type.name == "int"){
                    inc_value = (int)(std::any_cast<int>(val) + 1);
                    current_numeric_result = std::any_cast<int>(val) + 1;
                }

                if(type.name == "long"){
                    inc_value = (long)(std::any_cast<long>(val) + 1);
                    current_numeric_result = std::any_cast<long>(val) + 1;
                }

                if(type.name == "long long"){
                    inc_value = (long long)(std::any_cast<long long>(val) + 1);
                    current_numeric_result = std::any_cast<long long>(val) + 1;
                }

                if(type.name == "float"){
                    inc_value = (float)(std::any_cast<float>(val) + 1);
                    current_numeric_result = std::any_cast<float>(val);
                }

                if(type.name == "double"){
                    inc_value = (double)(std::any_cast<double>(val) + 1);
                    current_numeric_result = std::any_cast<double>(val) + 1;
                }

                if(type.name == "long double"){
                    inc_value = (long double)(std::any_cast<long double>(val) + 1);
                    current_numeric_result = std::any_cast<long double>(val) + 1;
                }

                current_scope -> symbol_table[identifier_name].value = inc_value;
                return;
            }
            if(prefix == DEC){
                std::any dec_value;

                if(type.name == "char"){
                    dec_value = (char)(std::any_cast<char>(val) - 1);
                    current_numeric_result = std::any_cast<char>(val) - 1;
                }

                if(type.name == "bool"){
                    dec_value = (bool)(std::any_cast<bool>(val) - 1);
                    current_numeric_result = std::any_cast<bool>(val) - 1;
                }

                if(type.name == "int"){
                    dec_value = (int)(std::any_cast<int>(val) - 1);
                    current_numeric_result = std::any_cast<int>(val) - 1;
                }

                if(type.name == "long"){
                    dec_value = (long)(std::any_cast<long>(val) - 1);
                    current_numeric_result = std::any_cast<long>(val) - 1;
                }

                if(type.name == "long long"){
                    dec_value = (long long)(std::any_cast<long long>(val) - 1);
                    current_numeric_result = std::any_cast<long long>(val) - 1;
                }

                if(type.name == "float"){
                    dec_value = (float)(std::any_cast<float>(val) - 1);
                    current_numeric_result = std::any_cast<float>(val) - 1;
                }

                if(type.name == "double"){
                    dec_value = (double)(std::any_cast<double>(val) - 1);
                    current_numeric_result = std::any_cast<double>(val) - 1;
                }

                if(type.name == "long double"){
                    dec_value = (long double)(std::any_cast<long double>(val) - 1);
                    current_numeric_result = std::any_cast<long double>(val) - 1;
                }

                current_scope -> symbol_table[identifier_name].value = dec_value;
                return;
            }
            if(prefix == UNARY_MINUS){
                std::any minus_val;

                if(type.name == "char"){
                    minus_val = (char)(-std::any_cast<char>(val));
                    current_numeric_result = -(std::any_cast<char>(val));
                }

                if(type.name == "bool"){
                    minus_val = (bool)(-std::any_cast<bool>(val));
                    current_numeric_result = -(std::any_cast<bool>(val));
                }

                if(type.name == "int"){
                    minus_val = (int)(-std::any_cast<int>(val));
                    current_numeric_result = -(std::any_cast<int>(val));
                }

                if(type.name == "long"){
                    minus_val = (long)(-std::any_cast<long>(val));
                    current_numeric_result = -(std::any_cast<long>(val));
                }

                if(type.name == "long long"){
                    minus_val = (long long)(-std::any_cast<long long>(val));
                    current_numeric_result = -(std::any_cast<long long>(val));
                }

                if(type.name == "float"){
                    minus_val = (float)(-std::any_cast<float>(val));
                    current_numeric_result = -(std::any_cast<float>(val));
                }

                if(type.name == "double"){
                    minus_val = (double)(-std::any_cast<double>(val));
                    current_numeric_result = -(std::any_cast<double>(val));
                }

                if(type.name == "long double"){
                    minus_val = (long double)(-std::any_cast<long double>(val));
                    current_numeric_result = -(std::any_cast<long double>(val));
                }
        
                current_scope -> symbol_table[identifier_name].value = minus_val;
                return;
            }
        }
    }
}


void Executer::Visit(FunctionCall_Expression& call_expression){
    if(call_expression.name == "print"){
        print(call_expression);
        return;
    }

    if(call_expression.name == "scan"){
        scan(call_expression);
        return;
    }

    auto callee_function = function_table[call_expression.name];

    if(callee_function -> void_flag){
        ScopeManager save_scope_manager = scope_manager;
        scope_manager = ScopeManager();
        scope_manager.enterScope(global_scope);
        callee_function -> Accept(*this);
        scope_manager = save_scope_manager;
        return;
    }

    ScopeManager new_scope_manager = ScopeManager();
    new_scope_manager.enterScope(global_scope);
    new_scope_manager.enterScope(Scope(FUNCTION_SCOPE, scope_manager.scopes.top(), call_expression.name));

    auto param_list = callee_function -> Parameter_List;
    
    auto arg_list = call_expression.arg_list;

    int i = 0;
    for(auto parameter : param_list){
        Variable func_var(Type_Qualifier(), parameter.t, parameter.name);
        arg_list[i] -> Accept(*this);

        if(!current_string_result.empty()){
            throw std::runtime_error("no pointers yet");
        }

        BuiltinType type = parameter.t;

        if(type.name == "char"){
            func_var.value = (char)current_numeric_result;
        }

        if(type.name == "bool"){
            func_var.value = (bool)current_numeric_result;
        }

        if(type.name == "int"){
            func_var.value = (int)current_numeric_result;
        }

        if(type.name == "long"){
            func_var.value = (long)current_numeric_result;
        }

        if(type.name == "long long"){
            func_var.value = (long long)current_numeric_result;
        }

        if(type.name == "float"){
            func_var.value = (float)current_numeric_result;
        }

        if(type.name == "double"){
            func_var.value = (double)current_numeric_result;
        }

        if(type.name == "long double"){
            func_var.value = (long double)current_numeric_result;
        }

        new_scope_manager.scopes.top() -> add(parameter.name, func_var);

        ++i;
    }

    ScopeManager save = scope_manager;
    scope_manager = new_scope_manager;

    callee_function -> Accept(*this);

    scope_manager = save;
}

void Executer::print(FunctionCall_Expression& call_expression){
    auto arg_list = call_expression.arg_list;

    if(arg_list.size() < 1){
        throw std::runtime_error("function \"print\" expects at least 1 argument");
    }

    arg_list[0] -> Accept(*this);

    if(current_string_result.empty())
        throw std::runtime_error("function \"print\" expects string as first argument");

    std::string print_str = current_string_result;

    int i = 1;
    char c;
    for(int j = 0; j < print_str.length(); ++j){
        c = print_str[j];

        if(c == '%'){
            ++j;
            if(j == print_str.length())
                throw std::runtime_error("expected specifyer for \"print\" funtion");

            c = print_str[j];

            if(i >= arg_list.size()){
                throw std::runtime_error("too few arguments passed to \"print\" function");
            }

            arg_list[i] -> Accept(*this);
            ++i;

            if(!current_string_result.empty())
                throw std::runtime_error("\"print\" can only have numeric type arguments");

            if(c == 'd'){
                std::cout << (int)current_numeric_result;
                continue;
            }

            if(c == 'l'){
                ++j;
                if(j >= print_str.length()){
                    std::cout << (long)current_numeric_result;
                    continue;
                }

                c = print_str[j];
                if(c == 'd'){
                    std::cout << (long double)current_numeric_result;
                    continue;
                }

                if(c == 'l'){
                    std::cout << (long long)current_numeric_result;
                    continue;
                }
                
                if(c == 'f'){
                    std::cout << (double)current_numeric_result;
                    continue;
                }

                std::cout << (long)current_numeric_result;
                --j;
                continue;
            }

            if(c == 'f'){
                std::cout << (float)current_numeric_result;
                continue;
            }

            if(c == 'c'){
                std::cout << (char)current_numeric_result;
                continue;
            }

            throw std::runtime_error("Unknown specifyier in function \"print\"");
        }else{
            if(c == '\\'){
                ++j;
                if(j == print_str.length())
                    throw std::runtime_error("expected symbol after \"\\\" for first argument of \"print\" function");

                c = print_str[j];

                if(c == 'n')
                    std::cout << std::endl;
                else
                    std::cout << c;

                continue;
            }
            std::cout << c;
            continue;
        }
    }

    if(i < arg_list.size())
        throw std::runtime_error("too many arguments passed to \"print\" function");
}

void Executer::scan(FunctionCall_Expression& call_expression){
    auto arg_list = call_expression.arg_list;

    int n = arg_list.size();

    for(int i = 0; i < n; ++i){
        arg_list[i] -> Accept(*this);

        auto current_scope = scope_manager.scopes.top();

        while(!current_scope -> symbol_table.contains(identifier_name) && current_scope){
            current_scope = current_scope -> parent;
        }

        auto& var = current_scope -> symbol_table[identifier_name];

        long double tmp;

        std::cin >> tmp;

        auto type = var.type;

        if(type.name == "char"){
            var.value = (char)tmp;
        }

        if(type.name == "bool"){
            var.value = (bool)tmp;
        }

        if(type.name == "int"){
            var.value = (int)tmp;
        }

        if(type.name == "long"){
            var.value = (long)tmp;
        }

        if(type.name == "long long"){
            var.value = (long long)tmp;
        }

        if(type.name == "float"){
            var.value = (float)tmp;
        }

        if(type.name == "double"){
            var.value = (double)tmp;
        }

        if(type.name == "long double"){
            var.value = (long double)tmp;
        }
    }
}