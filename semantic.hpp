// semantic.hpp - Analizador semantico de Nova
#pragma once
#include <string>
#include <vector>
#include "ast.hpp"
#include "symtab.hpp"

class AnalizadorSemantico {
public:
    explicit AnalizadorSemantico(TablaSimbolos& t) : tabla(t) {}
    void analizar(Programa& p);
    const std::vector<std::string>& errores() const { return msgs; }
private:
    TablaSimbolos& tabla;
    std::vector<std::string> msgs;
    Tipo tipoDe(Expr* e);
    void error(int linea, const std::string& msg);
};
