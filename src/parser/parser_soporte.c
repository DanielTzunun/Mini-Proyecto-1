/* parser_soporte.c - Funciones de apoyo del analizador sintactico */
#include <stdio.h>
#include <string.h>
#include "parser_interno.h"
#include "parser.tab.h"

int errores_sintacticos = 0;

Sentencia* anexar_a_lista(Sentencia* inicio, Sentencia* nueva) {
    if (!inicio) return nueva;
    Sentencia* ultimo = inicio;
    while (ultimo->siguiente) ultimo = ultimo->siguiente;
    ultimo->siguiente = nueva;
    return inicio;
}

/* Reemplaza todas las apariciones de 'a' por 'b' dentro de s */
static void reemplazar(char* s, size_t cap, const char* a, const char* b) {
    char tmp[512];
    size_t i = 0, j = 0, la = strlen(a), lb = strlen(b);
    while (s[i] && j < sizeof(tmp) - 1) {
        if (strncmp(s + i, a, la) == 0) {
            for (size_t k = 0; k < lb && j < sizeof(tmp) - 1; k++) tmp[j++] = b[k];
            i += la;
        } else tmp[j++] = s[i++];
    }
    tmp[j] = '\0';
    strncpy(s, tmp, cap - 1);
    s[cap - 1] = '\0';
}

void yyerror(const char* msg) {
    char m[512];
    snprintf(m, sizeof m, "%s", msg);
    reemplazar(m, sizeof m, "syntax error, ", "");
    reemplazar(m, sizeof m, "syntax error", "sintaxis invalida");
    reemplazar(m, sizeof m, "unexpected ", "se encontro ");
    reemplazar(m, sizeof m, ", expecting ", ", se esperaba ");
    reemplazar(m, sizeof m, " or ", " o ");
    reemplazar(m, sizeof m, "end of file", "fin del archivo");
    if (strstr(m, "se esperaba ';'") && strlen(m) < 400)
        strcat(m, " (revise si falta ';' al final de la linea anterior)");
    errores_sintacticos++;
    printf("Error sintactico (linea %d): %s\n", yylloc.first_line, m);
}
