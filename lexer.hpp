#pragma once

#include <string>
#include <vector>

enum class LexTokenType {
  lex_undefined,
  lex_eof, // End Of Field

  lex_fn, // Function Definition
  lex_fn_yields,

  lex_identifier, // Variable/Function name
  lex_intlit,     // Int literal

  lex_open_paren,
  lex_close_paren,
  lex_open_curly,
  lex_close_curly,
  lex_colon,

  lex_plus,
  lex_minus,
  lex_star,
  lex_fslash,

  lex_lt,
  lex_gt,
  lex_eq,

  lex_if,
  lex_else,

  lex_semi,
};

struct LexToken {
  LexTokenType type;
  std::optional<std::string> valueString{};
};

inline std::string to_string(const LexTokenType type) {
  switch (type) {
  case LexTokenType::lex_undefined:
    return "lex_undefined";
  case LexTokenType::lex_eof:
    return "lex_eof";
  case LexTokenType::lex_fn:
    return "lex_fn";
  case LexTokenType::lex_fn_yields:
    return "lex_fn_yields";
  case LexTokenType::lex_identifier:
    return "lex_identifier";
  case LexTokenType::lex_intlit:
    return "lex_intlit";
  case LexTokenType::lex_open_paren:
    return "lex_open_paren";
  case LexTokenType::lex_close_paren:
    return "lex_close_paren";
  case LexTokenType::lex_open_curly:
    return "lex_open_curly";
  case LexTokenType::lex_close_curly:
    return "lex_close_curly";
  case LexTokenType::lex_colon:
    return "lex_colon";
  case LexTokenType::lex_plus:
    return "lex_plus";
  case LexTokenType::lex_minus:
    return "lex_minus";
  case LexTokenType::lex_star:
    return "lex_star";
  case LexTokenType::lex_fslash:
    return "lex_fslash";
  case LexTokenType::lex_lt:
    return "lex_lt";
  case LexTokenType::lex_gt:
    return "lex_ft";
  case LexTokenType::lex_eq:
    return "lex_eq";
  case LexTokenType::lex_if:
    return "lex_if";
  case LexTokenType::lex_else:
    return "lex_else";
  case LexTokenType::lex_semi:
    return "lex_semi";
  }
}

inline std::string to_string(const LexToken token) {
  if (!token.valueString.has_value())
    return to_string(token.type);
  return to_string(token.type) + "(" + token.valueString.value() + ")";
}

class Lexer {
public:
  explicit Lexer(std::string source) : src(source) {}

  std::vector<LexToken> lex() {
    std::vector<LexToken> tokens;

    fprintf(stderr, "Lexing...\n");

    std::string buf;

    while (peek() != EOF) {
      if (std::isspace(peek())) {
        consume();
        continue;
      }
      if (std::isalpha(peek())) {
        buf = consume();
        while (std::isalnum(peek()))
          buf += consume();
        tokens.push_back(tokeniseIdentifier(buf));
        continue;
      }
      if (std::isdigit(peek())) {
        buf = consume();
        while (std::isdigit(peek()))
          buf += consume();
        tokens.push_back(tokeniseIntLiteral(buf));
        continue;
      }
      if (peek() == '-' && peek(1) == '>') {
        consume();
        consume();
        tokens.push_back({LexTokenType::lex_fn_yields});
        continue;
      }

      tokens.push_back(tokeniseOperator(consume()));
    }
    tokens.push_back({LexTokenType::lex_eof});
    idx = 0;
    return tokens;
  }

private:
  std::string src;
  size_t idx = 0;

  LexToken tokeniseIdentifier(std::string identifier) {
    if (identifier == "fn")
      return {LexTokenType::lex_fn};
    if (identifier == "if")
      return {LexTokenType::lex_if};
    if (identifier == "else")
      return {LexTokenType::lex_else};

    return {LexTokenType::lex_identifier, identifier};
  }

  LexToken tokeniseIntLiteral(std::string intlit) {
    return {LexTokenType::lex_intlit, intlit};
  }

  LexToken tokeniseOperator(char op) {
    switch (op) {
    case ':':
      return {LexTokenType::lex_colon};
    case '(':
      return {LexTokenType::lex_open_paren};
    case ')':
      return {LexTokenType::lex_close_paren};
    case '{':
      return {LexTokenType::lex_open_curly};
    case '}':
      return {LexTokenType::lex_close_curly};

    case '+':
      return {LexTokenType::lex_plus};
    case '-':
      return {LexTokenType::lex_minus};
    case '*':
      return {LexTokenType::lex_star};
    case '/':
      return {LexTokenType::lex_fslash};

    case '<':
      return {LexTokenType::lex_lt};
    case '>':
      return {LexTokenType::lex_gt};
    case '=':
      return {LexTokenType::lex_eq};

    case ';':
      return {LexTokenType::lex_semi};

    default:
      return {LexTokenType::lex_undefined, std::to_string(op)};
    }
  }

  char peek(const size_t offset = 0) const {
    if (idx + offset >= src.length())
      return EOF;
    return src.at(idx + offset);
  }

  char consume() { return src.at(idx++); }
};
