/* ast.c - Construccion del AST de Nova */
#include <stdlib.h>
#include "ast.h"
#include "utils.h"

Sentencia* programa_inicio = NULL;
static Sentencia* programa_fin = NULL;

Expr* nueva_expr(ClaseExpr clase, char* texto, int linea, Tipo tipo) {
    Expr* e = malloc(sizeof(Expr));
    e->clase = clase;
    e->texto = texto;
    e->linea = linea;
    e->tipo = tipo;
    e->izquierda = NULL;
    e->derecha = NULL;
    e->siguiente = NULL;
    return e;
}

Expr* nueva_expr_binaria(ClaseExpr clase, char* operador, Expr* izquierda,
                         Expr* derecha, int linea) {
    Expr* e = nueva_expr(clase, operador, linea, TIPO_DESCONOCIDO);
    e->izquierda = izquierda;
    e->derecha = derecha;
    return e;
}

Expr* nueva_expr_arreglo(Expr* elementos, int linea) {
    Expr* e = nueva_expr(EXPR_ARREGLO, copiar_cadena("[]"), linea, TIPO_DESCONOCIDO);
    e->izquierda = elementos;
    return e;
}

Expr* nueva_expr_indice(Expr* arreglo, Expr* indice, int linea) {
    Expr* e = nueva_expr(EXPR_INDICE, copiar_cadena("[]"), linea, TIPO_DESCONOCIDO);
    e->izquierda = arreglo;
    e->derecha = indice;
    return e;
}

Expr* anexar_expresion(Expr* inicio, Expr* nueva) {
    if (!inicio) return nueva;
    Expr* ultimo = inicio;
    while (ultimo->siguiente) ultimo = ultimo->siguiente;
    ultimo->siguiente = nueva;
    return inicio;
}

Sentencia* nueva_sentencia(ClaseSent clase, int es_constante, Tipo tipo,
                           char* id, Expr* valor, int linea) {
    Sentencia* s = malloc(sizeof(Sentencia));
    s->clase = clase; s->es_constante = es_constante; s->tipo = tipo;
    s->id = id; s->valor = valor; s->cuerpo = NULL; s->alternativa = NULL;
    s->linea = linea; s->siguiente = NULL;
    return s;
}

Sentencia* nueva_funcion(char* id, Sentencia* cuerpo, int linea) {
    Sentencia* s = nueva_sentencia(SENT_FUNCION, 0, TIPO_DESCONOCIDO, id, NULL, linea);
    s->cuerpo = cuerpo;
    return s;
}

Sentencia* nueva_llamada(char* id, int linea) {
    return nueva_sentencia(SENT_LLAMADA, 0, TIPO_DESCONOCIDO, id, NULL, linea);
}

Sentencia* nueva_condicional(Expr* condicion, Sentencia* cuerpo,
                             Sentencia* alternativa, int linea) {
    Sentencia* s = nueva_sentencia(SENT_CONDICIONAL, 0, TIPO_DESCONOCIDO,
                                   NULL, condicion, linea);
    s->cuerpo = cuerpo;
    s->alternativa = alternativa;
    return s;
}

Sentencia* nueva_agregar(char* id, Expr* valor, int linea) {
    return nueva_sentencia(SENT_AGREGAR, 0, TIPO_DESCONOCIDO, id, valor, linea);
}

Sentencia* nueva_quitar(char* id, int linea) {
    return nueva_sentencia(SENT_QUITAR, 0, TIPO_DESCONOCIDO, id, NULL, linea);
}

void agregar_sentencia(Sentencia* s) {
    if (!programa_inicio) programa_inicio = s;
    else programa_fin->siguiente = s;
    programa_fin = s;
}
