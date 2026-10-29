# Nova - compilador e interprete en C (Flex / Bison)

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
| `include/runtime.h` | Interfaz de ejecucion |
| `src/runtime/runtime.c` | Punto de entrada del interprete |
| `src/runtime/valores.c` | Valores y operaciones basicas |
| `src/runtime/expresiones.c` | Evaluacion de expresiones |
| `src/runtime/sentencias.c` | Ejecucion de sentencias y variables |
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

Las condiciones de `if` aceptan valores de tipo veracidad, comparaciones (`==`, `!=`, `>`,
`<`, `>=`, `<=`) y el operador `or`. Tambien se aceptan `is`/`is not`, `greater`/`less`
y `||` como aliases. Cada rama crea su propio ambito.

## Expresiones aritmeticas

Nova admite sumas, restas, multiplicaciones y divisiones enteras, con parentesis y negacion:

    variable:entero a = 10;
    variable:entero b = 3;

    mostrar a + b;              // 13
    mostrar a - b;              // 7
    mostrar a * b;              // 30
    mostrar a / b;              // 3 (division entera)
    mostrar (a + b) * 2;        // 26
    mostrar -a;                 // -10
    mostrar a - -b;             // 13

Precedencia, de mayor a menor:

1. Parentesis y acceso por indice `[]`
2. Negacion unaria `-`
3. `*` y `/`
4. `+` y `-`
5. Comparaciones (`==`, `!=`, `>`, `<`, `>=`, `<=`)
6. `or`

La division es entera. Si el divisor es cero, se reporta un error en tiempo de ejecucion.

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

## Ejecucion

La instruccion `mostrar` evalua una expresion y la imprime en pantalla:

    variable:entero[] numeros = [10, 20, 30];
    variable:mensaje saludo = "Hola Nova";

    mostrar saludo;                 // Hola Nova
    mostrar numeros;                // [10, 20, 30]
    mostrar numeros[1];             // 20
    mostrar numeros[0] == 10;       // verdadero

    agregar 40 en numeros;
    mostrar numeros;                // [10, 20, 30, 40]

    quitar de numeros;
    mostrar numeros;                // [10, 20, 30]

La ejecucion respeta los ambitos de funciones y de las ramas de `if`. Tambien detecta
errores en tiempo de ejecucion, por ejemplo un indice fuera de rango o `quitar` sobre
un arreglo vacio.

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
- `ejemplos/aritmetica.nv`
- `ejemplos/ejecucion.nv`
- `ejemplos/errores_semanticos.nv`
- `ejemplos/errores_arreglos.nv`
- `ejemplos/errores_sintacticos.nv`
- `ejemplos/error_lexico.nv`
- `ejemplos/error_ejecucion.nv`
- `ejemplos/error_aritmetica.nv`
