#include <cctype>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char **argv) {
  if (argc < 2)
    return EXIT_FAILURE;

  std::string source;
  {
    std::stringstream src_stream;
    std::fstream input(argv[1], std::ios::in);
    src_stream << input.rdbuf();
    source = src_stream.str();
  }
  printf("%s", source.c_str());

  Lexer lexer(source);
  std::vector<LexToken> lexed = lexer.lex();
  for (LexToken token : lexed) {
    fprintf(stderr, "%s\n", to_string(token).c_str());
  }

  Parser parser(lexed);

  parser.parse();

  return EXIT_SUCCESS;
}
