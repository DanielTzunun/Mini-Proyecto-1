/* semantic.c - Reglas semanticas:
 *  1. Toda variable debe declararse antes de usarse
 *  2. No se puede declarar dos veces el mismo identificador
 *  3. El tipo del valor debe coincidir con el tipo de la variable
 *  4. Una constante no se puede modificar con "guardar"
 *  5. No se puede leer una variable que no tiene valor
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "semantic.h"

#define MAX_ERRORES 200
static char* mensajes[MAX_ERRORES];
static int n_errores = 0;

int total_errores_semanticos(void) { return n_errores; }
const char* error_semantico(int i) { return mensajes[i]; }

static void error(int linea, const char* fmt, ...) {
    char cuerpo[256], completo[300];
    va_list args;
    va_start(args, fmt);
    vsnprintf(cuerpo, sizeof cuerpo, fmt, args);
    va_end(args);
    snprintf(completo, sizeof completo, "Error semantico (linea %d): %s", linea, cuerpo);
    if (n_errores < MAX_ERRORES) mensajes[n_errores++] = copiar_cadena(completo);
}

/* Devuelve el tipo de una expresion; anota el tipo en los identificadores */
static Tipo tipo_de(Expr* e) {
    if (e->clase != EXPR_IDENT) return e->tipo;          /* literal: ya trae su tipo */
    Simbolo* s = buscar_simbolo(e->texto);
    if (!s) {
        error(e->linea, "la variable '%s' no ha sido declarada", e->texto);
        return TIPO_DESCONOCIDO;
    }
    e->tipo = s->tipo;
    if (!s->inicializada) {
        error(e->linea, "la variable '%s' se usa pero todavia no tiene valor", e->texto);
        return TIPO_DESCONOCIDO;
    }
    return s->tipo;
}

/* Valor textual conocido: si es un identificador, se usa su ultimo valor */
static const char* valor_texto(const Expr* e) {
    if (e->clase != EXPR_IDENT) return e->texto;
    Simbolo* s = buscar_simbolo(e->texto);
    return s ? s->valor : "-";
}

void analizar_semantica(void) {
    for (Sentencia* s = programa_inicio; s; s = s->siguiente) {
        if (s->clase == SENT_DECLARACION) {
            Simbolo* previo = buscar_simbolo(s->id);
            if (previo)
                error(s->linea, "'%s' ya fue declarada en la linea %d", s->id, previo->linea);

            int ok = 1;
            const char* valor = "-";
            if (s->valor) {
                Tipo tv = tipo_de(s->valor);
                if (tv == TIPO_DESCONOCIDO) ok = 0;
                else if (tv != s->tipo) {
                    error(s->linea, "tipo incompatible: '%s' es %s pero se le da un valor de tipo %s",
                          s->id, nombre_tipo(s->tipo), nombre_tipo(tv));
                    ok = 0;
                }
                if (ok) valor = valor_texto(s->valor);
            }
            if (!previo)
                declarar_simbolo(s->id, s->tipo, s->es_constante, s->valor != NULL && ok, valor, s->linea);
        } else {   /* SENT_ASIGNACION: guardar valor en id */
            Simbolo* dest = buscar_simbolo(s->id);
            Tipo tv = tipo_de(s->valor);
            if (!dest) {
                error(s->linea, "la variable '%s' no ha sido declarada", s->id);
                continue;
            }
            if (dest->es_constante) {
                error(s->linea, "'%s' es una constante y no se puede modificar", s->id);
                continue;
            }
            if (tv != TIPO_DESCONOCIDO && tv != dest->tipo) {
                error(s->linea, "tipo incompatible: '%s' es %s pero se le da un valor de tipo %s",
                      s->id, nombre_tipo(dest->tipo), nombre_tipo(tv));
                continue;
            }
            if (tv != TIPO_DESCONOCIDO) asignar_valor(dest, valor_texto(s->valor));
        }
    }
}
