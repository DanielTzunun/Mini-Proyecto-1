// symtab.hpp - Tabla de simbolos de Nova
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "ast.hpp"

struct Simbolo {
    std::string nombre;
    Tipo tipo;
    bool esConstante;
    bool inicializada;
    std::string valor;   // ultimo valor conocido (texto del literal) o "-"
    int linea;           // linea de declaracion
};

class TablaSimbolos {
public:
    bool declarar(const Simbolo& s);              // false si ya existe
    Simbolo* buscar(const std::string& nombre);   // nullptr si no existe
    void imprimir() const;
private:
    std::vector<Simbolo> simbolos;                       // conserva el orden de declaracion
    std::unordered_map<std::string, size_t> indice;      // busqueda rapida por nombre
};
