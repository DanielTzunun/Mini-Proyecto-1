/* lexer.h - Interfaz del analizador lexico de Nova */
#ifndef LEXER_H
#define LEXER_H

extern int errores_lexicos;

void error_lexico(int linea, const char* msg);

#endif
