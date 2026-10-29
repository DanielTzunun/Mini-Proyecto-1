/* runtime.c - Punto de entrada del interprete de Nova */
#include <stdio.h>
#include <stdarg.h>
#include "runtime.h"
#include "runtime_interno.h"

VariableRuntime* variables_runtime = NULL;
VariableRuntime* variables_runtime_fin = NULL;
int ambito_runtime = 0;
int error_runtime_ocurrido = 0;

void error_runtime(int linea, const char* fmt, ...) {
    char cuerpo[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(cuerpo, sizeof cuerpo, fmt, args);
    va_end(args);

    error_runtime_ocurrido = 1;
    printf("Error en ejecucion (linea %d): %s\n", linea, cuerpo);
}

int ejecutar_programa(const Sentencia* programa) {
    variables_runtime = NULL;
    variables_runtime_fin = NULL;
    ambito_runtime = 0;
    error_runtime_ocurrido = 0;

    int ok = ejecutar_lista(programa);
    return ok && !error_runtime_ocurrido;
}
