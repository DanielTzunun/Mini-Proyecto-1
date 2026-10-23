/* ast.h - Nodos del AST de Nova */
#ifndef AST_H
#define AST_H

/* Tipos escalares y tipos de arreglo (un arreglo por cada tipo escalar). */
typedef enum {
    TIPO_ENTERO,
    TIPO_MENSAJE,
    TIPO_SIGNO,
    TIPO_VERACIDAD,
    TIPO_ARREGLO_ENTERO,
    TIPO_ARREGLO_MENSAJE,
    TIPO_ARREGLO_SIGNO,
    TIPO_ARREGLO_VERACIDAD,
    TIPO_DESCONOCIDO
} Tipo;

const char* nombre_tipo(Tipo t);
int         es_tipo_arreglo(Tipo t);
Tipo        tipo_elemento(Tipo t);
Tipo        tipo_arreglo(Tipo elemento);

/* Expresion: literal, identificador, comparacion, operacion logica,
 * literal de arreglo o acceso a un elemento por indice. */
typedef enum {
    EXPR_ENTERO, EXPR_MENSAJE, EXPR_SIGNO, EXPR_VERACIDAD, EXPR_IDENT,
    EXPR_COMPARACION, EXPR_OR, EXPR_ARREGLO, EXPR_INDICE
} ClaseExpr;

typedef struct Expr {
    ClaseExpr clase;
    char*     texto;
    int       linea;
    Tipo      tipo;   /* los literales lo traen; otros nodos lo recibe del semantico */
    struct Expr* izquierda;
    struct Expr* derecha;
    struct Expr* siguiente; /* encadena los elementos de un literal de arreglo */
} Expr;

/* Sentencia: declaracion, asignacion, funcion, llamada, condicional,
 * agregar un valor a un arreglo o quitar el ultimo valor de un arreglo. */
typedef enum {
    SENT_DECLARACION,
    SENT_ASIGNACION,
    SENT_FUNCION,
    SENT_LLAMADA,
    SENT_CONDICIONAL,
    SENT_MOSTRAR,
    SENT_AGREGAR,
    SENT_QUITAR
} ClaseSent;

typedef struct Sentencia {
    ClaseSent clase;
    int       es_constante;   /* solo declaraciones */
    Tipo      tipo;           /* tipo declarado (solo declaraciones) */
    char*     id;             /* variable declarada, destino de guardar o arreglo */
    Expr*     valor;          /* NULL en "variable:entero x;" */
    struct Sentencia* cuerpo; /* lista de instrucciones de una funcion */
    struct Sentencia* alternativa; /* rama else de un condicional */
    int       linea;
    struct Sentencia* siguiente;   /* lista enlazada: la raiz Programa es esta lista */
} Sentencia;

extern Sentencia* programa_inicio;

Expr*      nueva_expr(ClaseExpr clase, char* texto, int linea, Tipo tipo);
Sentencia* nueva_sentencia(ClaseSent clase, int es_constante, Tipo tipo,
                           char* id, Expr* valor, int linea);
Sentencia* nueva_funcion(char* id, Sentencia* cuerpo, int linea);
Sentencia* nueva_llamada(char* id, int linea);
Expr*      nueva_expr_binaria(ClaseExpr clase, char* operador, Expr* izquierda,
                              Expr* derecha, int linea);
Sentencia* nueva_condicional(Expr* condicion, Sentencia* cuerpo,
                             Sentencia* alternativa, int linea);

Expr*      nueva_expr_arreglo(Expr* elementos, int linea);
Expr*      nueva_expr_indice(Expr* arreglo, Expr* indice, int linea);
Expr*      anexar_expresion(Expr* inicio, Expr* nueva);
Sentencia* nueva_agregar(char* id, Expr* valor, int linea);
Sentencia* nueva_quitar(char* id, int linea);
Sentencia* nueva_mostrar(Expr* valor, int linea);

void       agregar_sentencia(Sentencia* s);

void       imprimir_arbol(void);        /* AST en forma de arbol */
void       imprimir_tabla_ast(void);    /* AST en forma de tabla */

#endif
