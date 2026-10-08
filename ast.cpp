// ast.cpp - Construccion e impresion del AST
#include "ast.hpp"
#include <cstdio>

Programa programa;

std::string nombreTipo(Tipo t) {
    switch (t) {
        case Tipo::ENTERO:    return "entero";
        case Tipo::MENSAJE:   return "mensaje";
        case Tipo::SIGNO:     return "signo";
        case Tipo::VERACIDAD: return "veracidad";
        default:              return "?";
    }
}

Expr* nuevaExpr(Expr::Clase c, const std::string& texto, int linea, Tipo t) {
    return new Expr{c, texto, linea, t};
}

Sentencia* nuevaSentencia(Sentencia::Clase c, bool esConst, Tipo t,
                          const std::string& id, Expr* valor, int linea) {
    return new Sentencia{c, esConst, t, id, valor, linea};
}

static std::string nombreExpr(const Expr* e) {
    switch (e->clase) {
        case Expr::LIT_ENTERO:    return "LitEntero";
        case Expr::LIT_MENSAJE:   return "LitMensaje";
        case Expr::LIT_SIGNO:     return "LitSigno";
        case Expr::LIT_VERACIDAD: return "LitVeracidad";
        default:                  return "Identificador";
    }
}

static std::string detalleExpr(const Expr* e) {
    std::string d = e->texto;
    if (e->tipo != Tipo::DESCONOCIDO) d += " : " + nombreTipo(e->tipo);
    return d;
}

static std::string detalleSent(const Sentencia* s) {
    if (s->clase == Sentencia::DECLARACION)
        return std::string(s->esConstante ? "constante " : "variable ") + s->id + " : " + nombreTipo(s->tipo);
    return "guardar en " + s->id;
}

void imprimirArbol(const Programa& p) {
    printf("Programa\n");
    for (size_t i = 0; i < p.size(); ++i) {
        const Sentencia* s = p[i];
        bool ultima = (i + 1 == p.size());
        printf(" %s %s %s [linea %d]\n", ultima ? "'--" : "|--",
               s->clase == Sentencia::DECLARACION ? "Declaracion" : "Asignacion",
               detalleSent(s).c_str(), s->linea);
        if (s->valor)
            printf(" %s     '-- Valor: %s %s\n", ultima ? " " : "|",
                   nombreExpr(s->valor).c_str(), detalleExpr(s->valor).c_str());
    }
}

void imprimirTablaAST(const Programa& p) {
    printf("%-4s %-14s %-6s %-34s %-5s\n", "#", "Nodo", "Padre", "Detalle", "Linea");
    printf("---- -------------- ------ ---------------------------------- -----\n");
    int n = 0;
    for (const Sentencia* s : p) {
        int idSent = ++n;
        printf("%-4d %-14s %-6d %-34s %-5d\n", idSent,
               s->clase == Sentencia::DECLARACION ? "Declaracion" : "Asignacion",
               0, detalleSent(s).c_str(), s->linea);
        if (s->valor) {
            ++n;
            printf("%-4d %-14s %-6d %-34s %-5d\n", n, nombreExpr(s->valor).c_str(),
                   idSent, detalleExpr(s->valor).c_str(), s->valor->linea);
        }
    }
}
