// =============================================================================
// main.cpp — Punto de entrada del compilador
// =============================================================================
// Flujo de compilación:
//   1. Leer el archivo fuente (.fun)
//   2. Scanner  → token stream
//   3. Parser   → AST
//   4. TypeChecker (dentro de GenCodeVisitor::generar)  → análisis semántico
//   5. GenCodeVisitor → emite código x86-64 AT&T en un archivo .s
//
// Uso:
//   ./compilador <archivo.fun>
//
// Produce:
//   <archivo>.s  — ensamblador listo para ensamblar con gcc/as
// =============================================================================

#include <fstream>
#include <iostream>
#include <string>
#include "ast.h"

#include "parser.h"
#include "scanner.h"
#include "visitor.h"

int main(int argc, const char* argv[]) {
    // ---- Validar argumentos ----
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo_de_entrada>\n";
        return 1;
    }

    // ---- Leer archivo fuente ----
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo '" << argv[1] << "'\n";
        return 1;
    }

    std::string input, line;
    while (std::getline(infile, line))
        input += line + '\n';
    infile.close();

    // ---- Análisis léxico + sintáctico ----
    Scanner scanner(input.c_str());
    Parser  parser(&scanner);

    Program* program = nullptr;
    try {
        program = parser.parseProgram();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    // ---- Construir nombre del archivo de salida ----
    std::string inputFile(argv[1]);
    size_t dotPos = inputFile.find_last_of('.');
    std::string baseName   = (dotPos == std::string::npos) ? inputFile
                                                           : inputFile.substr(0, dotPos);
    std::string outputFile = baseName + ".s";

    std::ofstream outfile(outputFile);
    if (!outfile.is_open()) {
        std::cerr << "Error: no se pudo crear el archivo de salida '" << outputFile << "'\n";
        delete program;
        return 1;
    }

    // ---- Optimización: plegado de constantes ----

    Opt1Visitor opt1;
    opt1.Opt1(program);

    Opt3Visitor opt3;
    opt3.Opt3(program);

    Opt2Visitor opt2;
    opt2.Opt2(program);

    // ---- Análisis semántico + generación de código ----
    try {
        std::cout << "Generando codigo ensamblador en '" << outputFile << "'...\n";
        GenCodeVisitor codegen(outfile);
        codegen.generar(program);
        std::cout << "Compilación exitosa.\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        outfile.close();
        delete program;
        return 1;
    }

    outfile.close();
    delete program;
    return 0;
}
