#include <parser.h> // Include ULang's Parser

#include <stdio.h>

using namespace UraniumLang;

int printHelp(char *path) {
  std::cout << "Usage: `" << std::string(path) << " <path>`" << std::endl;
  return EXIT_FAILURE;
}

int handleException(const char *what) {
  fprintf(stderr, "%s\n", what);
  delete []what;
  return 1;
}

int main(int argc, char** argv) {
  if (argc < 2) return printHelp(argv[0]);
  std::unique_ptr<Parser> parser = std::make_unique<Parser>("test.ulang");
  std::unique_ptr<ProgramAST> programAST = nullptr;
  try {
    programAST = parser->Parse();
  } catch (ParserUnexpectedTokEx ex) {
    return handleException(ex.what());
  } catch (ParserException ex) {
    return handleException(ex.what());
  }
  programAST->Generate();
  return 0;
}