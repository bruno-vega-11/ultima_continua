#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

// =============================================================================
// environment.h — Entorno de variables con alcance anidado (scoped environment)
// =============================================================================
// Implementa un entorno genérico de variables usando una pila de tablas hash
// (ribs). Cada nivel representa un scope (función, bloque, etc.).
//
// Operaciones principales:
//   add_level()    — abre un nuevo scope
//   remove_level() — cierra el scope más interno
//   add_var()      — declara una variable en el scope actual
//   check()        — verifica si una variable existe en algún scope
//   lookup()       — obtiene el valor de una variable
//   update()       — actualiza el valor de una variable existente
// =============================================================================

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
class Environment {
private:
    // Pila de tablas: cada tabla es un scope (rib)
    std::vector<std::unordered_map<std::string, T>> ribs;

    // Busca en qué rib está la variable; devuelve el índice o -1
    int search_rib(const std::string& var) const {
        for (int idx = static_cast<int>(ribs.size()) - 1; idx >= 0; --idx) {
            if (ribs[idx].count(var))
                return idx;
        }
        return -1;
    }

public:
    Environment() = default;

    // ---- Gestión de scopes ----

    // Abre un nuevo nivel de scope
    void add_level() {
        ribs.emplace_back();
    }

    // Cierra el nivel más interno; devuelve false si no hay niveles
    bool remove_level() {
        if (ribs.empty()) return false;
        ribs.pop_back();
        return true;
    }

    // Limpia todos los scopes
    void clear() {
        ribs.clear();
    }

    // ---- Gestión de variables ----

    // Declara una variable con valor explícito en el scope actual
    void add_var(const std::string& var, const T& value) {
        if (ribs.empty()) {
            std::cerr << "[Error interno] Environment sin niveles al agregar '" << var << "'\n";
            exit(EXIT_FAILURE);
        }
        ribs.back()[var] = value;
    }

    // Declara una variable con valor por defecto en el scope actual
    void add_var(const std::string& var) {
        if (ribs.empty()) {
            std::cerr << "[Error interno] Environment sin niveles al agregar '" << var << "'\n";
            exit(EXIT_FAILURE);
        }
        ribs.back()[var] = T{};
    }

    // Actualiza el valor de una variable en el scope donde fue declarada
    bool update(const std::string& x, const T& v) {
        int idx = search_rib(x);
        if (idx < 0) return false;
        ribs[idx][x] = v;
        return true;
    }

    // ---- Consultas ----

    // Devuelve true si la variable existe en algún scope
    bool check(const std::string& x) const {
        return search_rib(x) >= 0;
    }

    // Devuelve el valor de la variable; valor por defecto si no existe
    T lookup(const std::string& x) const {
        int idx = search_rib(x);
        if (idx < 0) {
            std::cerr << "[Advertencia] Variable no encontrada: '" << x << "'\n";
            return T{};
        }
        return ribs[idx].at(x);
    }

    // Versión con salida por referencia; devuelve false si no existe
    bool lookup(const std::string& x, T& v) const {
        int idx = search_rib(x);
        if (idx < 0) return false;
        v = ribs[idx].at(x);
        return true;
    }
};

#endif // ENVIRONMENT_H
