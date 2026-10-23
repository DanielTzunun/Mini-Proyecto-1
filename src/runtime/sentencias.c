/* sentencias.c - Ejecucion de sentencias y manejo de variables */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "runtime_interno.h"
#include "utils.h"

static void liberar_variable(VariableRuntime* v) {
    free(v->nombre);
    valor_liberar(&v->valor);
    free(v);
}

VariableRuntime* buscar_variable_runtime(const char* nombre) {
    VariableRuntime* encontrado = NULL;
    for (VariableRuntime* v = variables_runtime; v; v = v->siguiente)
        if (strcmp(v->nombre, nombre) == 0 &&
            (!encontrado || v->ambito > encontrado->ambito))
            encontrado = v;
    return encontrado;
}

int declarar_variable_runtime(const char* nombre, Tipo tipo, const Valor* valor) {
    VariableRuntime* v = malloc(sizeof(VariableRuntime));
    v->nombre = copiar_cadena(nombre);
    v->tipo = tipo;
    v->valor = valor ? valor_copiar(valor) : valor_por_defecto(tipo);
    v->ambito = ambito_runtime;
    v->siguiente = NULL;

    if (v->valor.clase == VAL_ARREGLO &&
        v->valor.tipo_elemento == TIPO_DESCONOCIDO &&
        es_tipo_arreglo(tipo))
        v->valor.tipo_elemento = tipo_elemento(tipo);

    if (!variables_runtime) variables_runtime = v;
    else variables_runtime_fin->siguiente = v;
    variables_runtime_fin = v;
    return 1;
}

int asignar_variable_runtime(const char* nombre, const Valor* valor) {
    VariableRuntime* v = buscar_variable_runtime(nombre);
    if (!v) return 0;

    Valor copia = valor_copiar(valor);
    if (copia.clase == VAL_ARREGLO &&
        copia.tipo_elemento == TIPO_DESCONOCIDO &&
        es_tipo_arreglo(v->tipo))
        copia.tipo_elemento = tipo_elemento(v->tipo);

    valor_liberar(&v->valor);
    v->valor = copia;
    return 1;
}

int agregar_a_arreglo_runtime(const char* nombre, const Valor* valor, int linea) {
    VariableRuntime* v = buscar_variable_runtime(nombre);
    if (!v) {
        error_runtime(linea, "la variable '%s' no existe en ejecucion", nombre);
        return 0;
    }
    if (v->valor.clase != VAL_ARREGLO) {
        error_runtime(linea, "'%s' no es un arreglo", nombre);
        return 0;
    }
    valor_agregar(&v->valor, valor);
    return 1;
}

int quitar_de_arreglo_runtime(const char* nombre, int linea) {
    VariableRuntime* v = buscar_variable_runtime(nombre);
    if (!v) {
        error_runtime(linea, "la variable '%s' no existe en ejecucion", nombre);
        return 0;
    }
    if (v->valor.clase != VAL_ARREGLO) {
        error_runtime(linea, "'%s' no es un arreglo", nombre);
        return 0;
    }
    if (!valor_quitar(&v->valor)) {
        error_runtime(linea, "no se puede quitar de un arreglo vacio ('%s')", nombre);
        return 0;
    }
    return 1;
}

void entrar_ambito_runtime(void) {
    ambito_runtime++;
}

void salir_ambito_runtime(void) {
    VariableRuntime** actual = &variables_runtime;
    while (*actual) {
        VariableRuntime* v = *actual;
        if (v->ambito == ambito_runtime) {
            *actual = v->siguiente;
            if (variables_runtime_fin == v) variables_runtime_fin = NULL;
            liberar_variable(v);
        } else {
            actual = &v->siguiente;
        }
    }

    if (!variables_runtime) {
        variables_runtime_fin = NULL;
    } else if (!variables_runtime_fin) {
        variables_runtime_fin = variables_runtime;
        while (variables_runtime_fin->siguiente)
            variables_runtime_fin = variables_runtime_fin->siguiente;
    }

    ambito_runtime--;
}

static Sentencia* buscar_funcion(const char* nombre) {
    for (Sentencia* s = programa_inicio; s; s = s->siguiente)
        if (s->clase == SENT_FUNCION && strcmp(s->id, nombre) == 0)
            return s;
    return NULL;
}

static int ejecutar_condicional(const Sentencia* s) {
    Valor condicion = {0};
    if (!evaluar_expresion(s->valor, &condicion)) return 0;

    if (condicion.clase != VAL_VERACIDAD) {
        error_runtime(s->valor->linea, "la condicion de 'if' no es de tipo veracidad");
        valor_liberar(&condicion);
        return 0;
    }

    int resultado = condicion.booleano;
    valor_liberar(&condicion);

    entrar_ambito_runtime();
    int ok = ejecutar_lista(resultado ? s->cuerpo : s->alternativa);
    salir_ambito_runtime();
    return ok;
}

static int profundidad_llamadas = 0;

static int ejecutar_llamada(const Sentencia* s) {
    Sentencia* funcion = buscar_funcion(s->id);
    if (!funcion) {
        error_runtime(s->linea, "la funcion '%s' no existe en ejecucion", s->id);
        return 0;
    }
    if (++profundidad_llamadas > 1000) {
        error_runtime(s->linea, "demasiadas llamadas anidadas a la funcion '%s'", s->id);
        profundidad_llamadas--;
        return 0;
    }

    entrar_ambito_runtime();
    int ok = ejecutar_lista(funcion->cuerpo);
    salir_ambito_runtime();
    profundidad_llamadas--;
    return ok;
}

int ejecutar_sentencia(const Sentencia* s) {
    switch (s->clase) {
        case SENT_DECLARACION: {
            Valor valor;
            if (s->valor) {
                if (!evaluar_expresion(s->valor, &valor)) return 0;
            } else {
                valor = valor_por_defecto(s->tipo);
            }
            int ok = declarar_variable_runtime(s->id, s->tipo, &valor);
            valor_liberar(&valor);
            return ok;
        }

        case SENT_ASIGNACION: {
            Valor valor = {0};
            if (!evaluar_expresion(s->valor, &valor)) return 0;
            int ok = asignar_variable_runtime(s->id, &valor);
            valor_liberar(&valor);
            if (!ok) {
                error_runtime(s->linea, "la variable '%s' no existe en ejecucion", s->id);
                return 0;
            }
            return 1;
        }

        case SENT_FUNCION:
            return 1;

        case SENT_LLAMADA:
            return ejecutar_llamada(s);

        case SENT_CONDICIONAL:
            return ejecutar_condicional(s);

        case SENT_MOSTRAR: {
            Valor valor = {0};
            if (!evaluar_expresion(s->valor, &valor)) return 0;
            valor_imprimir(&valor);
            printf("\n");
            valor_liberar(&valor);
            return 1;
        }

        case SENT_AGREGAR: {
            Valor valor = {0};
            if (!evaluar_expresion(s->valor, &valor)) return 0;
            int ok = agregar_a_arreglo_runtime(s->id, &valor, s->linea);
            valor_liberar(&valor);
            return ok;
        }

        case SENT_QUITAR:
            return quitar_de_arreglo_runtime(s->id, s->linea);
    }

    error_runtime(s->linea, "sentencia no soportada en ejecucion");
    return 0;
}

int ejecutar_lista(const Sentencia* lista) {
    for (const Sentencia* s = lista; s; s = s->siguiente)
        if (!ejecutar_sentencia(s)) return 0;
    return 1;
}
