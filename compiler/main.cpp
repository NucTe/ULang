#include <parser.h> // Include ULang's Parser

#include <stdio.h>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils.h>
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

// from parser.cpp

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

int printHelp(char *path) {
  std::cout << "Usage: `" << std::string(path) << " <path>`" << std::endl;
  return 1;
}

int main(int argc, char** argv) {
  if (argc < 2) return printHelp(argv[0]);
  std::unique_ptr<UraniumLang::Parser> parser = std::make_unique<UraniumLang::Parser>(argv[1]);
  
  {
    auto prog = parser->Parse();
    std::cout << "Parsed successfully!" << std::endl;
    // TODO: Compile based on options
  }
  
  return 0;
}