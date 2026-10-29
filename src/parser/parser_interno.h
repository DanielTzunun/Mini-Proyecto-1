/* parser_interno.h - Declaraciones compartidas del analizador sintactico */
#ifndef PARSER_INTERNO_H
#define PARSER_INTERNO_H

#include "ast.h"
#include "parser.h"

int yylex(void);
Sentencia* anexar_a_lista(Sentencia* inicio, Sentencia* nueva);

#endif
