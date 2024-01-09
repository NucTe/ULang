#include "compiler.h"

#include <filesystem>

namespace UraniumLang {

  void Help(CompilerOptions &options, const char *cmd) {
    const std::vector<std::pair<std::string, std::string>> options_ =
    {
      { "--h",       "Display this information." },
      { "--v",       "Display version." },
      { "-o <file>", "Place the output into <file>." },
    };

    std::stringstream msg("");
    auto optFunc = [&](std::string name, std::string description) {
      int nameLen = name.size();
      int spaceLen = nameLen < 25 ? 25 - nameLen : 1; // 20 = space between first character and description
      for (int i = 0; i < 2; ++i) msg << " ";
      msg << name;
      for (int i = 0; i < spaceLen; ++i) msg << " ";
      msg << description << std::endl;
    };

    msg << "Usage: " << cmd << " [options] file..." << std::endl << "Options:" << std::endl;
    for (auto opt : options_) optFunc(opt.first, opt.second);

    options._Error = true;
    options._ErrorMsg = msg.str();
  }

  CompilerOptions ParseArguments(int argc, char** argv) {
    CompilerOptions options{};

    if (argc == 1) {
      Help(options, argv[0]);
    } else {
      for (size_t i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--h") == 0) {
          Help(options, argv[0]);
          break;
        }
        else if (strcmp(argv[i], "-o") == 0) {
          if (i < argc) {
            options.Output = argv[++i];
          } else {
            Help(options, argv[0]);
            break;
          }
        }
        else { // Probably input file
          if (!std::filesystem::exists(argv[i])) {
            options._Error = true;
            options._ErrorMsg = "File " + std::string(argv[i]) + " doesn't exist!\n";
            break;
          }
          options.Input = argv[i];
        }
      }
    }

    return options;
  }

  std::vector<CompilationError> Generator::Generate(CompilerOptions options) {
    std::vector<CompilationError> errors{};

    auto TargetTriple = sys::getDefaultTargetTriple();
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!Target) {
      errs() << Error;
      return errors;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, Reloc::PIC_);

    TheModule->setDataLayout(TargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);

    return errors;
  }

  // Value *VarDefExprAST::Generate() {
  //   AllocaInst *OldBinding;
  //   GlobalVariable *OldGBinding;

  //   Value *InitVal 
  //     = m_Expr == nullptr ? ConstantFP::get(*Context, APFloat(0.0))
  //       : m_Expr->Generate();
  //   if (!InitVal) throw std::runtime_error("Failed!");

  //   bool isLocal;
  //   std::optional<AllocaInst *> Alloca;
  //   std::optional<GlobalVariable *> GlobalVar;
  //   if ((isLocal = Builder->GetInsertBlock())) {
  //     Function *TheFunction = Builder->GetInsertBlock()->getParent();
  //     Alloca = CreateEntryBlockAlloca(TheFunction, m_Name);
  //     Builder->CreateStore(InitVal, Alloca.value());
  //   }
  //   else {
  //     GlobalVar = CreateGlobal();
  //   }

  //   if (!(Alloca.has_value() || GlobalVar.has_value())) throw std::runtime_error("Failed to VarDef!");

  //   if (isLocal) {
  //     OldBinding = NamedValues[m_Name];
  //     NamedValues[m_Name] = Alloca.value();
  //     // NamedValues[m_Name] = OldBinding; - once we implement scopes = pop variables
  //   } 
  //   else {
  //     OldGBinding = GlobalValues[m_Name];
  //     GlobalValues[m_Name] = GlobalVar.value();
  //     // GlobalValues[m_Name] = OldGBinding; - once we implement scopes = pop variables
  //   }

  //   return (isLocal ? (Value*)NamedValues[m_Name] : (Value*)GlobalValues[m_Name]);
  // }

  // GlobalVariable *VarDefExprAST::CreateGlobal() {
  //   GlobalVariable* gVar = (GlobalVariable*)TheModule->getOrInsertGlobal(m_Name.c_str(), Builder->getDoubleTy());
  //   gVar->setLinkage(GlobalValue::PrivateLinkage);
  //   gVar->setAlignment(Align(alignof(double)));

  //   Value *InitVal = !m_Expr ? ConstantFP::get(*Context, APFloat(0.0)) : m_Expr->Generate();
  //   gVar->setInitializer(cast<Constant>(InitVal));

  //   return gVar;
  // }

  // Value *VariableExprAST::Generate() {
  //   if (GlobalValues.find(m_Name) != GlobalValues.end()) {
  //     auto var = GlobalValues[m_Name];
  //     if (!var) {
  //         throw std::runtime_error("Global variable pointer is null: \"" + m_Name + "\"");
  //     }
  //     return Builder->CreateLoad(var->getValueType(), var, m_Name.c_str());
  //   } else {
  //     if (NamedValues.find(m_Name) == NamedValues.end()) {
  //       throw std::runtime_error("Unknown variable name: \"" + m_Name + "\"");
  //     }
  //     AllocaInst *A = NamedValues[m_Name];
  //     if (!A) {
  //       throw std::runtime_error("Local variable pointer is null: \"" + m_Name + "\"");
  //     }
  //     return Builder->CreateLoad(A->getAllocatedType(), A, m_Name.c_str());
  //   }
  // }

  // Value *BinOpExprAST::Generate() {
  //   Value *L = m_LHS->Generate();
  //   Value *R = m_RHS->Generate();
  //   if (!L || !R)
  //     return nullptr;

  //   switch (m_OpCode) {
  //   case Token::Type::TOKN_PLUS:
  //     return Builder->CreateFAdd(L, R, "addtmp");
  //   case Token::Type::TOKN_MINUS:
  //     return Builder->CreateFSub(L, R, "subtmp");
  //   case Token::Type::TOKN_STAR:
  //     return Builder->CreateFMul(L, R, "multmp");
  //   case Token::Type::TOKN_FSLASH:
  //     return Builder->CreateFDiv(L, R, "divtmp");
  //   default: throw std::runtime_error("Invalid binary operator " + Token::ToString(m_OpCode) + "!");
  //   }
  // }

  // Value *ProgramAST::Generate() {
  //   Value *val = nullptr;
  //   for (auto &stmt : m_Stmts) val = stmt->Generate();
  //   TheModule->print(errs(), nullptr);
  //   return val;
  // }

  Compiler::Compiler(const CompilerOptions &options)
    : m_Options(options) {
    m_Parser = std::make_unique<UraniumLang::Parser>(m_Options.Input);
  }

  bool Compiler::Compile() {
    auto progNode = m_Parser->Parse();

    return true;
  }

}