/* main.c - Programa principal del compilador Nova */
#include <stdio.h>
#include "ast.h"
#include "symtab.h"
#include "semantic.h"

extern FILE* yyin;
int yyparse(void);

int errores_lexicos = 0;
int errores_sintacticos = 0;

void error_lexico(int linea, const char* msg) {
    errores_lexicos++;
    printf("Error lexico (linea %d): %s\n", linea, msg);
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("Uso: %s programa.nv\n", argv[0]); return 2; }
    yyin = fopen(argv[1], "r");
    if (!yyin) { printf("No se pudo abrir '%s'\n", argv[1]); return 2; }

    printf("=== NOVA: compilando %s ===\n\n", argv[1]);

    /* 1) Analisis lexico + sintactico (construye el AST) */
    printf("--- ANALISIS LEXICO Y SINTACTICO ---\n");
    yyparse();
    fclose(yyin);
    if (errores_lexicos + errores_sintacticos > 0) {
        printf("\nCompilacion detenida: %d error(es) lexico(s), %d error(es) sintactico(s).\n",
               errores_lexicos, errores_sintacticos);
        return 1;
    }
    printf("OK: el programa cumple la gramatica de Nova.\n\n");

    /* 2) Analisis semantico (llena la tabla de simbolos y anota tipos en el AST) */
    analizar_semantica();

    /* 3) AST */
    printf("--- AST (arbol) ---\n");
    imprimir_arbol();
    printf("\n--- AST (tabla) ---\n");
    imprimir_tabla_ast();

    /* 4) Resultado semantico */
    printf("\n--- ANALISIS SEMANTICO ---\n");
    int n = total_errores_semanticos();
    if (n == 0) printf("OK: sin errores semanticos.\n");
    else for (int i = 0; i < n; i++) printf("%s\n", error_semantico(i));

    /* 5) Tabla de simbolos */
    printf("\n--- TABLA DE SIMBOLOS ---\n");
    imprimir_tabla_simbolos();

    if (n > 0) { printf("\nCompilacion con %d error(es) semantico(s).\n", n); return 1; }
    printf("\nCompilacion exitosa.\n");
    return 0;
}
