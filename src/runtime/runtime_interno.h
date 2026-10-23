/* runtime_interno.h - Estructuras internas del interprete de Nova */
#ifndef RUNTIME_INTERNO_H
#define RUNTIME_INTERNO_H

#include "ast.h"

typedef enum {
    VAL_ENTERO,
    VAL_MENSAJE,
    VAL_SIGNO,
    VAL_VERACIDAD,
    VAL_ARREGLO
} ClaseValor;

typedef struct Valor {
    ClaseValor clase;
    int        entero;
    char*      texto;
    int        booleano;
    Tipo       tipo_elemento;   /* solo arreglos */
    struct Valor* primero;      /* primer elemento de un arreglo */
    struct Valor* siguiente;    /* siguiente elemento de un arreglo */
} Valor;

typedef struct VariableRuntime {
    char* nombre;
    Tipo  tipo;
    Valor valor;
    int   ambito;
    struct VariableRuntime* siguiente;
} VariableRuntime;

extern VariableRuntime* variables_runtime;
extern VariableRuntime* variables_runtime_fin;
extern int ambito_runtime;
extern int error_runtime_ocurrido;

void error_runtime(int linea, const char* fmt, ...);

Valor valor_entero(int v);
Valor valor_texto(ClaseValor clase, const char* texto);
Valor valor_booleano(int v);
Valor valor_arreglo(Tipo tipo_elemento);
Valor valor_por_defecto(Tipo tipo);
Valor valor_copiar(const Valor* v);
void  valor_liberar(Valor* v);
void  valor_agregar(Valor* arreglo, const Valor* elemento);
int   valor_quitar(Valor* arreglo);
int   valor_longitud(const Valor* arreglo);
void  valor_imprimir(const Valor* v);
int   valores_iguales(const Valor* a, const Valor* b);
int   valor_a_entero(const Valor* v);

int evaluar_expresion(const Expr* e, Valor* salida);
int ejecutar_sentencia(const Sentencia* s);
int ejecutar_lista(const Sentencia* lista);

void entrar_ambito_runtime(void);
void salir_ambito_runtime(void);
int declarar_variable_runtime(const char* nombre, Tipo tipo, const Valor* valor);
VariableRuntime* buscar_variable_runtime(const char* nombre);
int asignar_variable_runtime(const char* nombre, const Valor* valor);
int agregar_a_arreglo_runtime(const char* nombre, const Valor* valor, int linea);
int quitar_de_arreglo_runtime(const char* nombre, int linea);

#endif
