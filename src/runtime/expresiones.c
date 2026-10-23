/* expresiones.c - Evaluacion de expresiones del interprete */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runtime_interno.h"
#include "utils.h"

static char* sin_comillas(const char* s) {
    size_t n = strlen(s);
    if (n >= 2) {
        char* r = malloc(n - 1);
        memcpy(r, s + 1, n - 2);
        r[n - 2] = '\0';
        return r;
    }
    return copiar_cadena(s);
}

static int aplicar_comparacion(const Valor* a, const Valor* b,
                               const char* operador, int* resultado) {
    if (strcmp(operador, "==") == 0) {
        *resultado = valores_iguales(a, b);
        return 1;
    }
    if (strcmp(operador, "!=") == 0) {
        *resultado = !valores_iguales(a, b);
        return 1;
    }

    if (a->clase != VAL_ENTERO || b->clase != VAL_ENTERO) {
        error_runtime(0, "el operador '%s' requiere valores enteros", operador);
        return 0;
    }
    if (strcmp(operador, ">") == 0) *resultado = a->entero > b->entero;
    else if (strcmp(operador, "<") == 0) *resultado = a->entero < b->entero;
    else if (strcmp(operador, ">=") == 0) *resultado = a->entero >= b->entero;
    else if (strcmp(operador, "<=") == 0) *resultado = a->entero <= b->entero;
    else {
        error_runtime(0, "operador de comparacion desconocido '%s'", operador);
        return 0;
    }
    return 1;
}

static int evaluar_arreglo(const Expr* e, Valor* salida) {
    Valor arreglo = valor_arreglo(TIPO_DESCONOCIDO);

    for (const Expr* it = e->izquierda; it; it = it->siguiente) {
        Valor elemento;
        if (!evaluar_expresion(it, &elemento)) {
            valor_liberar(&arreglo);
            return 0;
        }
        if (elemento.clase == VAL_ARREGLO) {
            error_runtime(it->linea, "no se permiten arreglos dentro de otro arreglo");
            valor_liberar(&elemento);
            valor_liberar(&arreglo);
            return 0;
        }
        valor_agregar(&arreglo, &elemento);
        valor_liberar(&elemento);
    }

    *salida = arreglo;
    return 1;
}

static int evaluar_indice(const Expr* e, Valor* salida) {
    Valor arreglo = {0};
    Valor indice = {0};

    if (!evaluar_expresion(e->izquierda, &arreglo)) return 0;
    if (!evaluar_expresion(e->derecha, &indice)) {
        valor_liberar(&arreglo);
        return 0;
    }

    if (arreglo.clase != VAL_ARREGLO) {
        error_runtime(e->linea, "solo se puede indexar un arreglo");
        valor_liberar(&arreglo);
        valor_liberar(&indice);
        return 0;
    }
    if (indice.clase != VAL_ENTERO) {
        error_runtime(e->linea, "el indice de un arreglo debe ser entero");
        valor_liberar(&arreglo);
        valor_liberar(&indice);
        return 0;
    }
    if (indice.entero < 0 || indice.entero >= valor_longitud(&arreglo)) {
        error_runtime(e->linea, "indice %d fuera de rango (el arreglo tiene %d elemento(s))",
                      indice.entero, valor_longitud(&arreglo));
        valor_liberar(&arreglo);
        valor_liberar(&indice);
        return 0;
    }

    const Valor* elemento = arreglo.primero;
    for (int i = 0; i < indice.entero; i++) elemento = elemento->siguiente;
    *salida = valor_copiar(elemento);

    valor_liberar(&arreglo);
    valor_liberar(&indice);
    return 1;
}

int evaluar_expresion(const Expr* e, Valor* salida) {
    switch (e->clase) {
        case EXPR_ENTERO:
            *salida = valor_entero(atoi(e->texto));
            return 1;

        case EXPR_MENSAJE: {
            char* texto = sin_comillas(e->texto);
            *salida = valor_texto(VAL_MENSAJE, texto);
            free(texto);
            return 1;
        }

        case EXPR_SIGNO: {
            char* texto = sin_comillas(e->texto);
            *salida = valor_texto(VAL_SIGNO, texto);
            free(texto);
            return 1;
        }

        case EXPR_VERACIDAD:
            *salida = valor_booleano(strcmp(e->texto, "verdadero") == 0);
            return 1;

        case EXPR_IDENT: {
            VariableRuntime* variable = buscar_variable_runtime(e->texto);
            if (!variable) {
                error_runtime(e->linea, "la variable '%s' no existe en ejecucion", e->texto);
                return 0;
            }
            *salida = valor_copiar(&variable->valor);
            return 1;
        }

        case EXPR_ARREGLO:
            return evaluar_arreglo(e, salida);

        case EXPR_INDICE:
            return evaluar_indice(e, salida);

        case EXPR_ARITMETICA: {
            Valor izquierda = {0};
            Valor derecha = {0};
            if (!evaluar_expresion(e->izquierda, &izquierda)) return 0;
            if (!evaluar_expresion(e->derecha, &derecha)) {
                valor_liberar(&izquierda);
                return 0;
            }

            if (izquierda.clase != VAL_ENTERO || derecha.clase != VAL_ENTERO) {
                error_runtime(e->linea, "el operador '%s' requiere valores enteros", e->texto);
                valor_liberar(&izquierda);
                valor_liberar(&derecha);
                return 0;
            }

            int resultado = 0;
            if (strcmp(e->texto, "+") == 0) resultado = izquierda.entero + derecha.entero;
            else if (strcmp(e->texto, "-") == 0) resultado = izquierda.entero - derecha.entero;
            else if (strcmp(e->texto, "*") == 0) resultado = izquierda.entero * derecha.entero;
            else if (strcmp(e->texto, "/") == 0) {
                if (derecha.entero == 0) {
                    error_runtime(e->linea, "no se puede dividir entre cero");
                    valor_liberar(&izquierda);
                    valor_liberar(&derecha);
                    return 0;
                }
                resultado = izquierda.entero / derecha.entero;
            } else {
                error_runtime(e->linea, "operador aritmetico desconocido '%s'", e->texto);
                valor_liberar(&izquierda);
                valor_liberar(&derecha);
                return 0;
            }

            valor_liberar(&izquierda);
            valor_liberar(&derecha);
            *salida = valor_entero(resultado);
            return 1;
        }

        case EXPR_NEGATIVO: {
            Valor operando = {0};
            if (!evaluar_expresion(e->izquierda, &operando)) return 0;
            if (operando.clase != VAL_ENTERO) {
                error_runtime(e->linea, "el operador unario '-' requiere un valor entero");
                valor_liberar(&operando);
                return 0;
            }
            *salida = valor_entero(-operando.entero);
            valor_liberar(&operando);
            return 1;
        }

        case EXPR_COMPARACION: {
            Valor izquierda = {0};
            Valor derecha = {0};
            if (!evaluar_expresion(e->izquierda, &izquierda)) return 0;
            if (!evaluar_expresion(e->derecha, &derecha)) {
                valor_liberar(&izquierda);
                return 0;
            }

            int resultado = 0;
            int ok = aplicar_comparacion(&izquierda, &derecha, e->texto, &resultado);
            valor_liberar(&izquierda);
            valor_liberar(&derecha);
            if (!ok) return 0;

            *salida = valor_booleano(resultado);
            return 1;
        }

        case EXPR_OR: {
            Valor izquierda = {0};
            Valor derecha = {0};
            if (!evaluar_expresion(e->izquierda, &izquierda)) return 0;
            if (!evaluar_expresion(e->derecha, &derecha)) {
                valor_liberar(&izquierda);
                return 0;
            }

            if (izquierda.clase != VAL_VERACIDAD || derecha.clase != VAL_VERACIDAD) {
                error_runtime(e->linea, "el operador 'or' requiere valores de tipo veracidad");
                valor_liberar(&izquierda);
                valor_liberar(&derecha);
                return 0;
            }

            *salida = valor_booleano(izquierda.booleano || derecha.booleano);
            valor_liberar(&izquierda);
            valor_liberar(&derecha);
            return 1;
        }
    }

    error_runtime(e->linea, "expresion no soportada en ejecucion");
    return 0;
}
