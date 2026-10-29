/* tabla_simbolos.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabla_simbolos.h"
#include "utils.h"

static Simbolo* inicio = NULL;
static Simbolo* fin = NULL;
static int ambito_actual = 0;

typedef struct {
    Simbolo* simbolo;
    int inicializada;
    char* valor;
} EstadoSimbolo;

struct EstadoSimbolos {
    size_t cantidad;
    EstadoSimbolo* simbolos;
};

Simbolo* buscar_simbolo(const char* nombre) {
    Simbolo* encontrado = NULL;
    for (Simbolo* s = inicio; s; s = s->siguiente)
        if (strcmp(s->nombre, nombre) == 0 &&
            (!encontrado || s->ambito > encontrado->ambito)) encontrado = s;
    return encontrado;
}

int declarar_simbolo(const char* nombre, Tipo tipo, int es_constante,
                     int inicializada, const char* valor, int linea) {
    for (Simbolo* actual = inicio; actual; actual = actual->siguiente)
        if (actual->ambito == ambito_actual && strcmp(actual->nombre, nombre) == 0) return 0;
    Simbolo* s = malloc(sizeof(Simbolo));
    s->nombre = copiar_cadena(nombre);
    s->tipo = tipo;
    s->es_constante = es_constante;
    s->inicializada = inicializada;
    s->valor = copiar_cadena(valor);
    s->linea = linea;
    s->ambito = ambito_actual;
    s->siguiente = NULL;
    if (!inicio) inicio = s; else fin->siguiente = s;
    fin = s;
    return 1;
}

void iniciar_ambito(void) {
    ambito_actual++;
}

void terminar_ambito(void) {
    Simbolo** actual = &inicio;
    while (*actual) {
        Simbolo* s = *actual;
        if (s->ambito == ambito_actual) {
            *actual = s->siguiente;
            if (fin == s) fin = NULL;
            free(s->nombre);
            free(s->valor);
            free(s);
        } else {
            actual = &s->siguiente;
        }
    }
    if (!inicio) fin = NULL;
    else if (!fin) {
        fin = inicio;
        while (fin->siguiente) fin = fin->siguiente;
    }
    ambito_actual--;
}

EstadoSimbolos* guardar_estado_simbolos(void) {
    size_t cantidad = 0;
    for (Simbolo* s = inicio; s; s = s->siguiente) cantidad++;

    EstadoSimbolos* estado = malloc(sizeof(*estado));
    estado->cantidad = cantidad;
    estado->simbolos = malloc(cantidad * sizeof(*estado->simbolos));
    size_t i = 0;
    for (Simbolo* s = inicio; s; s = s->siguiente) {
        estado->simbolos[i].simbolo = s;
        estado->simbolos[i].inicializada = s->inicializada;
        estado->simbolos[i].valor = copiar_cadena(s->valor);
        i++;
    }
    return estado;
}

void restaurar_estado_simbolos(const EstadoSimbolos* estado) {
    for (size_t i = 0; i < estado->cantidad; i++) {
        Simbolo* s = estado->simbolos[i].simbolo;
        s->inicializada = estado->simbolos[i].inicializada;
        free(s->valor);
        s->valor = copiar_cadena(estado->simbolos[i].valor);
    }
}

void liberar_estado_simbolos(EstadoSimbolos* estado) {
    for (size_t i = 0; i < estado->cantidad; i++) free(estado->simbolos[i].valor);
    free(estado->simbolos);
    free(estado);
}

void asignar_valor(Simbolo* s, const char* valor) {
    free(s->valor);
    s->valor = copiar_cadena(valor);
    s->inicializada = 1;
}

void imprimir_tabla_simbolos(void) {
    printf("%-14s %-10s %-10s %-13s %-14s %-5s\n",
           "Nombre", "Tipo", "Categoria", "Inicializada", "Valor", "Linea");
    printf("-------------- ---------- ---------- ------------- -------------- -----\n");
    for (Simbolo* s = inicio; s; s = s->siguiente)
        printf("%-14s %-10s %-10s %-13s %-14s %-5d\n", s->nombre, nombre_tipo(s->tipo),
               s->es_constante ? "constante" : "variable",
               s->inicializada ? "si" : "no", s->valor, s->linea);
}
