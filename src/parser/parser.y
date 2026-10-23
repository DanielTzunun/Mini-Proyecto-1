%code requires {
#include "ast.h"
}

%{
/* parser.y - Analizador sintactico de Nova (Bison) */
#include "ast.h"
#include "parser_interno.h"
#include "utils.h"
%}

%locations
%define parse.error verbose

%union {
    char*      str;
    Tipo       tipo;
    Expr*      expr;
    Sentencia* sent;
}

%token T_VARIABLE  "variable"
%token T_CONSTANTE "constante"
%token T_GUARDAR   "guardar"
%token T_EN        "en"
%token T_AGREGAR   "agregar"
%token T_QUITAR    "quitar"
%token T_DE        "de"
%token T_FUNCION   "funcion"
%token T_IF        "if"
%token T_ELSE      "else"
%token T_OR        "or"
%token T_IGUAL     "=="
%token T_DISTINTO  "!="
%token T_MAYOR     ">"
%token T_MENOR     "<"
%token T_MAYOR_IGUAL ">="
%token T_MENOR_IGUAL "<="
%token <tipo> T_TIPO    "tipo de dato"
%token <str>  T_ID      "identificador"
%token <str>  T_ENTERO  "numero entero"
%token <str>  T_MENSAJE "mensaje"
%token <str>  T_SIGNO   "signo"
%token <str>  T_BOOL    "verdadero/falso"

%type <tipo> tipo_declarado
%type <expr> expresion condicion lista_expresiones
%type <sent> sentencia sentencia_bloque declaracion asignacion funcion llamada
%type <sent> bloque_funcion condicional agregar quitar

%left T_OR
%nonassoc T_IGUAL T_DISTINTO T_MAYOR T_MENOR T_MAYOR_IGUAL T_MENOR_IGUAL

%%

programa
    : /* vacio */
    | programa sentencia        { if ($2) agregar_sentencia($2); }
    ;

sentencia
    : declaracion ';'           { $$ = $1; }
    | asignacion ';'            { $$ = $1; }
    | funcion                   { $$ = $1; }
    | condicional               { $$ = $1; }
    | llamada ';'               { $$ = $1; }
    | agregar ';'               { $$ = $1; }
    | quitar ';'                { $$ = $1; }
    | error ';'                 { yyerrok; $$ = NULL; }   /* recuperacion de errores */
    ;

sentencia_bloque
    : declaracion ';'           { $$ = $1; }
    | asignacion ';'            { $$ = $1; }
    | llamada ';'               { $$ = $1; }
    | condicional               { $$ = $1; }
    | agregar ';'               { $$ = $1; }
    | quitar ';'                { $$ = $1; }
    ;

funcion
    : T_FUNCION T_ID '(' ')' '{' bloque_funcion '}'
        { $$ = nueva_funcion($2, $6, @1.first_line); }
    ;

bloque_funcion
    : /* vacio */               { $$ = NULL; }
    | bloque_funcion sentencia_bloque
                                { $$ = anexar_a_lista($1, $2); }
    ;

condicional
    : T_IF '(' condicion ')' '{' bloque_funcion '}'
        { $$ = nueva_condicional($3, $6, NULL, @1.first_line); }
    | T_IF '(' condicion ')' '{' bloque_funcion '}' T_ELSE '{' bloque_funcion '}'
        { $$ = nueva_condicional($3, $6, $10, @1.first_line); }
    ;

llamada
    : T_ID '(' ')'
        { $$ = nueva_llamada($1, @1.first_line); }
    ;

agregar
    : T_AGREGAR expresion T_EN T_ID
        { $$ = nueva_agregar($4, $2, @1.first_line); }
    ;

quitar
    : T_QUITAR T_DE T_ID
        { $$ = nueva_quitar($3, @1.first_line); }
    ;

tipo_declarado
    : T_TIPO                    { $$ = $1; }
    | T_TIPO '[' ']'            { $$ = tipo_arreglo($1); }
    ;

declaracion
    : T_VARIABLE ':' tipo_declarado T_ID
        { $$ = nueva_sentencia(SENT_DECLARACION, 0, $3, $4, NULL, @1.first_line); }
    | T_VARIABLE ':' tipo_declarado T_ID '=' expresion
        { $$ = nueva_sentencia(SENT_DECLARACION, 0, $3, $4, $6, @1.first_line); }
    | T_CONSTANTE ':' tipo_declarado T_ID '=' expresion
        { $$ = nueva_sentencia(SENT_DECLARACION, 1, $3, $4, $6, @1.first_line); }
    ;

asignacion
    : T_GUARDAR expresion T_EN T_ID
        { $$ = nueva_sentencia(SENT_ASIGNACION, 0, TIPO_DESCONOCIDO, $4, $2, @1.first_line); }
    ;

expresion
    : T_ENTERO   { $$ = nueva_expr(EXPR_ENTERO,    $1, @1.first_line, TIPO_ENTERO); }
    | T_MENSAJE  { $$ = nueva_expr(EXPR_MENSAJE,   $1, @1.first_line, TIPO_MENSAJE); }
    | T_SIGNO    { $$ = nueva_expr(EXPR_SIGNO,     $1, @1.first_line, TIPO_SIGNO); }
    | T_BOOL     { $$ = nueva_expr(EXPR_VERACIDAD, $1, @1.first_line, TIPO_VERACIDAD); }
    | T_ID       { $$ = nueva_expr(EXPR_IDENT,     $1, @1.first_line, TIPO_DESCONOCIDO); }
    | '[' ']'    { $$ = nueva_expr_arreglo(NULL, @1.first_line); }
    | '[' lista_expresiones ']'
                 { $$ = nueva_expr_arreglo($2, @1.first_line); }
    | expresion '[' expresion ']'
                 { $$ = nueva_expr_indice($1, $3, @2.first_line); }
    ;

lista_expresiones
    : expresion
                 { $$ = $1; }
    | lista_expresiones ',' expresion
                 { $$ = anexar_expresion($1, $3); }
    ;

condicion
    : expresion
        { $$ = $1; }
    | expresion T_IGUAL expresion
        { $$ = nueva_expr_binaria(EXPR_COMPARACION, copiar_cadena("=="), $1, $3, @2.first_line); }
    | expresion T_DISTINTO expresion
        { $$ = nueva_expr_binaria(EXPR_COMPARACION, copiar_cadena("!="), $1, $3, @2.first_line); }
    | expresion T_MAYOR expresion
        { $$ = nueva_expr_binaria(EXPR_COMPARACION, copiar_cadena(">"), $1, $3, @2.first_line); }
    | expresion T_MENOR expresion
        { $$ = nueva_expr_binaria(EXPR_COMPARACION, copiar_cadena("<"), $1, $3, @2.first_line); }
    | expresion T_MAYOR_IGUAL expresion
        { $$ = nueva_expr_binaria(EXPR_COMPARACION, copiar_cadena(">="), $1, $3, @2.first_line); }
    | expresion T_MENOR_IGUAL expresion
        { $$ = nueva_expr_binaria(EXPR_COMPARACION, copiar_cadena("<="), $1, $3, @2.first_line); }
    | condicion T_OR condicion
        { $$ = nueva_expr_binaria(EXPR_OR, copiar_cadena("or"), $1, $3, @2.first_line); }
    ;

%%
