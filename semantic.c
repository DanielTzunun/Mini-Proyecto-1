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

static Tipo tipo_de(Expr* e) {
    if (e->clase == EXPR_IDENT) {
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

    if (e->clase == EXPR_COMPARACION || e->clase == EXPR_OR) {
        Tipo izquierda = tipo_de(e->izquierda);
        Tipo derecha = tipo_de(e->derecha);
        if (izquierda == TIPO_DESCONOCIDO || derecha == TIPO_DESCONOCIDO)
            return TIPO_DESCONOCIDO;
        if (e->clase == EXPR_OR) {
            if (izquierda != TIPO_VERACIDAD || derecha != TIPO_VERACIDAD) {
                error(e->linea, "el operador 'or' requiere operandos de tipo veracidad");
                return TIPO_DESCONOCIDO;
            }
        } else {
            if (izquierda != derecha) {
                error(e->linea, "no se pueden comparar valores de tipo %s y %s",
                      nombre_tipo(izquierda), nombre_tipo(derecha));
                return TIPO_DESCONOCIDO;
            }
            if ((strcmp(e->texto, ">") == 0 || strcmp(e->texto, "<") == 0 ||
                 strcmp(e->texto, ">=") == 0 || strcmp(e->texto, "<=") == 0) &&
                izquierda != TIPO_ENTERO) {
                error(e->linea, "el operador '%s' solo admite valores de tipo entero", e->texto);
                return TIPO_DESCONOCIDO;
            }
        }
        e->tipo = TIPO_VERACIDAD;
        return e->tipo;
    }
    return e->tipo;
}


static const char* valor_texto(const Expr* e) {
    if (e->clase != EXPR_IDENT) return e->texto;
    Simbolo* s = buscar_simbolo(e->texto);
    return s ? s->valor : "-";
}

static int existe_funcion(const char* nombre) {
    for (Sentencia* s = programa_inicio; s; s = s->siguiente)
        if (s->clase == SENT_FUNCION && strcmp(s->id, nombre) == 0) return 1;
    return 0;
}

static void validar_llamadas(Sentencia* lista) {
    for (Sentencia* s = lista; s; s = s->siguiente) {
        if (s->clase == SENT_LLAMADA && !existe_funcion(s->id))
            error(s->linea, "la funcion '%s' no ha sido declarada", s->id);
        if (s->clase == SENT_FUNCION) validar_llamadas(s->cuerpo);
        if (s->clase == SENT_CONDICIONAL) {
            validar_llamadas(s->cuerpo);
            validar_llamadas(s->alternativa);
        }
    }
}

static void analizar_lista(Sentencia* lista, int dentro_funcion);

static void analizar_condicional(Sentencia* s, int dentro_funcion) {
    Tipo tipo_condicion = tipo_de(s->valor);
    if (tipo_condicion != TIPO_DESCONOCIDO && tipo_condicion != TIPO_VERACIDAD)
        error(s->valor->linea, "la condicion de 'if' debe ser de tipo veracidad, no %s",
              nombre_tipo(tipo_condicion));

    EstadoSimbolos* estado = guardar_estado_simbolos();
    iniciar_ambito();
    analizar_lista(s->cuerpo, dentro_funcion);
    terminar_ambito();
    restaurar_estado_simbolos(estado);

    if (s->alternativa) {
        iniciar_ambito();
        analizar_lista(s->alternativa, dentro_funcion);
        terminar_ambito();
        restaurar_estado_simbolos(estado);
    }
    liberar_estado_simbolos(estado);
}

static void analizar_lista(Sentencia* lista, int dentro_funcion) {
    for (Sentencia* s = lista; s; s = s->siguiente) {
        if (s->clase == SENT_FUNCION || s->clase == SENT_LLAMADA) continue;
        if (s->clase == SENT_CONDICIONAL) {
            analizar_condicional(s, dentro_funcion);
            continue;
        }
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
        } else {
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
            if (tv != TIPO_DESCONOCIDO && (!dentro_funcion || dest->ambito > 0))
                asignar_valor(dest, valor_texto(s->valor));
        }
    }
}

void analizar_semantica(void) {
    for (Sentencia* s = programa_inicio; s; s = s->siguiente) {
        if (s->clase != SENT_FUNCION) continue;
        for (Sentencia* anterior = programa_inicio; anterior != s; anterior = anterior->siguiente) {
            if (anterior->clase == SENT_FUNCION && strcmp(anterior->id, s->id) == 0) {
                error(s->linea, "la funcion '%s' ya fue declarada en la linea %d", s->id, anterior->linea);
                break;
            }
        }
    }

    analizar_lista(programa_inicio, 0);

    for (Sentencia* s = programa_inicio; s; s = s->siguiente) {
        if (s->clase != SENT_FUNCION) continue;
        if (buscar_simbolo(s->id))
            error(s->linea, "el nombre de funcion '%s' ya se usa como variable", s->id);
        iniciar_ambito();
        analizar_lista(s->cuerpo, 1);
        terminar_ambito();
    }

    validar_llamadas(programa_inicio);
}
