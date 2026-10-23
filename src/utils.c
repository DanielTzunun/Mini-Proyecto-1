/* utils.c - Utilidades comunes de Nova */
#include <stdlib.h>
#include <string.h>
#include "utils.h"

char* copiar_cadena(const char* s) {
    char* r = malloc(strlen(s) + 1);
    strcpy(r, s);
    return r;
}
