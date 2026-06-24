// =============================================================================
// parser.cpp — Implementación del Parser (analizador sintáctico descendente)
// =============================================================================

#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "token.h"
#include <iostream>
#include <stdexcept>
#include <string>

// =============================================================================
// Constructor
// =============================================================================

Parser::Parser(Scanner *sc) : scanner(sc), previous(nullptr) {
  current = scanner->nextToken();
  if (current->type == Token::ERR) {
    throw std::runtime_error("Error léxico: carácter no reconocido '" +
                             current->text + "'");
  }
}

// =============================================================================
// Primitivas de consumo de tokens
// =============================================================================

bool Parser::isAtEnd() { return current->type == Token::END; }

bool Parser::check(Token::Type ttype) {
  if (isAtEnd())
    return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token *temp = current;
    if (previous)
      delete previous;
    current = scanner->nextToken();
    previous = temp;
    if (current->type == Token::ERR) {
      throw std::runtime_error("Error léxico: carácter no reconocido '" +
                               current->text + "'");
    }
    return true;
  }
  return false;
}

bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

// =============================================================================
// Reporte de errores
// =============================================================================

// error — construye un mensaje indicando qué se esperaba vs. qué se encontró.
void Parser::error(const std::string &expected) {
  std::string found;
  if (isAtEnd()) {
    found = "fin de entrada";
  } else {
    found = Token::typeName(current->type);
    if (!current->text.empty())
      found += " '" + current->text + "'";
  }
  throw std::runtime_error("Error sintáctico: se esperaba " + expected +
                           ", pero se encontró " + found);
}

// expect — consume el token si coincide; si no, lanza error descriptivo.
void Parser::expect(Token::Type ttype) {
  if (!match(ttype))
    error(Token::typeName(ttype));
}

// =============================================================================
// Reglas gramaticales
// =============================================================================

// -----------------------------------------------------------------------------
// Program → VarDec* FunDec*
// -----------------------------------------------------------------------------
Program *Parser::parseProgram() {
  Program *p = new Program();

  // Declaraciones de estructuras en la cabecera
  while (check(Token::STRUCT)) {
    p->sdlist.push_back(parseStructDec());
    match(Token::SEMICOL);
  }

  // Declaraciones globales de variables
  while (check(Token::VAR)) {
    p->vdlist.push_back(parseVarDec());
    // Las VarDec globales se separan con ';'
    if (check(Token::FUN) || isAtEnd())
      break;
    expect(Token::SEMICOL);
  }

  // Declaraciones de funciones
  while (check(Token::FUN)) {
    p->fdlist.push_back(parseFunDec());
  }

  // Al terminar debe haberse consumido toda la entrada
  if (!isAtEnd()) {
    error("declaracion de estructura ('struct'), variable ('var'), funcion ('fun') o fin de entrada");
  }

  std::cout << "Parser exitoso" << std::endl;
  return p;
}

StructDec *Parser::parseStructDec() {
  StructDec *sd = new StructDec();

  expect(Token::STRUCT);

  if (!match(Token::ID))
    error("nombre de estructura despues de 'struct'");
  sd->name = previous->text;

  expect(Token::LBRACE);

  while (!check(Token::RBRACE)) {
    if (!check(Token::VAR))
      error("declaracion de campo ('var') o '}' en la estructura '" + sd->name + "'");

    sd->fields.push_back(parseVarDec());

    if (check(Token::RBRACE))
      break;
    expect(Token::SEMICOL);
  }

  expect(Token::RBRACE);
  return sd;
}

// -----------------------------------------------------------------------------
// VarDec → 'var' ID ID (',' ID)*
//   Primer ID = tipo, siguientes IDs = nombres de variables
//   o lista
// VarDec → 'var' ID [CE]
//   Primer ID = tipo, CE es la cantidad de elementos en la lista
// -----------------------------------------------------------------------------
VarDec *Parser::parseVarDec() {
  expect(Token::VAR);

  // Tipo
  if (match(Token::STRUCT)) {
    if (!match(Token::ID))
      error("nombre de estructura despues de 'struct'");
  } else if (!match(Token::ID)) {
    error("nombre de tipo despues de 'var'");
  }
  std::string type = previous->text;

  VarDec *vd = new VarDec();
  vd->type = type;

  if (!match(Token::ID))
    error("nombre de variable después del tipo '" + vd->type + "'");
  std::string varName = previous->text;
  vd->vars.push_back(varName);

  // Más variables separadas por coma
  while (match(Token::COMA)) {
    if (!match(Token::ID))
      error("nombre de variable después de ','");
    varName = previous->text;
    vd->vars.push_back(varName);
  }
  return vd;
}

// -----------------------------------------------------------------------------
// FunDec → 'fun' ID ID '(' Params ')' Body 'endfun'
//   Primer ID = tipo de retorno, segundo ID = nombre de función
// -----------------------------------------------------------------------------
FunDec *Parser::parseFunDec() {
  FunDec *fd = new FunDec();

  expect(Token::FUN);

  // Tipo de retorno
  if (!match(Token::ID))
    error("tipo de retorno después de 'fun'");
  fd->tipo = previous->text;

  // Nombre de la función
  if (!match(Token::ID))
    error("nombre de función después del tipo '" + fd->tipo + "'");
  fd->nombre = previous->text;

  // Parámetros formales
  expect(Token::LPAREN);
  while (check(Token::ID)) {
    // Tipo del parámetro
    match(Token::ID);
    fd->Ptipos.push_back(previous->text);

    // Nombre del parámetro
    if (!match(Token::ID))
      error("nombre de parámetro después del tipo '" + previous->text + "'");
    fd->Pnombres.push_back(previous->text);

    // Coma opcional entre parámetros
    if (check(Token::RPAREN))
      break;
    if (!match(Token::COMA))
      error("',' o ')' en la lista de parámetros de '" + fd->nombre + "'");
  }
  expect(Token::RPAREN);

  // Cuerpo
  fd->cuerpo = parseBody();

  // Aceptar 'endfun' o fin de archivo (tolerancia a endfun faltante)
  if (!match(Token::ENDFUN) && !isAtEnd()) {
    error("'endfun' para cerrar la función '" + fd->nombre + "'");
  }

  return fd;
}

// -----------------------------------------------------------------------------
// Body → VarDec* Stm (';' Stm)*
// -----------------------------------------------------------------------------
Body *Parser::parseBody() {
  Body *b = new Body();

  // Declaraciones locales (separadas por ';'; el ';' final es opcional)
  while (check(Token::VAR)) {
    b->declarations.push_back(parseVarDec());
    if (!match(Token::SEMICOL))
      break;
  }

  // Función auxiliar local: ¿el token actual puede iniciar un statement?
  // Un body termina cuando llega endif/endwhile/endfun/else/END.
  auto isBodyTerminator = [&]() {
    return check(Token::ENDIF) || check(Token::ENDWHILE) ||
           check(Token::ENDFUN) || check(Token::ELSE) ||
           check(Token::ENDSWITCH) || check(Token::ENDDO) || isAtEnd();
  };

  auto isStmStart = [&]() {
    return check(Token::ID) || check(Token::PRINT) || check(Token::RETURN) ||
           check(Token::IF) || check(Token::WHILE) || check(Token::BREAK) ||
           check(Token::SWITCH) || check(Token::DO);
  };

  // Al menos un statement
  int counter = 0;
  if (!isBodyTerminator()) {
    counter++;
  }
  b->StmList.push_back(parseStm());

  // Statements adicionales:
  // Se separan por ';', pero el ';' es opcional después de un bloque
  // estructurado (if/while) porque estos tienen su propio terminador.
  // Algoritmo: consumir ';' si existe; luego, si hay más statements, parsear.
  while (true) {
    // Consumir ';' opcional
    match(Token::SEMICOL);

    // Fin del body
    if (isBodyTerminator())
      break;

    // Si hay algo que parsear, parsearlo
    if (isStmStart()) {
      b->StmList.push_back(parseStm());
    } else {
      break;
    }
  }

  return b;
}

// -----------------------------------------------------------------------------
// Stm → AssignStm | PrintStm | ReturnStm | IfStm | WhileStm
// -----------------------------------------------------------------------------
Stm *Parser::parseStm() {
  // ---- Asignación: ID '=' Exp or ID[CExp] '=' EXP ----
  if (match(Token::ID)) {
    std::string variable = previous->text;

    // Check if ID[CE] or ID.ID
    Exp *var = nullptr;
    if (match(Token::LBRACKET)) {
      Exp *idx = parseCE();
      expect(Token::RBRACKET);
      if (match(Token::LBRACKET)) {
        Exp *col = parseCE();
        expect(Token::RBRACKET);
        var = new MatrixExp(variable, idx, col);
      } else {
        var = new IndexExp(variable, idx);
      }
    } else if (match(Token::DOT)) {
      if (!match(Token::ID))
        error("nombre de campo despues de '.'");
      var = new FieldExp(variable, previous->text);
    } else {
      var = new IdExp(variable);
    }
    // ID = CE
    if (match(Token::ASSIGN)) {
      Exp *rhs = parseCE();
      return new AssignStm(var, rhs);
    } else {
      error("Operacion de asignacion no aceptada. Se espero =");
    }
  }

  // ---- Print: 'print' '(' Exp ')' ----
  if (match(Token::PRINT)) {
    expect(Token::LPAREN);
    Exp *e = parseCE();
    expect(Token::RPAREN);
    return new PrintStm(e);
  }

  // ---- Return: 'return' '(' Exp ')' ----
  if (match(Token::RETURN)) {
    ReturnStm *r = new ReturnStm();
    expect(Token::LPAREN);
    r->e = parseCE();
    expect(Token::RPAREN);
    return r;
  }

  // ---- If: 'if' CE 'then' Body ('else' Body)? 'endif' ----
  if (match(Token::IF)) {
    Exp *cond = parseCE();
    Body *tb = nullptr;
    Body *fb = nullptr;

    if (!match(Token::THEN))
      error("'then' después de la condición del 'if'");
    tb = parseBody();

    if (match(Token::ELSE))
      fb = parseBody();

    if (!match(Token::ENDIF))
      error("'endif' para cerrar el bloque 'if'");

    return new IfStm(cond, tb, fb);
  }

  // ---- While: 'while' CE 'do' Body 'endwhile' ----
  if (match(Token::WHILE)) {
    Exp *cond = parseCE();

    if (!match(Token::DO))
      error("'do' después de la condición del 'while'");

    Body *b = parseBody();

    if (!match(Token::ENDWHILE))
      error("'endwhile' para cerrar el bloque 'while'");

    return new WhileStm(cond, b);
  }

  // ---- Do-While: 'do' Body 'while' CE 'endwhile' ----
  if (match(Token::DO)) {
    Body *b = parseBody();

    if (!match(Token::DOWHILE))
      error("'dowhile' después del cuerpo de do-while");

    Exp *cond = parseCE();

    if (!match(Token::ENDDO))
      error("'enddo' para cerrar el do-while");

    return new DoWhileStm(b, cond);
  }

  // ---- Break: 'break' ----
  if (match(Token::BREAK)) {
    return new BreakStm();
  }

  // ---- Switch: 'switch' Exp 'case' NUM Body ... 'default' Body? 'endswitch'
  // ----
  if (match(Token::SWITCH)) {

    Exp *expr = parseCE();

    SwitchStm *sw = new SwitchStm(expr);
    while (match(Token::CASE)) {
      if (!match(Token::NUM))
        error("número después de case");
      int value = std::stoi(previous->text);
      CaseStm *c = new CaseStm(value);
      auto isCaseEnd = [&]() {
        return check(Token::CASE) || check(Token::DEFAULT) ||
               check(Token::ENDSWITCH);
      };
      while (!isCaseEnd()) {
        c->body.push_back(parseStm());
        if (check(Token::SEMICOL))
          advance();
      }
      sw->cases.push_back(c);
    }
    if (match(Token::DEFAULT)) {
      while (!check(Token::ENDSWITCH)) {
        sw->default_body.push_back(parseStm());
        if (check(Token::SEMICOL))
          advance();
      }
    }
    expect(Token::ENDSWITCH);
    return sw;
  }
  // ---- Sin coincidencia ----
  error("inicio de sentencia: identificador, 'print', 'return', 'if', 'while', "
        "'do', 'break' o 'switch'");
  return nullptr; // inalcanzable; silencia el warning del compilador
}

// =============================================================================
// Reglas de expresiones (precedencia ascendente)
// =============================================================================

// LogicalOr → LogicalAnd ('||' LogicalAnd)*
Exp *Parser::parseCE() {
  Exp *l = parseLogicalAnd();
  while (match(Token::OR)) {
    Exp *r = parseLogicalAnd();
    l = new BinaryExp(l, r, OR_OP);
  }
  return l;
}

// LogicalAnd → RelExp ('&&' RelExp)*
Exp *Parser::parseLogicalAnd() {
  Exp *l = parseRelExp();
  while (match(Token::AND)) {
    Exp *r = parseRelExp();
    l = new BinaryExp(l, r, AND_OP);
  }
  return l;
}

// RelExp → BE (('<'|'>'|'<='|'>='|'=='|'!=') BE)*
Exp *Parser::parseRelExp() {
  Exp *l = parseBE();
  while (true) {
    if (match(Token::LE)) {
      Exp *r = parseBE();
      l = new BinaryExp(l, r, LE_OP);
    } else if (match(Token::GT)) {
      Exp *r = parseBE();
      l = new BinaryExp(l, r, GT_OP);
    } else if (match(Token::LEQ)) {
      Exp *r = parseBE();
      l = new BinaryExp(l, r, LEQ_OP);
    } else if (match(Token::GEQ)) {
      Exp *r = parseBE();
      l = new BinaryExp(l, r, GEQ_OP);
    } else if (match(Token::EQ)) {
      Exp *r = parseBE();
      l = new BinaryExp(l, r, EQ_OP);
    } else if (match(Token::NE)) {
      Exp *r = parseBE();
      l = new BinaryExp(l, r, NE_OP);
    } else {
      break;
    }
  }
  return l;
}

// BE → E (('+' | '-') E)*
Exp *Parser::parseBE() {
  Exp *l = parseE();
  while (match(Token::PLUS) || match(Token::MINUS)) {
    BinaryOp op = (previous->type == Token::PLUS) ? PLUS_OP : MINUS_OP;
    Exp *r = parseE();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

// E → T (('*' | '/') T)*
Exp *Parser::parseE() {
  Exp *l = parseT();
  while (match(Token::MUL) || match(Token::DIV)) {
    BinaryOp op = (previous->type == Token::MUL) ? MUL_OP : DIV_OP;
    Exp *r = parseT();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

// T → F ('**' F)?
Exp *Parser::parseT() {
  Exp *l = parseF();
  if (match(Token::POW)) {
    Exp *r = parseF();
    l = new BinaryExp(l, r, POW_OP);
  }
  return l;
}

// F → NUM | 'true' | 'false' | '!' F | '(' CE ')' | ID ('(' Args ')')?
Exp *Parser::parseF() {
  // Operador unario NOT
  if (match(Token::NOT)) {
    Exp *operand = parseF();
    return new UnaryExp(operand);
  }

  // Número literal
  if (match(Token::NUM))
    return new NumberExp(std::stoi(previous->text));

  // Booleanos como enteros
  if (match(Token::TRUE))
    return new NumberExp(1);
  if (match(Token::FALSE))
    return new NumberExp(0);

  // Expresión entre paréntesis
  if (match(Token::LPAREN)) {
    Exp *e = parseCE();
    expect(Token::RPAREN);
    return e;
  }

  if (match(Token::NEW)) {
    match(Token::ID);
    std::string type = previous->text;
    // a.1) id = new ID[CE]
    if (match(Token::LBRACKET)) {
      Exp *first = parseCE();
      expect(Token::RBRACKET);
      if (match(Token::LBRACKET)) {
        Exp *second = parseCE();
        expect(Token::RBRACKET);
        if (match(Token::LBRACE)) {
          ExpMatrixVals *e = new ExpMatrixVals(type, first, second);
          if (!check(Token::RBRACE)) {
            e->values.push_back(parseCE());
            while (match(Token::COMA))
              e->values.push_back(parseCE());
          }
          expect(Token::RBRACE);
          return e;
        }
        return new ExpMatrixSize(type, first, second);
      }
      return new ExpListSize(type, first);
    }
    // a.2) id = new ID{CE (, CE)*}
    else if (match(Token::LBRACE)) {
      ExpListVals *e = new ExpListVals(type);
      if (!check(Token::RBRACE)) {
        e->values.push_back(parseCE());
        while (match(Token::COMA))
          e->values.push_back(parseCE());
      }

      expect(Token::RBRACE);
      return e;
    }
  }

  // Identificador o llamada a función
  if (match(Token::ID)) {
    std::string nom = previous->text;
    if (check(Token::LPAREN)) {
      // Llamada a función: ID '(' (CE (',' CE)*)? ')'
      advance(); // consume '('
      FcallExp *fcall = new FcallExp();
      fcall->nombre = nom;
      if (!check(Token::RPAREN)) {
        fcall->argumentos.push_back(parseCE());
        while (match(Token::COMA))
          fcall->argumentos.push_back(parseCE());
      }
      expect(Token::RPAREN);
      return fcall;
    }
    // ID[CE]
    else if (match(Token::LBRACKET)) {
      Exp *t = parseCE();
      expect(Token::RBRACKET);
      if (match(Token::LBRACKET)) {
        Exp *col = parseCE();
        expect(Token::RBRACKET);
        return new MatrixExp(nom, t, col);
      }
      return new IndexExp(nom, t);
    }
    else if (match(Token::DOT)) {
      if (!match(Token::ID))
        error("nombre de campo despues de '.'");
      return new FieldExp(nom, previous->text);
    }
    return new IdExp(nom);
  }

  error("expresión: número, identificador, 'true', 'false' o '('");
  return nullptr; // inalcanzable
}
