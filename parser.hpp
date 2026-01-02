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
  op_undefined,
  op_add,
  op_sub,
  op_mult,
  op_div,
};

inline BinaryOp getBinaryOp(LexTokenType type) {
  switch (type) {
  case LexTokenType::lex_plus:
    return BinaryOp::op_add;
  case LexTokenType::lex_minus:
    return BinaryOp::op_sub;
  case LexTokenType::lex_star:
    return BinaryOp::op_mult;
  case LexTokenType::lex_fslash:
    return BinaryOp::op_div;

  default:
    return BinaryOp::op_undefined;
  }
}

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
    fprintf(stderr, "Parsing...\n");

    while (peek().type != LexTokenType::lex_eof) {
      if (peek().type == LexTokenType::lex_semi) {
        consume();
        continue;
      }
      parseTopLevel();
    }
  }

private:
  std::unique_ptr<ASTExpr> parseIntLit() {
    fprintf(stderr, "Parsing intlit: %s\n", peek().valueString.value().c_str());
    auto res = std::make_unique<ASTExprIntLit>(
        std::stoi(consume().valueString.value()));
    return std::move(res);
  }

  std::unique_ptr<ASTExpr> parseParen() {
    printf("Parsing Paren Expression...\n");
    consume();
    auto res = parseExpression();
    if (!res)
      return nullptr;

    if (peek().type != LexTokenType::lex_close_paren)
      return nullptr;
    consume();
    fprintf(stderr, "Finished Paren Expression.\n");
    return res;
  }

  std::unique_ptr<ASTExpr> parseIdentifier() {
    std::string name = consume().valueString.value();

    fprintf(stderr, "Parsing Identifier (%s)...\n", name.c_str());

    if (peek().type != LexTokenType::lex_open_paren)
      return std::make_unique<ASTExprVariable>(name);

    consume();
    std::vector<std::unique_ptr<ASTExpr>> args;
    while (peek().type != LexTokenType::lex_close_paren) {
      if (auto arg = parseExpression())
        args.push_back(std::move(arg));
      else
        return nullptr;

      if (peek().type == LexTokenType::lex_close_paren)
        break;

      if (peek().type != LexTokenType::lex_comma)
        return nullptr;

      consume();
    }
    consume();

    return std::make_unique<ASTExprCall>(name, std::move(args));
  }

  std::unique_ptr<ASTExpr> parsePrimary() {
    fprintf(stderr, "Parsing Primary Expression (%s)...\n",
            to_string(peek().type).c_str());
    switch (peek().type) {
    case LexTokenType::lex_identifier:
      return parseIdentifier();
    case LexTokenType::lex_intlit:
      return parseIntLit();
    case LexTokenType::lex_open_paren:
      return parseParen();
    default:
      return nullptr;
    }
  }

  std::unique_ptr<ASTExpr> parseBinOpRHS(BinaryOp op,
                                         std::unique_ptr<ASTExpr> LHS) {
    fprintf(stderr, "Parsing Binary Expression RHS...\n");
    while (true) {
      if (getBinaryOp(peek().type) <= op) {
        fprintf(stderr, "Finished Binary Expression RHS.\n");
        return LHS;
      }
      BinaryOp binOp = getBinaryOp(consume().type);

      auto RHS = parsePrimary();
      if (!RHS)
        break;

      if (binOp <= getBinaryOp(peek().type)) {
        RHS = parseBinOpRHS(binOp, std::move(RHS));
        if (!RHS)
          break;
      }
      LHS = std::make_unique<ASTExprBinary>(binOp, std::move(LHS),
                                            std::move(RHS));
    }
    fprintf(stderr, "Finished Binary Expression RHS (nullptr).\n");
    return nullptr;
  }

  std::unique_ptr<ASTExpr> parseExpression() {
    fprintf(stderr, "Parsing Expression...\n");
    auto LHS = parsePrimary();
    if (!LHS)
      return nullptr;

    return parseBinOpRHS(BinaryOp::op_undefined, std::move(LHS));
  }

  std::unique_ptr<ASTFunction> parseTopLevel() {
    printf("\nParsing Top Level Expression...\n");
    if (auto e = parseExpression()) {
      auto proto =
          std::make_unique<ASTPrototype>("", std::vector<std::string>());
      printf("Finished Top Level Expression.\n");
      return std::make_unique<ASTFunction>(std::move(proto), std::move(e));
    }
    printf("Finished Top Level Expression.\n");
    return nullptr;
  }

  std::vector<LexToken> src;
  size_t idx = 0;

  LexToken peek(const size_t offset = 0) const {
    if (idx + offset >= src.size())
      return {LexTokenType::lex_eof};
    return src.at(idx + offset);
  }

  LexToken consume() { return src.at(idx++); }
};
