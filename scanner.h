#ifndef SCANNER_H
#define SCANNER_H

// =============================================================================
// scanner.h — Definición del Scanner (analizador léxico)
// =============================================================================
// El Scanner lee una cadena de entrada carácter a carácter y produce tokens
// uno por uno mediante el método nextToken().
// =============================================================================

#include <string>
#include "token.h"

class Scanner {
private:
    std::string input;      // Cadena de entrada completa
    std::size_t first;      // Inicio del lexema actual
    std::size_t current;    // Posición actual de lectura

public:
    Scanner(const char* in_s);
    Token* nextToken();
    ~Scanner();
};

// Función de prueba: tokeniza el archivo y escribe los tokens a <archivo>_tokens.txt
int ejecutar_scanner(Scanner* scanner, const std::string& InputFile);

#endif // SCANNER_H
