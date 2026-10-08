// ast.hpp - Nodos del AST de Nova
#pragma once
#include <string>
#include <vector>

enum class Tipo { ENTERO, MENSAJE, SIGNO, VERACIDAD, DESCONOCIDO };
std::string nombreTipo(Tipo t);

// Expresion: literal o identificador
struct Expr {
    enum Clase { LIT_ENTERO, LIT_MENSAJE, LIT_SIGNO, LIT_VERACIDAD, IDENT } clase;
    std::string texto;
    int linea;
    Tipo tipo;   // los literales lo traen; los identificadores lo recibe el analizador semantico
};

// Sentencia: declaracion o asignacion
struct Sentencia {
    enum Clase { DECLARACION, ASIGNACION } clase;
    bool esConstante;   // solo declaraciones
    Tipo tipo;          // tipo declarado (solo declaraciones)
    std::string id;     // variable declarada o destino de guardar
    Expr* valor;        // puede ser nullptr en "variable:entero x;"
    int linea;
};

using Programa = std::vector<Sentencia*>;
extern Programa programa;

Expr* nuevaExpr(Expr::Clase c, const std::string& texto, int linea, Tipo t);
Sentencia* nuevaSentencia(Sentencia::Clase c, bool esConst, Tipo t,
                          const std::string& id, Expr* valor, int linea);

void imprimirArbol(const Programa& p);     // AST en forma de arbol
void imprimirTablaAST(const Programa& p);  // AST en forma de tabla
