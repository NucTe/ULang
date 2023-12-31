#ifndef ULANG_COMPILER_H_
#define ULANG_COMPILER_H_

#include <parser.h> // Include ULang's Parser

#include <stdio.h>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;

static UraniumLang::uptr<LLVMContext> Context{};
static UraniumLang::uptr<Module> TheModule{};
static std::map<std::string, AllocaInst*> NamedValues{};      // { name, value }
static std::map<std::string, GlobalVariable*> GlobalValues{}; // { name, value }
static std::unique_ptr<IRBuilder<>> Builder = std::make_unique<IRBuilder<>>(*Context);

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          StringRef VarName) {
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                    TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getDoubleTy(*Context), nullptr, VarName);
}

namespace UraniumLang {

  struct CompilerOptions {
    bool        _Error = false;
    std::string _ErrorMsg = "";

    std::string Input{}, Output{};
    std::vector<std::string> IncludeDirs{};
    bool ULangBitcode = false;
  };

  CompilerOptions ParseArguments(int argc, char **argv);

  struct CompilationError {
    std::string Description{};

    friend std::ostream &operator <<(std::ostream &os, const CompilationError &err) {
      return os << err.Description;
    }
  };

  class Generator {
  public:
    Generator(uptr<ProgNode> program) : m_Program(std::move(program)) {}
    ~Generator() = default;

    std::vector<CompilationError> Generate(CompilerOptions options);
  private:
    uptr<ProgNode> m_Program;
  };

  class Compiler {
  public:
    Compiler(const CompilerOptions &options);
    ~Compiler() = default;

    bool Compile();
    std::vector<CompilationError> GetErrors() { return m_Errors; }
  private:
    CompilerOptions m_Options{};
    uptr<Parser> m_Parser{};
    std::vector<CompilationError> m_Errors{};
  };

}

#endif