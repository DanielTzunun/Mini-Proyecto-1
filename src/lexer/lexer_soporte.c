/* lexer_soporte.c - Funciones de apoyo del analizador lexico */
#include <stdio.h>
#include "lexer.h"

int errores_lexicos = 0;

void error_lexico(int linea, const char* msg) {
    errores_lexicos++;
    printf("Error lexico (linea %d): %s\n", linea, msg);
}
