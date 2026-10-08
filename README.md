# Nova - compilador (C++ / Flex / Bison)

## Compilar y ejecutar
    sudo apt install flex bison g++ make
    make
    ./nova ejemplos/correcto.nv

## Archivos
| Archivo | Fase |
|---|---|
| scanner.l | Analizador lexico (Flex): tokens |
| parser.y | Analizador sintactico (Bison): gramatica y construccion del AST |
| ast.hpp / ast.cpp | Nodos del AST, arbol y tabla AST |
| semantic.hpp / semantic.cpp | Analizador semantico |
| symtab.hpp / symtab.cpp | Tabla de simbolos |
| main.cpp | Ejecuta las fases en orden |

## Lenguaje Nova
    variable:entero edad = 20;       // variable (opcionalmente con valor)
    constante:entero iva = 13;       // constante (valor obligatorio)
    guardar 21 en edad;              // asignacion

Tipos: entero, mensaje ("texto"), signo ('a'), veracidad (verdadero/falso).

## Reglas semanticas
1. Declarar antes de usar.
2. No declarar dos veces el mismo identificador.
3. El valor debe tener el mismo tipo que la variable.
4. Una constante no se puede modificar.
5. No se puede leer una variable sin valor.

## Ejemplos
- ejemplos/correcto.nv
- ejemplos/errores_semanticos.nv
- ejemplos/errores_sintacticos.nv
- ejemplos/error_lexico.nv
