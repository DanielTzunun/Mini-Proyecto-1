# Nova - compilador en C (Flex / Bison)

## Compilar y ejecutar
    sudo apt install flex bison gcc make
    make
    ./nova ejemplos/correcto.nv

## Archivos
| Archivo | Fase |
|---|---|
| scanner.l | Analizador lexico (Flex): tokens |
| parser.y | Analizador sintactico (Bison): gramatica y construccion del AST |
| ast.h / ast.c | Nodos del AST (lista enlazada), arbol y tabla AST |
| semantic.h / semantic.c | Analizador semantico |
| symtab.h / symtab.c | Tabla de simbolos (lista enlazada) |
| main.c | Ejecuta las fases en orden |

## Lenguaje Nova
    variable:entero edad = 20;       // variable (valor opcional)
    constante:entero iva = 13;       // constante (valor obligatorio)
    guardar 21 en edad;              // asignacion

Tipos: entero, mensaje ("texto"), signo ('a'), veracidad (verdadero/falso).

Las funciones pueden declararse sin parametros ni valor de retorno. Su cuerpo admite declaraciones,
asignaciones, llamadas a otras funciones y condicionales:

    funcion actualizar() {
        variable:entero temporal = 1;
        guardar temporal en edad;
    }
    actualizar();

    variable:veracidad activo = verdadero;
    if (edad >= 18 or activo) {
        guardar 1 en edad;
    } else {
        guardar 0 en edad;
    }

Nova analiza la sintaxis y valida las llamadas; todavia no ejecuta los programas.
Las condiciones de `if` aceptan valores de tipo veracidad, comparaciones (`==`, `!=`, `>`,
`<`, `>=`, `<=`) y el operador `or`. Tambien se aceptan `is`/`is not`, `greater`/`less`
y `||` como aliases. Cada rama crea su propio ambito.

## Reglas semanticas
1. Declarar antes de usar.
2. No declarar dos veces el mismo identificador.
3. El valor debe tener el mismo tipo que la variable.
4. Una constante no se puede modificar.
5. No se puede leer una variable sin valor.

## Ejemplos
- ejemplos/demo.nv, ejemplos/correcto.nv
- ejemplos/condicionales.nv
- ejemplos/errores_semanticos.nv
- ejemplos/errores_sintacticos.nv
- ejemplos/error_lexico.nv
