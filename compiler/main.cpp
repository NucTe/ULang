#include "compiler.h"

int printError(UraniumLang::CompilerOptions options) {
  std::cout << options._ErrorMsg;

  return 1;
}

int printCompilationErrors(UraniumLang::Compiler *compiler) {
  auto errors = compiler->GetErrors();
  for (auto error : errors) std::cerr << error << std::endl;
  return 1;
}

int main(int argc, char** argv) {
  UraniumLang::CompilerOptions options = UraniumLang::ParseArguments(argc, argv);
  if (options._Error) return printError(options);
  UraniumLang::uptr<UraniumLang::Compiler> compiler = std::make_unique<UraniumLang::Compiler>(options);
  if (!compiler->Compile()) return printCompilationErrors(compiler.get());
  return 0;
}