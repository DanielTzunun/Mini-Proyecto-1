/* symtab.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

static Simbolo* inicio = NULL;
static Simbolo* fin = NULL;
static int ambito_actual = 0;

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
