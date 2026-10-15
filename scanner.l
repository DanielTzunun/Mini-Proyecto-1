%{
/* scanner.l - Analizador lexico de Nova (Flex) */
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

void error_lexico(int linea, const char* msg);
#define YY_USER_ACTION yylloc.first_line = yylineno;
%}

%option noyywrap
%option yylineno

%%

[ \t\r\n]+                      { /* espacios y saltos de linea: se ignoran */ }
"//".*                          { /* comentario de una linea: se ignora */ }

"variable"                      { return T_VARIABLE; }
"constante"                     { return T_CONSTANTE; }
"guardar"                       { return T_GUARDAR; }
"en"                            { return T_EN; }
"funcion"                       { return T_FUNCION; }

"entero"                        { yylval.tipo = TIPO_ENTERO;    return T_TIPO; }
"mensaje"                       { yylval.tipo = TIPO_MENSAJE;   return T_TIPO; }
"signo"                         { yylval.tipo = TIPO_SIGNO;     return T_TIPO; }
"veracidad"                     { yylval.tipo = TIPO_VERACIDAD; return T_TIPO; }

"verdadero"|"falso"             { yylval.str = copiar_cadena(yytext); return T_BOOL; }



-?[0-9]+                        { yylval.str = copiar_cadena(yytext); return T_ENTERO; }
\"[^\"\n]*\"                    { yylval.str = copiar_cadena(yytext); return T_MENSAJE; }
\"[^\"\n]*                      { error_lexico(yylineno, "mensaje sin cerrar (falta la comilla final)"); }
'[^'\\\n]'                      { yylval.str = copiar_cadena(yytext); return T_SIGNO; }

[a-zA-Z_][a-zA-Z0-9_]*          { yylval.str = copiar_cadena(yytext); return T_ID; }

[:=;(){}]                       { return yytext[0]; }

.                               { char m[64];
                                  snprintf(m, sizeof m, "caracter no reconocido '%s'", yytext);
                                  error_lexico(yylineno, m); }

%%
