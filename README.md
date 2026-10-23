# Nova - compilador en C (Flex / Bison)

## Compilar y ejecutar

    sudo apt install flex bison gcc make
    make
    ./nova ejemplos/correcto.nv

Tambien se puede usar `make run` para compilar y ejecutar `ejemplos/correcto.nv`.

## Estructura del proyecto

| Ruta | Fase |
|---|---|
| `include/ast.h` | Tipos, nodos del AST y constructores |
| `src/ast/ast.c` | Construccion del AST |
| `src/ast/tipos.c` | Nombres y relaciones entre tipos |
| `src/ast/ast_impresion.c` | Impresion del AST (arbol y tabla) |
| `src/lexer/lexer.l` | Analizador lexico (Flex): tokens |
| `src/lexer/lexer_soporte.c` | Reporte de errores lexicos |
| `src/parser/parser.y` | Gramatica (Bison) y construccion del AST |
| `src/parser/parser_soporte.c` | Apoyo del parser y mensajes de error |
| `src/semantico/semantico.c` | Analizador semantico |
| `src/tabla_simbolos.c` | Tabla de simbolos (lista enlazada) |
| `src/utils.c` | Utilidades comunes |
| `src/main.c` | Ejecuta las fases en orden |

Los encabezados publicos estan en `include/` y los archivos generados por Flex/Bison en `build/`.

## Lenguaje Nova

    variable:entero edad = 20;       // variable (valor opcional)
    constante:entero iva = 13;       // constante (valor obligatorio)
    guardar 21 en edad;              // asignacion

Tipos: `entero`, `mensaje` ("texto"), `signo` ('a'), `veracidad` (`verdadero`/`falso`).

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

## Arreglos

Un arreglo se declara agregando `[]` despues del tipo. Su literal se escribe entre corchetes
y todos sus elementos deben ser del mismo tipo:

    variable:entero[] numeros = [1, 2, 3];
    variable:mensaje[] nombres = [];
    variable:signo[] letras = ['a', 'b'];

    variable:entero primero = numeros[0];
    guardar numeros[1] en primero;

    agregar 4 en numeros;   // agrega al final
    quitar de numeros;      // quita el ultimo elemento

Reglas semanticas de los arreglos:

1. Solo se permite un nivel de arreglos (no hay arreglos de arreglos).
2. Todos los elementos del literal deben tener el mismo tipo.
3. El indice debe ser de tipo `entero`.
4. Solo se puede indexar un valor de tipo arreglo.
5. No se pueden comparar arreglos entre si.
6. `agregar` exige un arreglo y un valor del tipo de sus elementos.
7. `quitar` exige un arreglo que ya tenga valor.
8. Una constante de tipo arreglo no se puede modificar.

## Reglas semanticas generales

1. Declarar antes de usar.
2. No declarar dos veces el mismo identificador.
3. El valor debe tener el mismo tipo que la variable.
4. Una constante no se puede modificar.
5. No se puede leer una variable sin valor.

## Ejemplos

- `ejemplos/correcto.nv`
- `ejemplos/funciones.nv`
- `ejemplos/condicionales.nv`
- `ejemplos/arreglos.nv`
- `ejemplos/errores_semanticos.nv`
- `ejemplos/errores_arreglos.nv`
- `ejemplos/errores_sintacticos.nv`
- `ejemplos/error_lexico.nv`
