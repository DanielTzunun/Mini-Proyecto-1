%{
/* scanner.l - Analizador lexico de Nova (Flex) */
#include <string>
#include "ast.hpp"
#include "parser.tab.hpp"

void errorLexico(int linea, const char* msg);
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

"entero"                        { yylval.tipo = Tipo::ENTERO;    return T_TIPO; }
"mensaje"                       { yylval.tipo = Tipo::MENSAJE;   return T_TIPO; }
"signo"                         { yylval.tipo = Tipo::SIGNO;     return T_TIPO; }
"veracidad"                     { yylval.tipo = Tipo::VERACIDAD; return T_TIPO; }

"verdadero"|"falso"             { yylval.str = new std::string(yytext); return T_BOOL; }

-?[0-9]+                        { yylval.str = new std::string(yytext); return T_ENTERO; }
\"[^\"\n]*\"                    { yylval.str = new std::string(yytext); return T_MENSAJE; }
\"[^\"\n]*                      { errorLexico(yylineno, "mensaje sin cerrar (falta la comilla final)"); }
'[^'\\\n]'                      { yylval.str = new std::string(yytext); return T_SIGNO; }

[a-zA-Z_][a-zA-Z0-9_]*          { yylval.str = new std::string(yytext); return T_ID; }

[:=;]                           { return yytext[0]; }

.                               { std::string m = std::string("caracter no reconocido '") + yytext + "'";
                                  errorLexico(yylineno, m.c_str()); }

%%
