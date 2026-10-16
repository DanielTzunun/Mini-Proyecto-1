/* symtab.h - Tabla de simbolos de Nova (lista enlazada) */
#ifndef SYMTAB_H
#define SYMTAB_H
#include "ast.h"

typedef struct Simbolo {
    char* nombre;
    Tipo  tipo;
    int   es_constante;
    int   inicializada;
    char* valor;      /* ultimo valor conocido (texto) o "-" */
    int   linea;      /* linea de declaracion */
    int   ambito;
    struct Simbolo* siguiente;
} Simbolo;

typedef struct EstadoSimbolos EstadoSimbolos;

int      declarar_simbolo(const char* nombre, Tipo tipo, int es_constante,
                          int inicializada, const char* valor, int linea);  /* 0 si ya existe */
Simbolo* buscar_simbolo(const char* nombre);                                /* NULL si no existe */
void     asignar_valor(Simbolo* s, const char* valor);
void     iniciar_ambito(void);
void     terminar_ambito(void);
EstadoSimbolos* guardar_estado_simbolos(void);
void     restaurar_estado_simbolos(const EstadoSimbolos* estado);
void     liberar_estado_simbolos(EstadoSimbolos* estado);
void     imprimir_tabla_simbolos(void);

#endif
