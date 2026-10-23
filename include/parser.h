/* parser.h - Interfaz del analizador sintactico de Nova */
#ifndef PARSER_H
#define PARSER_H

extern int errores_sintacticos;

int  yyparse(void);
void yyerror(const char* msg);

#endif
