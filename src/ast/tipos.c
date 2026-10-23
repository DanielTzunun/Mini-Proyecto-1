/* tipos.c - Nombres y relaciones entre los tipos de Nova */
#include "ast.h"

const char* nombre_tipo(Tipo t) {
    switch (t) {
        case TIPO_ENTERO:          return "entero";
        case TIPO_MENSAJE:         return "mensaje";
        case TIPO_SIGNO:           return "signo";
        case TIPO_VERACIDAD:       return "veracidad";
        case TIPO_ARREGLO_ENTERO:  return "entero[]";
        case TIPO_ARREGLO_MENSAJE: return "mensaje[]";
        case TIPO_ARREGLO_SIGNO:   return "signo[]";
        case TIPO_ARREGLO_VERACIDAD: return "veracidad[]";
        default:                   return "?";
    }
}

int es_tipo_arreglo(Tipo t) {
    return t == TIPO_ARREGLO_ENTERO || t == TIPO_ARREGLO_MENSAJE ||
           t == TIPO_ARREGLO_SIGNO || t == TIPO_ARREGLO_VERACIDAD;
}

Tipo tipo_elemento(Tipo t) {
    switch (t) {
        case TIPO_ARREGLO_ENTERO:    return TIPO_ENTERO;
        case TIPO_ARREGLO_MENSAJE:   return TIPO_MENSAJE;
        case TIPO_ARREGLO_SIGNO:     return TIPO_SIGNO;
        case TIPO_ARREGLO_VERACIDAD: return TIPO_VERACIDAD;
        default:                     return TIPO_DESCONOCIDO;
    }
}

Tipo tipo_arreglo(Tipo elemento) {
    switch (elemento) {
        case TIPO_ENTERO:    return TIPO_ARREGLO_ENTERO;
        case TIPO_MENSAJE:   return TIPO_ARREGLO_MENSAJE;
        case TIPO_SIGNO:     return TIPO_ARREGLO_SIGNO;
        case TIPO_VERACIDAD: return TIPO_ARREGLO_VERACIDAD;
        default:             return TIPO_DESCONOCIDO;
    }
}
