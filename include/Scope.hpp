#pragma once

#include <stack>
#include <unordered_map>
#include <string>
#include "Type.hpp"
#include <memory>
#include <any>

enum Scope_Type {
    FUNCTION_SCOPE,
    WHILE_SCOPE,
    IF_SCOPE,
    ELSE_SCOPE,
    REGULAR_SCOPE
};

struct Variable {
    Variable() = default;
    Variable(Type_Qualifier t_q, const BuiltinType& type, const std::string& name,  const std::any& value = 0, bool const_decl = false) : type(type), value(value), type_qualifier(t_q), const_declaration(const_decl) {}

    Type_Qualifier type_qualifier;
    BuiltinType type;
    std::string name;
    std::any value;
    bool const_declaration = false;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Scope {
    Scope_Type type;
    std::unordered_map<std::string, Variable> symbol_table;
    std::shared_ptr<Scope> parent;
    std::string function_name = "";

    Scope(Scope_Type type, std::shared_ptr<Scope> parent, std::string function_name) : type(type), parent(parent), function_name(function_name) {}
    Scope(Scope_Type type = REGULAR_SCOPE,  std::shared_ptr<Scope> parent = nullptr) : type(type), parent(parent) {}

     void add(const std::string& name, const Variable& symbol) {
        if(symbol_table.contains(name)) {
            throw std::runtime_error("Redeclaration of symbol " + name + ".");
        }
        symbol_table[name] = symbol;
    }

    bool lookup(const std::string& name) {
        if (!symbol_table.contains(name)) {
            if (parent == nullptr) {
                return false;
            }
            return parent->lookup(name);
        }
        return true;
    }

};

struct ScopeManager {
    std::stack<std::shared_ptr<Scope>> scopes;

    ScopeManager() {
        scopes.push(std::make_shared<Scope>());
    }

    void enterScope(const Scope& new_scope) {
        scopes.push(std::make_shared<Scope>(new_scope));
    }

    void exitScope(){
        scopes.pop();
    }
};
