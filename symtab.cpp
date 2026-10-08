// symtab.cpp
#include "symtab.hpp"
#include <cstdio>

bool TablaSimbolos::declarar(const Simbolo& s) {
    if (indice.count(s.nombre)) return false;
    indice[s.nombre] = simbolos.size();
    simbolos.push_back(s);
    return true;
}

Simbolo* TablaSimbolos::buscar(const std::string& nombre) {
    auto it = indice.find(nombre);
    return it == indice.end() ? nullptr : &simbolos[it->second];
}

void TablaSimbolos::imprimir() const {
    printf("%-14s %-10s %-10s %-13s %-14s %-5s\n",
           "Nombre", "Tipo", "Categoria", "Inicializada", "Valor", "Linea");
    printf("-------------- ---------- ---------- ------------- -------------- -----\n");
    for (const Simbolo& s : simbolos)
        printf("%-14s %-10s %-10s %-13s %-14s %-5d\n", s.nombre.c_str(),
               nombreTipo(s.tipo).c_str(), s.esConstante ? "constante" : "variable",
               s.inicializada ? "si" : "no", s.valor.c_str(), s.linea);
}
