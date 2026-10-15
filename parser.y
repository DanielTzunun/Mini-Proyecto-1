%code requires {
#include "ast.h"
}

%{
/* parser.y - Analizador sintactico de Nova (Bison) */
#include <stdio.h>
#include <string.h>
#include "ast.h"

int yylex(void);
void yyerror(const char* msg);
extern int errores_sintacticos;

static Sentencia* anexar_a_lista(Sentencia* inicio, Sentencia* nueva) {
    if (!inicio) return nueva;
    Sentencia* ultimo = inicio;
    while (ultimo->siguiente) ultimo = ultimo->siguiente;
    ultimo->siguiente = nueva;
    return inicio;
}
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
%token T_FUNCION   "funcion"
%token <tipo> T_TIPO    "tipo de dato"
%token <str>  T_ID      "identificador"
%token <str>  T_ENTERO  "numero entero"
%token <str>  T_MENSAJE "mensaje"
%token <str>  T_SIGNO   "signo"
%token <str>  T_BOOL    "verdadero/falso"

%type <expr> expresion
%type <sent> sentencia sentencia_bloque declaracion asignacion funcion llamada bloque_funcion

%%

programa
    : /* vacio */
    | programa sentencia        { if ($2) agregar_sentencia($2); }
    ;

sentencia
    : declaracion ';'           { $$ = $1; }
    | asignacion ';'            { $$ = $1; }
    | funcion                   { $$ = $1; }
    | llamada ';'               { $$ = $1; }
    | error ';'                 { yyerrok; $$ = NULL; }   /* recuperacion de errores */
    ;

sentencia_bloque
    : declaracion ';'           { $$ = $1; }
    | asignacion ';'            { $$ = $1; }
    | llamada ';'               { $$ = $1; }
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

llamada
    : T_ID '(' ')'
        { $$ = nueva_llamada($1, @1.first_line); }
    ;

declaracion
    : T_VARIABLE ':' T_TIPO T_ID
        { $$ = nueva_sentencia(SENT_DECLARACION, 0, $3, $4, NULL, @1.first_line); }
    | T_VARIABLE ':' T_TIPO T_ID '=' expresion
        { $$ = nueva_sentencia(SENT_DECLARACION, 0, $3, $4, $6, @1.first_line); }
    | T_CONSTANTE ':' T_TIPO T_ID '=' expresion
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
    ;

%%

/* Reemplaza todas las apariciones de 'a' por 'b' dentro de s */
static void reemplazar(char* s, size_t cap, const char* a, const char* b) {
    char tmp[512];
    size_t i = 0, j = 0, la = strlen(a), lb = strlen(b);
    while (s[i] && j < sizeof(tmp) - 1) {
        if (strncmp(s + i, a, la) == 0) {
            for (size_t k = 0; k < lb && j < sizeof(tmp) - 1; k++) tmp[j++] = b[k];
            i += la;
        } else tmp[j++] = s[i++];
    }
    tmp[j] = '\0';
    strncpy(s, tmp, cap - 1);
    s[cap - 1] = '\0';
}

void yyerror(const char* msg) {
    char m[512];
    snprintf(m, sizeof m, "%s", msg);
    reemplazar(m, sizeof m, "syntax error, ", "");
    reemplazar(m, sizeof m, "syntax error", "sintaxis invalida");
    reemplazar(m, sizeof m, "unexpected ", "se encontro ");
    reemplazar(m, sizeof m, ", expecting ", ", se esperaba ");
    reemplazar(m, sizeof m, " or ", " o ");
    reemplazar(m, sizeof m, "end of file", "fin del archivo");
    if (strstr(m, "se esperaba ';'") && strlen(m) < 400)
        strcat(m, " (revise si falta ';' al final de la linea anterior)");
    errores_sintacticos++;
    printf("Error sintactico (linea %d): %s\n", yylloc.first_line, m);
}
