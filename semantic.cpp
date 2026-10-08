// semantic.cpp - Reglas semanticas:
//  1. Toda variable debe declararse antes de usarse
//  2. No se puede declarar dos veces el mismo identificador
//  3. El tipo del valor debe coincidir con el tipo de la variable
//  4. Una constante no se puede modificar con "guardar"
//  5. No se puede leer una variable que no tiene valor
#include "semantic.hpp"

void AnalizadorSemantico::error(int linea, const std::string& msg) {
    msgs.push_back("Error semantico (linea " + std::to_string(linea) + "): " + msg);
}

Tipo AnalizadorSemantico::tipoDe(Expr* e) {
    if (e->clase != Expr::IDENT) return e->tipo;          // literal: ya trae su tipo
    Simbolo* s = tabla.buscar(e->texto);
    if (!s) {
        error(e->linea, "la variable '" + e->texto + "' no ha sido declarada");
        return Tipo::DESCONOCIDO;
    }
    if (!s->inicializada) {
        error(e->linea, "la variable '" + e->texto + "' se usa pero todavia no tiene valor");
        e->tipo = s->tipo;
        return Tipo::DESCONOCIDO;
    }
    e->tipo = s->tipo;
    return s->tipo;
}

// Valor textual conocido de una expresion (si es un identificador, se usa su ultimo valor)
static std::string valorTexto(TablaSimbolos& tabla, const Expr* e) {
    if (e->clase != Expr::IDENT) return e->texto;
    Simbolo* s = tabla.buscar(e->texto);
    return s ? s->valor : "-";
}

void AnalizadorSemantico::analizar(Programa& p) {
    for (Sentencia* s : p) {
        if (s->clase == Sentencia::DECLARACION) {
            Simbolo* previo = tabla.buscar(s->id);
            if (previo)
                error(s->linea, "'" + s->id + "' ya fue declarada en la linea " + std::to_string(previo->linea));

            bool ok = true;
            std::string valor = "-";
            if (s->valor) {
                Tipo tv = tipoDe(s->valor);
                if (tv == Tipo::DESCONOCIDO) ok = false;
                else if (tv != s->tipo) {
                    error(s->linea, "tipo incompatible: '" + s->id + "' es " + nombreTipo(s->tipo) +
                                    " pero se le da un valor de tipo " + nombreTipo(tv));
                    ok = false;
                }
                if (ok) valor = valorTexto(tabla, s->valor);
            }
            if (!previo)
                tabla.declarar({s->id, s->tipo, s->esConstante, s->valor != nullptr && ok, valor, s->linea});
        } else {  // ASIGNACION: guardar valor en id
            Simbolo* dest = tabla.buscar(s->id);
            Tipo tv = tipoDe(s->valor);
            if (!dest) {
                error(s->linea, "la variable '" + s->id + "' no ha sido declarada");
                continue;
            }
            if (dest->esConstante) {
                error(s->linea, "'" + s->id + "' es una constante y no se puede modificar");
                continue;
            }
            if (tv != Tipo::DESCONOCIDO && tv != dest->tipo) {
                error(s->linea, "tipo incompatible: '" + s->id + "' es " + nombreTipo(dest->tipo) +
                                " pero se le da un valor de tipo " + nombreTipo(tv));
                continue;
            }
            if (tv != Tipo::DESCONOCIDO) {
                dest->inicializada = true;
                dest->valor = valorTexto(tabla, s->valor);
            }
        }
    }
}
