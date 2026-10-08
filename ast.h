/* ast.h - Nodos del AST de Nova */
#ifndef AST_H
#define AST_H

typedef enum { TIPO_ENTERO, TIPO_MENSAJE, TIPO_SIGNO, TIPO_VERACIDAD, TIPO_DESCONOCIDO } Tipo;
const char* nombre_tipo(Tipo t);

/* Expresion: literal o identificador (hoja del arbol) */
typedef enum { EXPR_ENTERO, EXPR_MENSAJE, EXPR_SIGNO, EXPR_VERACIDAD, EXPR_IDENT } ClaseExpr;
typedef struct Expr {
    ClaseExpr clase;
    char*     texto;
    int       linea;
    Tipo      tipo;   /* los literales lo traen; los identificadores lo recibe del semantico */
} Expr;

/* Sentencia: declaracion o asignacion (hijo directo de Programa) */
typedef enum { SENT_DECLARACION, SENT_ASIGNACION } ClaseSent;
typedef struct Sentencia {
    ClaseSent clase;
    int       es_constante;   /* solo declaraciones */
    Tipo      tipo;           /* tipo declarado (solo declaraciones) */
    char*     id;             /* variable declarada o destino de guardar */
    Expr*     valor;          /* NULL en "variable:entero x;" */
    int       linea;
    struct Sentencia* siguiente;   /* lista enlazada: la raiz Programa es esta lista */
} Sentencia;

extern Sentencia* programa_inicio;

char*      copiar_cadena(const char* s);
Expr*      nueva_expr(ClaseExpr clase, char* texto, int linea, Tipo tipo);
Sentencia* nueva_sentencia(ClaseSent clase, int es_constante, Tipo tipo,
                           char* id, Expr* valor, int linea);
void       agregar_sentencia(Sentencia* s);

void imprimir_arbol(void);        /* AST en forma de arbol */
void imprimir_tabla_ast(void);    /* AST en forma de tabla */

#endif
