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
    s->id = id; s->valor = valor; s->cuerpo = NULL;
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
    else if (s->clase == SENT_ASIGNACION)
        snprintf(buf, n, "guardar en %s", s->id);
    else if (s->clase == SENT_FUNCION)
        snprintf(buf, n, "funcion %s()", s->id);
    else
        snprintf(buf, n, "llamada %s()", s->id);
}

static void imprimir_lista_arbol(const Sentencia* lista, int nivel) {
    char detalle[200], valor[200];
    for (const Sentencia* s = lista; s; s = s->siguiente) {
        detalle_sent(s, detalle, sizeof detalle);
        printf("%*s- %s [linea %d]\n", nivel * 2, "", detalle, s->linea);
        if (s->valor) {
            detalle_expr(s->valor, valor, sizeof valor);
            printf("%*s  Valor: %s %s\n", nivel * 2, "", nombre_expr(s->valor), valor);
        }
        if (s->clase == SENT_FUNCION) imprimir_lista_arbol(s->cuerpo, nivel + 1);
    }
}

void imprimir_arbol(void) {
    printf("Programa\n");
    imprimir_lista_arbol(programa_inicio, 1);
}

static int imprimir_lista_tabla(const Sentencia* lista, int padre, int* siguiente_id) {
    char detalle[200];
    for (const Sentencia* s = lista; s; s = s->siguiente) {
        int id_sent = ++*siguiente_id;
        detalle_sent(s, detalle, sizeof detalle);
        const char* clase = s->clase == SENT_DECLARACION ? "Declaracion" :
                            s->clase == SENT_ASIGNACION ? "Asignacion" :
                            s->clase == SENT_FUNCION ? "Funcion" : "Llamada";
        printf("%-4d %-14s %-6d %-34s %-5d\n", id_sent, clase, padre, detalle, s->linea);
        if (s->valor) {
            ++*siguiente_id;
            detalle_expr(s->valor, detalle, sizeof detalle);
            printf("%-4d %-14s %-6d %-34s %-5d\n", *siguiente_id,
                   nombre_expr(s->valor), id_sent, detalle, s->valor->linea);
        }
        if (s->clase == SENT_FUNCION)
            imprimir_lista_tabla(s->cuerpo, id_sent, siguiente_id);
    }
    return *siguiente_id;
}

void imprimir_tabla_ast(void) {
    int siguiente_id = 0;
    printf("%-4s %-14s %-6s %-34s %-5s\n", "#", "Nodo", "Padre", "Detalle", "Linea");
    printf("---- -------------- ------ ---------------------------------- -----\n");
    imprimir_lista_tabla(programa_inicio, 0, &siguiente_id);
}
