/* symtab.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

static Simbolo* inicio = NULL;
static Simbolo* fin = NULL;

Simbolo* buscar_simbolo(const char* nombre) {
    for (Simbolo* s = inicio; s; s = s->siguiente)
        if (strcmp(s->nombre, nombre) == 0) return s;
    return NULL;
}

int declarar_simbolo(const char* nombre, Tipo tipo, int es_constante,
                     int inicializada, const char* valor, int linea) {
    if (buscar_simbolo(nombre)) return 0;
    Simbolo* s = malloc(sizeof(Simbolo));
    s->nombre = copiar_cadena(nombre);
    s->tipo = tipo;
    s->es_constante = es_constante;
    s->inicializada = inicializada;
    s->valor = copiar_cadena(valor);
    s->linea = linea;
    s->siguiente = NULL;
    if (!inicio) inicio = s; else fin->siguiente = s;
    fin = s;
    return 1;
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
