/* ast.c - Construccion e impresion del AST */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Sentencia* programa_inicio = NULL;
static Sentencia* programa_fin = NULL;

char* copiar_cadena(const char* s) {
    char* r = malloc(strlen(s) + 1);
    strcpy(r, s);
    return r;
}

const char* nombre_tipo(Tipo t) {
    switch (t) {
        case TIPO_ENTERO:    return "entero";
        case TIPO_MENSAJE:   return "mensaje";
        case TIPO_SIGNO:     return "signo";
        case TIPO_VERACIDAD: return "veracidad";
        default:             return "?";
    }
}

Expr* nueva_expr(ClaseExpr clase, char* texto, int linea, Tipo tipo) {
    Expr* e = malloc(sizeof(Expr));
    e->clase = clase; e->texto = texto; e->linea = linea; e->tipo = tipo;
    return e;
}

Sentencia* nueva_sentencia(ClaseSent clase, int es_constante, Tipo tipo,
                           char* id, Expr* valor, int linea) {
    Sentencia* s = malloc(sizeof(Sentencia));
    s->clase = clase; s->es_constante = es_constante; s->tipo = tipo;
    s->id = id; s->valor = valor; s->linea = linea; s->siguiente = NULL;
    return s;
}

void agregar_sentencia(Sentencia* s) {
    if (!programa_inicio) programa_inicio = s;
    else programa_fin->siguiente = s;
    programa_fin = s;
}

static const char* nombre_expr(const Expr* e) {
    switch (e->clase) {
        case EXPR_ENTERO:    return "LitEntero";
        case EXPR_MENSAJE:   return "LitMensaje";
        case EXPR_SIGNO:     return "LitSigno";
        case EXPR_VERACIDAD: return "LitVeracidad";
        default:             return "Identificador";
    }
}

static void detalle_expr(const Expr* e, char* buf, size_t n) {
    if (e->tipo != TIPO_DESCONOCIDO) snprintf(buf, n, "%s : %s", e->texto, nombre_tipo(e->tipo));
    else snprintf(buf, n, "%s", e->texto);
}

static void detalle_sent(const Sentencia* s, char* buf, size_t n) {
    if (s->clase == SENT_DECLARACION)
        snprintf(buf, n, "%s %s : %s", s->es_constante ? "constante" : "variable", s->id, nombre_tipo(s->tipo));
    else
        snprintf(buf, n, "guardar en %s", s->id);
}

void imprimir_arbol(void) {
    char d[200], v[200];
    printf("Programa\n");
    for (Sentencia* s = programa_inicio; s; s = s->siguiente) {
        int ultima = (s->siguiente == NULL);
        detalle_sent(s, d, sizeof d);
        printf(" %s %s %s [linea %d]\n", ultima ? "'--" : "|--",
               s->clase == SENT_DECLARACION ? "Declaracion" : "Asignacion", d, s->linea);
        if (s->valor) {
            detalle_expr(s->valor, v, sizeof v);
            printf(" %s     '-- Valor: %s %s\n", ultima ? " " : "|", nombre_expr(s->valor), v);
        }
    }
}

void imprimir_tabla_ast(void) {
    char d[200];
    int n = 0;
    printf("%-4s %-14s %-6s %-34s %-5s\n", "#", "Nodo", "Padre", "Detalle", "Linea");
    printf("---- -------------- ------ ---------------------------------- -----\n");
    for (Sentencia* s = programa_inicio; s; s = s->siguiente) {
        int id_sent = ++n;
        detalle_sent(s, d, sizeof d);
        printf("%-4d %-14s %-6d %-34s %-5d\n", id_sent,
               s->clase == SENT_DECLARACION ? "Declaracion" : "Asignacion", 0, d, s->linea);
        if (s->valor) {
            ++n;
            detalle_expr(s->valor, d, sizeof d);
            printf("%-4d %-14s %-6d %-34s %-5d\n", n, nombre_expr(s->valor), id_sent, d, s->valor->linea);
        }
    }
}
