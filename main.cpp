// main.cpp - Programa principal del compilador Nova
#include <cstdio>
#include "ast.hpp"
#include "symtab.hpp"
#include "semantic.hpp"

extern FILE* yyin;
int yyparse();

int erroresLexicos = 0;
int erroresSintacticos = 0;

void errorLexico(int linea, const char* msg) {
    ++erroresLexicos;
    printf("Error lexico (linea %d): %s\n", linea, msg);
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("Uso: %s programa.nv\n", argv[0]); return 2; }
    yyin = fopen(argv[1], "r");
    if (!yyin) { printf("No se pudo abrir '%s'\n", argv[1]); return 2; }

    printf("=== NOVA: compilando %s ===\n\n", argv[1]);

    // 1) Analisis lexico + sintactico
    printf("--- ANALISIS LEXICO Y SINTACTICO ---\n");
    yyparse();
    fclose(yyin);
    if (erroresLexicos + erroresSintacticos > 0) {
        printf("\nCompilacion detenida: %d error(es) lexico(s), %d error(es) sintactico(s).\n",
               erroresLexicos, erroresSintacticos);
        return 1;
    }
    printf("OK: el programa cumple la gramatica de Nova.\n\n");

    // 2) Analisis semantico (llena la tabla de simbolos)
    TablaSimbolos tabla;
    AnalizadorSemantico semantico(tabla);
    semantico.analizar(programa);

    // 3) AST (ya anotado con tipos por el analizador semantico)
    printf("--- AST (arbol) ---\n");
    imprimirArbol(programa);
    printf("\n--- AST (tabla) ---\n");
    imprimirTablaAST(programa);

    // 4) Resultado semantico
    printf("\n--- ANALISIS SEMANTICO ---\n");
    if (semantico.errores().empty()) printf("OK: sin errores semanticos.\n");
    else for (const auto& e : semantico.errores()) printf("%s\n", e.c_str());

    // 5) Tabla de simbolos
    printf("\n--- TABLA DE SIMBOLOS ---\n");
    tabla.imprimir();

    if (!semantico.errores().empty()) {
        printf("\nCompilacion con %zu error(es) semantico(s).\n", semantico.errores().size());
        return 1;
    }
    printf("\nCompilacion exitosa.\n");
    return 0;
}
