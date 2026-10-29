/* ast_impresion.c - Impresion del AST de Nova (arbol y tabla) */
#include <stdio.h>
#include "ast.h"

static const char* nombre_expr(const Expr* e) {
    switch (e->clase) {
        case EXPR_ENTERO:      return "LitEntero";
        case EXPR_MENSAJE:     return "LitMensaje";
        case EXPR_SIGNO:       return "LitSigno";
        case EXPR_VERACIDAD:   return "LitVeracidad";
        case EXPR_COMPARACION: return "Comparacion";
        case EXPR_OR:          return "O";
        case EXPR_ARITMETICA:  return "Aritmetica";
        case EXPR_NEGATIVO:    return "Negativo";
        case EXPR_ARREGLO:     return "Arreglo";
        case EXPR_INDICE:      return "Indice";
        default:               return "Identificador";
    }
}

static int contar_elementos(const Expr* e) {
    int total = 0;
    for (const Expr* it = e->izquierda; it; it = it->siguiente) total++;
    return total;
}

static void detalle_expr(const Expr* e, char* buf, size_t n) {
    if (e->clase == EXPR_COMPARACION || e->clase == EXPR_OR ||
        e->clase == EXPR_ARITMETICA || e->clase == EXPR_NEGATIVO) {
        snprintf(buf, n, "%s", e->texto);
    } else if (e->clase == EXPR_ARREGLO) {
        snprintf(buf, n, "[%d elemento(s)]", contar_elementos(e));
    } else if (e->clase == EXPR_INDICE) {
        const char* base = (e->izquierda && e->izquierda->texto)
                               ? e->izquierda->texto : "arreglo";
        if (e->tipo != TIPO_DESCONOCIDO)
            snprintf(buf, n, "%s[...] : %s", base, nombre_tipo(e->tipo));
        else
            snprintf(buf, n, "%s[...]", base);
    } else if (e->tipo != TIPO_DESCONOCIDO) {
        snprintf(buf, n, "%s : %s", e->texto, nombre_tipo(e->tipo));
    } else {
        snprintf(buf, n, "%s", e->texto);
    }
}

static void detalle_sent(const Sentencia* s, char* buf, size_t n) {
    if (s->clase == SENT_DECLARACION)
        snprintf(buf, n, "%s %s : %s", s->es_constante ? "constante" : "variable",
                 s->id, nombre_tipo(s->tipo));
    else if (s->clase == SENT_ASIGNACION)
        snprintf(buf, n, "guardar en %s", s->id);
    else if (s->clase == SENT_FUNCION)
        snprintf(buf, n, "funcion %s()", s->id);
    else if (s->clase == SENT_CONDICIONAL)
        snprintf(buf, n, "if");
    else if (s->clase == SENT_AGREGAR)
        snprintf(buf, n, "agregar en %s", s->id);
    else if (s->clase == SENT_QUITAR)
        snprintf(buf, n, "quitar de %s", s->id);
    else if (s->clase == SENT_MOSTRAR)
        snprintf(buf, n, "mostrar");
    else
        snprintf(buf, n, "llamada %s()", s->id);
}

static void imprimir_expr_arbol(const Expr* e, int nivel) {
    char detalle[200];
    detalle_expr(e, detalle, sizeof detalle);
    printf("%*s- %s: %s [linea %d]\n", nivel * 2, "", nombre_expr(e), detalle, e->linea);

    if (e->clase == EXPR_ARREGLO) {
        for (const Expr* it = e->izquierda; it; it = it->siguiente)
            imprimir_expr_arbol(it, nivel + 1);
    } else {
        if (e->izquierda) imprimir_expr_arbol(e->izquierda, nivel + 1);
        if (e->derecha) imprimir_expr_arbol(e->derecha, nivel + 1);
    }
}

static void imprimir_expr_tabla(const Expr* e, int padre, int* siguiente_id) {
    int id_expr = ++*siguiente_id;
    char detalle[200];
    detalle_expr(e, detalle, sizeof detalle);
    printf("%-4d %-14s %-6d %-34s %-5d\n", id_expr,
           nombre_expr(e), padre, detalle, e->linea);

    if (e->clase == EXPR_ARREGLO) {
        for (const Expr* it = e->izquierda; it; it = it->siguiente)
            imprimir_expr_tabla(it, id_expr, siguiente_id);
    } else {
        if (e->izquierda) imprimir_expr_tabla(e->izquierda, id_expr, siguiente_id);
        if (e->derecha) imprimir_expr_tabla(e->derecha, id_expr, siguiente_id);
    }
}

static void imprimir_lista_arbol(const Sentencia* lista, int nivel) {
    char detalle[200], valor[200];
    for (const Sentencia* s = lista; s; s = s->siguiente) {
        detalle_sent(s, detalle, sizeof detalle);
        printf("%*s- %s [linea %d]\n", nivel * 2, "", detalle, s->linea);
        if (s->valor) {
            if (s->clase == SENT_CONDICIONAL) {
                printf("%*s  Condicion:\n", nivel * 2, "");
                imprimir_expr_arbol(s->valor, nivel + 2);
            } else if (s->valor->clase == EXPR_ARREGLO ||
                       s->valor->clase == EXPR_INDICE ||
                       s->valor->clase == EXPR_COMPARACION ||
                       s->valor->clase == EXPR_OR ||
                       s->valor->clase == EXPR_ARITMETICA ||
                       s->valor->clase == EXPR_NEGATIVO) {
                printf("%*s  Valor:\n", nivel * 2, "");
                imprimir_expr_arbol(s->valor, nivel + 2);
            } else {
                detalle_expr(s->valor, valor, sizeof valor);
                printf("%*s  Valor: %s %s\n", nivel * 2, "",
                       nombre_expr(s->valor), valor);
            }
        }
        if (s->clase == SENT_FUNCION || s->clase == SENT_CONDICIONAL)
            imprimir_lista_arbol(s->cuerpo, nivel + 1);
        if (s->clase == SENT_CONDICIONAL && s->alternativa) {
            printf("%*s- else\n", nivel * 2, "");
            imprimir_lista_arbol(s->alternativa, nivel + 1);
        }
    }
}

void imprimir_arbol(void) {
    printf("Programa\n");
    imprimir_lista_arbol(programa_inicio, 1);
}

static void imprimir_lista_tabla(const Sentencia* lista, int padre, int* siguiente_id) {
    char detalle[200];
    for (const Sentencia* s = lista; s; s = s->siguiente) {
        int id_sent = ++*siguiente_id;
        detalle_sent(s, detalle, sizeof detalle);
        const char* clase = s->clase == SENT_DECLARACION ? "Declaracion" :
                            s->clase == SENT_ASIGNACION ? "Asignacion" :
                            s->clase == SENT_FUNCION ? "Funcion" :
                            s->clase == SENT_CONDICIONAL ? "Condicional" :
                            s->clase == SENT_AGREGAR ? "Agregar" :
                            s->clase == SENT_QUITAR ? "Quitar" :
                            s->clase == SENT_MOSTRAR ? "Mostrar" : "Llamada";
        printf("%-4d %-14s %-6d %-34s %-5d\n", id_sent, clase, padre, detalle, s->linea);
        if (s->valor)
            imprimir_expr_tabla(s->valor, id_sent, siguiente_id);
        if (s->clase == SENT_FUNCION || s->clase == SENT_CONDICIONAL)
            imprimir_lista_tabla(s->cuerpo, id_sent, siguiente_id);
        if (s->clase == SENT_CONDICIONAL && s->alternativa)
            imprimir_lista_tabla(s->alternativa, id_sent, siguiente_id);
    }
}

void imprimir_tabla_ast(void) {
    int siguiente_id = 0;
    printf("%-4s %-14s %-6s %-34s %-5s\n", "#", "Nodo", "Padre", "Detalle", "Linea");
    printf("---- -------------- ------ ---------------------------------- -----\n");
    imprimir_lista_tabla(programa_inicio, 0, &siguiente_id);
}
