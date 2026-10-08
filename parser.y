%code requires {
#include <string>
#include "ast.hpp"
}

%{
/* parser.y - Analizador sintactico de Nova (Bison) */
#include <cstdio>
#include <string>
#include "ast.hpp"

int yylex();
void yyerror(const char* msg);
extern int erroresSintacticos;
%}

%locations
%define parse.error verbose

%union {
    std::string* str;
    Tipo         tipo;
    Expr*        expr;
    Sentencia*   sent;
}

%token T_VARIABLE  "variable"
%token T_CONSTANTE "constante"
%token T_GUARDAR   "guardar"
%token T_EN        "en"
%token <tipo> T_TIPO    "tipo de dato"
%token <str>  T_ID      "identificador"
%token <str>  T_ENTERO  "numero entero"
%token <str>  T_MENSAJE "mensaje"
%token <str>  T_SIGNO   "signo"
%token <str>  T_BOOL    "verdadero/falso"

%type <expr> expresion
%type <sent> sentencia declaracion asignacion

%%

programa
    : /* vacio */
    | programa sentencia        { if ($2) programa.push_back($2); }
    ;

sentencia
    : declaracion ';'           { $$ = $1; }
    | asignacion ';'            { $$ = $1; }
    | error ';'                 { yyerrok; $$ = nullptr; }   /* recuperacion de errores */
    ;

declaracion
    : T_VARIABLE ':' T_TIPO T_ID
        { $$ = nuevaSentencia(Sentencia::DECLARACION, false, $3, *$4, nullptr, @1.first_line); }
    | T_VARIABLE ':' T_TIPO T_ID '=' expresion
        { $$ = nuevaSentencia(Sentencia::DECLARACION, false, $3, *$4, $6, @1.first_line); }
    | T_CONSTANTE ':' T_TIPO T_ID '=' expresion
        { $$ = nuevaSentencia(Sentencia::DECLARACION, true, $3, *$4, $6, @1.first_line); }
    ;

asignacion
    : T_GUARDAR expresion T_EN T_ID
        { $$ = nuevaSentencia(Sentencia::ASIGNACION, false, Tipo::DESCONOCIDO, *$4, $2, @1.first_line); }
    ;

expresion
    : T_ENTERO   { $$ = nuevaExpr(Expr::LIT_ENTERO,    *$1, @1.first_line, Tipo::ENTERO); }
    | T_MENSAJE  { $$ = nuevaExpr(Expr::LIT_MENSAJE,   *$1, @1.first_line, Tipo::MENSAJE); }
    | T_SIGNO    { $$ = nuevaExpr(Expr::LIT_SIGNO,     *$1, @1.first_line, Tipo::SIGNO); }
    | T_BOOL     { $$ = nuevaExpr(Expr::LIT_VERACIDAD, *$1, @1.first_line, Tipo::VERACIDAD); }
    | T_ID       { $$ = nuevaExpr(Expr::IDENT,         *$1, @1.first_line, Tipo::DESCONOCIDO); }
    ;

%%

static std::string reemplazar(std::string s, const std::string& a, const std::string& b) {
    size_t p = 0;
    while ((p = s.find(a, p)) != std::string::npos) { s.replace(p, a.size(), b); p += b.size(); }
    return s;
}

void yyerror(const char* msg) {
    std::string m = msg;
    m = reemplazar(m, "syntax error, ", "");
    m = reemplazar(m, "syntax error", "sintaxis invalida");
    m = reemplazar(m, "unexpected ", "se encontro ");
    m = reemplazar(m, ", expecting ", ", se esperaba ");
    m = reemplazar(m, " or ", " o ");
    m = reemplazar(m, "end of file", "fin del archivo");
    if (m.find("se esperaba ';'") != std::string::npos)
        m += " (revise si falta ';' al final de la linea anterior)";
    ++erroresSintacticos;
    printf("Error sintactico (linea %d): %s\n", yylloc.first_line, m.c_str());
}
