/* valores.c - Valores y operaciones basicas del interprete */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runtime_interno.h"
#include "utils.h"

static Tipo tipo_de_valor(const Valor* v) {
    switch (v->clase) {
        case VAL_ENTERO:    return TIPO_ENTERO;
        case VAL_MENSAJE:   return TIPO_MENSAJE;
        case VAL_SIGNO:     return TIPO_SIGNO;
        case VAL_VERACIDAD: return TIPO_VERACIDAD;
        case VAL_ARREGLO:   return tipo_arreglo(v->tipo_elemento);
    }
    return TIPO_DESCONOCIDO;
}

Valor valor_entero(int v) {
    Valor r = {0};
    r.clase = VAL_ENTERO;
    r.entero = v;
    return r;
}

Valor valor_texto(ClaseValor clase, const char* texto) {
    Valor r = {0};
    r.clase = clase;
    r.texto = copiar_cadena(texto ? texto : "");
    return r;
}

Valor valor_booleano(int v) {
    Valor r = {0};
    r.clase = VAL_VERACIDAD;
    r.booleano = v ? 1 : 0;
    return r;
}

Valor valor_arreglo(Tipo tipo_elemento) {
    Valor r = {0};
    r.clase = VAL_ARREGLO;
    r.tipo_elemento = tipo_elemento;
    return r;
}

Valor valor_por_defecto(Tipo tipo) {
    if (es_tipo_arreglo(tipo))
        return valor_arreglo(tipo_elemento(tipo));

    switch (tipo) {
        case TIPO_MENSAJE:   return valor_texto(VAL_MENSAJE, "");
        case TIPO_SIGNO:     return valor_texto(VAL_SIGNO, "");
        case TIPO_VERACIDAD: return valor_booleano(0);
        default:             return valor_entero(0);
    }
}

Valor valor_copiar(const Valor* v) {
    Valor r = {0};
    if (!v) return r;

    r.clase = v->clase;
    r.entero = v->entero;
    r.booleano = v->booleano;
    r.tipo_elemento = v->tipo_elemento;
    if (v->texto) r.texto = copiar_cadena(v->texto);

    if (v->clase == VAL_ARREGLO) {
        for (const Valor* it = v->primero; it; it = it->siguiente)
            valor_agregar(&r, it);
    }
    return r;
}

void valor_liberar(Valor* v) {
    if (!v) return;
    free(v->texto);
    v->texto = NULL;

    if (v->clase == VAL_ARREGLO) {
        Valor* it = v->primero;
        while (it) {
            Valor* siguiente = it->siguiente;
            valor_liberar(it);
            free(it);
            it = siguiente;
        }
        v->primero = NULL;
    }
}

void valor_agregar(Valor* arreglo, const Valor* elemento) {
    Valor* copia = malloc(sizeof(Valor));
    *copia = valor_copiar(elemento);
    copia->siguiente = NULL;

    if (!arreglo->primero) {
        arreglo->primero = copia;
    } else {
        Valor* ultimo = arreglo->primero;
        while (ultimo->siguiente) ultimo = ultimo->siguiente;
        ultimo->siguiente = copia;
    }

    if (arreglo->tipo_elemento == TIPO_DESCONOCIDO && elemento->clase != VAL_ARREGLO)
        arreglo->tipo_elemento = tipo_de_valor(elemento);
}

int valor_quitar(Valor* arreglo) {
    if (!arreglo->primero) return 0;

    Valor* anterior = NULL;
    Valor* ultimo = arreglo->primero;
    while (ultimo->siguiente) {
        anterior = ultimo;
        ultimo = ultimo->siguiente;
    }

    if (anterior) anterior->siguiente = NULL;
    else arreglo->primero = NULL;

    valor_liberar(ultimo);
    free(ultimo);
    return 1;
}

int valor_longitud(const Valor* arreglo) {
    int total = 0;
    for (const Valor* it = arreglo->primero; it; it = it->siguiente) total++;
    return total;
}

void valor_imprimir(const Valor* v) {
    switch (v->clase) {
        case VAL_ENTERO:
            printf("%d", v->entero);
            break;
        case VAL_MENSAJE:
        case VAL_SIGNO:
            printf("%s", v->texto ? v->texto : "");
            break;
        case VAL_VERACIDAD:
            printf("%s", v->booleano ? "verdadero" : "falso");
            break;
        case VAL_ARREGLO: {
            printf("[");
            int primero = 1;
            for (const Valor* it = v->primero; it; it = it->siguiente) {
                if (!primero) printf(", ");
                valor_imprimir(it);
                primero = 0;
            }
            printf("]");
            break;
        }
    }
}

int valores_iguales(const Valor* a, const Valor* b) {
    if (a->clase != b->clase) return 0;

    switch (a->clase) {
        case VAL_ENTERO:
            return a->entero == b->entero;
        case VAL_VERACIDAD:
            return a->booleano == b->booleano;
        case VAL_MENSAJE:
        case VAL_SIGNO:
            return strcmp(a->texto ? a->texto : "", b->texto ? b->texto : "") == 0;
        case VAL_ARREGLO: {
            const Valor* x = a->primero;
            const Valor* y = b->primero;
            while (x && y) {
                if (!valores_iguales(x, y)) return 0;
                x = x->siguiente;
                y = y->siguiente;
            }
            return x == NULL && y == NULL;
        }
    }
    return 0;
}

int valor_a_entero(const Valor* v) {
    return v->clase == VAL_ENTERO ? v->entero : 0;
}
