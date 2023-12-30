#include <parser.h> // Include ULang's Parser

#include <stdio.h>

using namespace UraniumLang;

int printHelp(char *path) {
  std::cout << "Usage: `" << std::string(path) << " <path>`" << std::endl;
  return EXIT_FAILURE;
}

int main(int argc, char** argv) {
  if (argc < 2) return printHelp(argv[0]);
  std::unique_ptr<Parser> parser = std::make_unique<Parser>("test.ulang");
  auto program = parser->Parse();
  if (argc > 2) program->Generate();
  return 0;
}
