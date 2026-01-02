#pragma once

#include <string>
#include <vector>

#include "lexer.hpp"

class ASTExpr {
public:
  virtual ~ASTExpr() = default;
};

class ASTExprIntLit : public ASTExpr {
public:
  ASTExprIntLit(int val) : value(val) {}

private:
  int value;
};

class ASTExprVariable : public ASTExpr {
public:
  ASTExprVariable(const std::string name) : name(name) {}

private:
  std::string name;
};

enum class BinaryOp {
  op_add,
  op_sub,
  op_mult,
  op_div,
};

class ASTExprBinary : public ASTExpr {
public:
  ASTExprBinary(BinaryOp op, std::unique_ptr<ASTExpr> LHS,
                std::unique_ptr<ASTExpr> RHS)
      : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

private:
  BinaryOp op;
  std::unique_ptr<ASTExpr> LHS, RHS;
};

class ASTExprCall : public ASTExpr {
private:
  std::string callee;
  std::vector<std::unique_ptr<ASTExpr>> args;

public:
  ASTExprCall(const std::string &callee,
              std::vector<std::unique_ptr<ASTExpr>> args)
      : callee(callee), args(std::move(args)) {}
};

class ASTPrototype {
public:
  ASTPrototype(const std::string &name, std::vector<std::string> args)
      : name(name), args(std::move(args)) {}

  const std::string &getName() const { return name; }

private:
  std::string name;
  std::vector<std::string> args;
};

class ASTFunction {
private:
  std::unique_ptr<ASTPrototype> proto;
  std::unique_ptr<ASTExpr> body;

public:
  ASTFunction(std::unique_ptr<ASTPrototype> proto,
              std::unique_ptr<ASTExpr> body)
      : proto(std::move(proto)), body(std::move(body)) {}
};

class Parser {
public:
  explicit Parser(std::vector<LexToken> tokens) : src(tokens) {}

  void parse() {
    fprintf(stderr, "Parsing...");

    while (peek().type != LexTokenType::lex_eof) {

      consume();
    }
  }

private:
  std::vector<LexToken> src;
  size_t idx;

  LexToken peek(const size_t offset = 0) const {
    if (idx + offset >= src.size())
      return {LexTokenType::lex_eof};
    return src.at(idx + offset);
  }

  LexToken consume() { return src.at(idx++); }
};
