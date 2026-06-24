#ifndef VISITOR_H
#define VISITOR_H

// =============================================================================
// visitor.h — Definición de los visitantes del AST
// =============================================================================
// Se implementan dos visitantes sobre el patrón Visitor:
//
//   TypeCheckerVisitor — Análisis semántico:
//     · Verifica que las variables usadas hayan sido declaradas.
//     · Verifica que las funciones llamadas existan.
//     · Verifica que las llamadas tengan el número correcto de argumentos.
//     · Cuenta las variables locales de cada función para la generación.
//
//   GenCodeVisitor — Generación de código x86-64 (AT&T syntax):
//     · Emite ensamblador para cada nodo del AST.
//     · Usa TypeCheckerVisitor para obtener metadatos (tamaño de frame, etc.).
// =============================================================================

#include <map>

#include "ast.h"
#include "environment.h"
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class BinaryExp;
class Body;
class BreakStm;
class DoWhileStm;
class FunDec;
class FcallExp;
class IdExp;
class IndexExp;
class IfStm;
class NumberExp;
class PrintStm;
class Program;
class ReturnStm;
class Stm;
class StructDec;
class SwitchStm;
class UnaryExp;
class VarDec;
class WhileStm;
class ExpListSize;
class ExpListVals;
class ExpMatrixSize;
class ExpMatrixVals;
class FieldExp;
class MatrixExp;

// =============================================================================
// Clase base abstracta Visitor
// =============================================================================

class Visitor {
public:
  virtual int visit(BinaryExp *exp) = 0;
  virtual int visit(NumberExp *exp) = 0;
  virtual int visit(IdExp *exp) = 0;
  virtual int visit(UnaryExp *exp) = 0;
  virtual int visit(IndexExp *exp) = 0;
  virtual int visit(MatrixExp *exp) = 0;
  virtual int visit(FieldExp *exp) = 0;
  virtual int visit(Program *p) = 0;
  virtual int visit(StructDec *sd) = 0;
  virtual int visit(PrintStm *stm) = 0;
  virtual int visit(WhileStm *stm) = 0;
  virtual int visit(DoWhileStm *stm) = 0;
  virtual int visit(IfStm *stm) = 0;
  virtual int visit(AssignStm *stm) = 0;
  virtual int visit(ExpListSize *stm) = 0;
  virtual int visit(ExpListVals *stm) = 0;
  virtual int visit(ExpMatrixSize *stm) = 0;
  virtual int visit(ExpMatrixVals *stm) = 0;
  virtual int visit(BreakStm *stm) = 0;
  virtual int visit(SwitchStm *stm) = 0;
  virtual int visit(Body *body) = 0;
  virtual int visit(VarDec *vd) = 0;
  virtual int visit(FcallExp *fc) = 0;
  virtual int visit(ReturnStm *r) = 0;
  virtual int visit(FunDec *fd) = 0;
};

// =============================================================================
// TypeCheckerVisitor — Análisis semántico
// =============================================================================
class Opt1Visitor : public Visitor {
public:
  int Opt1(Program *program);
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int visit(MatrixExp *exp) override;
  int visit(FieldExp *exp) override;
  int visit(Program *p) override;
  int visit(StructDec *sd) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(ExpMatrixSize *stm) override;
  int visit(ExpMatrixVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
};


class Opt2Visitor : public Visitor {
public:
  int Opt2(Program *program);
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int visit(MatrixExp *exp) override;
  int visit(FieldExp *exp) override;
  int visit(Program *p) override;
  int visit(StructDec *sd) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(ExpMatrixSize *stm) override;
  int visit(ExpMatrixVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
};

class Opt3Visitor : public Visitor {
public:
  int Opt3(Program *program);
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int visit(MatrixExp *exp) override;
  int visit(FieldExp *exp) override;
  int visit(Program *p) override;
  int visit(StructDec *sd) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(ExpMatrixSize *stm) override;
  int visit(ExpMatrixVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
};

//////////////////////////////
class TypeCheckerVisitor : public Visitor {
public:
  // Mapa función → número total de variables en el frame (parámetros + locales)
  std::unordered_map<std::string, int> funcontador;

  // Mapa función → número de parámetros (para verificar aridad en llamadas)
  std::unordered_map<std::string, int> funAridad;
  std::unordered_map<std::string, int> structFields;
  std::unordered_map<std::string, std::unordered_map<std::string, int>>
      structFieldOffsets;

  // Contador de variables locales del cuerpo actual (acumulado por
  // visit(VarDec))
  int locales;

  // Entorno de variables: lleva el alcance (scope) actual
  Environment<int> entorno;
  Environment<std::string> tiposVar;

  // Nombre de la función que se está analizando (para mensajes de error)
  std::string funcionActual;

  // Punto de entrada del análisis semántico
  int TypeChecker(Program *program);

  // ---- Visitas ----
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int visit(MatrixExp *exp) override;
  int visit(FieldExp *exp) override;
  int visit(Program *p) override;
  int visit(StructDec *sd) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(ExpMatrixSize *stm) override;
  int visit(ExpMatrixVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
};

// =============================================================================
// GenCodeVisitor — Generación de código ensamblador x86-64
// =============================================================================

// -----------------------------------------------------------------------------
// LVal — captura de una expresión lvalue para asignación/store
//
// Agrupa todos los campos que antes vivían sueltos como captured* en la clase.
// captureLVal(Exp*) llena un LVal por valor; los visit() que actúan como
// lvalue comprueban si reciben un LVal* != nullptr en lugar del bool captureMode.
// -----------------------------------------------------------------------------
enum class LValKind {
  Invalid,
  Number,
  Id,
  ListSize,
  ListVals,
  MatrixSize,
  MatrixVals,
  Index,
  Matrix,
  Field
};

struct LVal {
  LValKind          kind        = LValKind::Invalid;
  std::string       name;
  Exp              *index       = nullptr;
  Exp              *row         = nullptr;
  Exp              *col         = nullptr;
  std::string       object;
  std::string       field;
  std::string       type;
  std::vector<Exp*> values;
  bool              numIsConst  = false;
  int               numVal      = 0;
};

class GenCodeVisitor : public Visitor {
private:
  std::ostream &out; // Stream de salida (archivo .s)
  LVal *lvalTarget = nullptr;
  LVal captureLVal(Exp *exp);
  int storeTarget(const LVal &lv);
  int storeId    (const LVal &lv);
  int storeIndex (const LVal &lv);
  int storeMatrix(const LVal &lv);
  int storeField (const LVal &lv);

public:
  bool necesitaPotencia = false;
  TypeCheckerVisitor tipos; 
  std::unordered_map<std::string, int> funcontador;
  std::unordered_map<std::string, int> structFields;
  std::unordered_map<std::string, std::unordered_map<std::string, int>> structFieldOffsets;
  std::unordered_map<std::string, int> memoria;
  std::unordered_map<std::string, std::string> variableTypes;
  std::unordered_map<std::string, bool> structAllocated;
  std::unordered_map<std::string, int> matrixColumns;
  std::unordered_map<std::string, std::vector<std::string>> funParamNames;
  std::unordered_map<std::string, std::vector<std::string>> funParamTypes;
  std::unordered_map<std::string, std::string> currentMatrixParamLabels;
  std::unordered_map<std::string, bool> memoriaGlobal;
  int offset = -8;               // Próximo offset disponible en el frame local
  int labelcont = 0;             // Contador de etiquetas únicas para if/while
  bool entornoFuncion = false;   // ¿Estamos dentro de una función?
  std::string currentBreakLabel; // Etiqueta a la que deben saltar los break
  std::string nombreFuncion;     // Nombre de la función actual

  explicit GenCodeVisitor(std::ostream &out) : out(out) {}

  // Punto de entrada de la generación
  int generar(Program *program);

  // ---- Visitas ----
  int visit(BinaryExp *exp) override;
  int visit(NumberExp *exp) override;
  int visit(IdExp *exp) override;
  int visit(UnaryExp *exp) override;
  int visit(IndexExp *exp) override;
  int visit(MatrixExp *exp) override;
  int visit(FieldExp *exp) override;
  int visit(Program *p) override;
  int visit(StructDec *sd) override;
  int visit(PrintStm *stm) override;
  int visit(AssignStm *stm) override;
  int visit(ExpListSize *stm) override;
  int visit(ExpListVals *stm) override;
  int visit(ExpMatrixSize *stm) override;
  int visit(ExpMatrixVals *stm) override;
  int visit(WhileStm *stm) override;
  int visit(DoWhileStm *stm) override;
  int visit(IfStm *stm) override;
  int visit(BreakStm *stm) override;
  int visit(SwitchStm *stm) override;
  int visit(Body *body) override;
  int visit(VarDec *vd) override;
  int visit(FcallExp *fc) override;
  int visit(ReturnStm *r) override;
  int visit(FunDec *fd) override;
};

#endif // VISITOR_H
