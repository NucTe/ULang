#ifndef ULANG_PARSER_H
#define ULANG_PARSER_H

#include "lexer.h"

#include <iostream>
#include <vector>
#include <map>
#include <memory>

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

namespace UraniumLang {

  class ParserException : public std::exception {
  public:
    inline ParserException(std::string name, std::string description) : m_Name(name), m_Description(description) {}
    virtual ~ParserException() = default;
    inline const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
      const char *strTemplate = "[Parser] (%s): %s";
      char *buf = new char[1024];
      sprintf(buf, strTemplate, m_Name.c_str(), m_Description.c_str());
      return buf;
    }
  private:
    std::string m_Name, m_Description;
  };

  class ParserUnexpectedTokEx : public ParserException {
  public:
    inline ParserUnexpectedTokEx(Token::Type expected, Token got)
      : ParserException("Unexpected Token Exception", "Expected token " + Token::ToString(expected) + ", but got " + Token::ToString(got.type) + " at " + std::to_string(got.line) + ":" + std::to_string(got.col)) {}
  };

  inline static std::unique_ptr<LLVMContext> Context{};
  inline static std::unique_ptr<Module> TheModule{};
  inline static std::map<std::string, AllocaInst*> NamedValues{}; // name, value
  inline static std::map<std::string, GlobalVariable*> GlobalValues{}; // name, value

  static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                            StringRef VarName) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                      TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(Type::getDoubleTy(*Context), nullptr, VarName);
  }

  class StmtAST {
  public:
    virtual ~StmtAST() = default;
    virtual Value *Generate() = 0;
  };

  class ExprAST : public StmtAST {
  public:
    virtual ~ExprAST() = default;
    virtual Value *Generate() = 0;
  };

  class TermAST : public ExprAST {
  public:
    TermAST(Token tok) : m_Token(tok) {}
    inline Token &Get() { return m_Token; }
    inline virtual Value *Generate() override { 
      Value *result = nullptr;
      if (m_Token.type == Token::Type::TOKN_INT) {
        if (!m_Token.value.has_value()) return result;
        auto val = atof(m_Token.value.value().c_str());
        result = ConstantFP::get(*Context, APFloat(val));
      }
      return result; 
    }
  private:
    Token m_Token{};
  };

  class VarDefExprAST : public ExprAST {
  public:
    inline VarDefExprAST(Token type, const std::string &name) : m_Type(type), m_Name(name) {}
    inline void SetExpr(std::unique_ptr<ExprAST> expr) { m_Expr = std::move(expr); }
    virtual Value *Generate() override;
  private:
    GlobalVariable *CreateGlobal();
  private:
    Token m_Type{};
    std::string m_Name{};
    std::unique_ptr<ExprAST> m_Expr = nullptr;
  };

  class VariableExprAST : public ExprAST {
  public:
    inline VariableExprAST(const std::string &name) : m_Name(name) {}
    virtual Value *Generate() override;
  private:
    std::string m_Name{};
  };

  class BinOpExprAST : public ExprAST {
  public:
    BinOpExprAST(std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs, Token::Type opcode)
      : m_LHS(std::move(lhs)), m_RHS(std::move(rhs)), m_OpCode(opcode) {}
    virtual Value *Generate() override;
  private:
    std::unique_ptr<ExprAST> m_LHS{}, m_RHS{};
    Token::Type m_OpCode = Token::Type::TOKN_EOF;
  };

  class ProgramAST : public StmtAST {
  public:
    inline ProgramAST(std::vector<std::unique_ptr<StmtAST>> stmts = {}) : m_Stmts(std::move(stmts)) {}
    inline void AddStmt(std::unique_ptr<StmtAST> stmt) { m_Stmts.push_back(std::move(stmt)); }
    virtual Value *Generate() override;
  private:
    std::vector<std::unique_ptr<StmtAST>> m_Stmts;
  };

  inline static std::map<Token::Type, int> BinopPrecedence = {
    { Token::Type::TOKN_PLUS, 10 },   // Add
    { Token::Type::TOKN_MINUS, 10 },  // Sub
    { Token::Type::TOKN_STAR, 20 },   // Mul
    { Token::Type::TOKN_FSLASH, 20 }, // Div
  };

  static int GetTokPrecedence(Token::Type type) {
    int typeI = (int)type;
    if (typeI < 15 && typeI > 18) return -1; // 15 = TOKN_PLUS, 18 = TOKN_FSLASH (/)
    int TokPrec = BinopPrecedence[type];
    if (TokPrec <= 0) return -1;
    return TokPrec;
  }

  class Parser {
  public:
  Parser() = default;
  Parser(std::unique_ptr<Lexer> lex);
  Parser(const std::string &filepath);
  ~Parser() = default;

  std::unique_ptr<ProgramAST> Parse();
  private:
  std::unique_ptr<ProgramAST> ParseProgram();
  std::unique_ptr<StmtAST> ParseStmt();
  std::unique_ptr<ExprAST> ParseBinOpRHS(int prec, std::unique_ptr<ExprAST> LHS);
  std::unique_ptr<ExprAST> ParseExpr();
  std::unique_ptr<TermAST> ParseTerm();
  private:
  inline Token peek() { return (m_CurTok = m_Lexer->GetTok()); }
  inline bool consume(Token::Type type) {
    return peek().type == type;
  }
  inline void expectToken(Token::Type type) {
    if (peek().type != type) throw ParserUnexpectedTokEx(type, m_CurTok); 
  }
  private:
  Token m_CurTok{};
  std::unique_ptr<Lexer> m_Lexer{};
  };

  // std::vector<AllocaInst *> OldBindings;

  // Function *TheFunction = Builder->GetInsertBlock()->getParent();

  // // Register all variables and emit their initializer.
  // for (unsigned i = 0, e = VarNames.size(); i != e; ++i) {
  //   const std::string &VarName = VarNames[i].first;
  //   ExprAST *Init = VarNames[i].second.get();

  //   // Emit the initializer before adding the variable to scope, this prevents
  //   // the initializer from referencing the variable itself, and permits stuff
  //   // like this:
  //   //  var a = 1 in
  //   //    var a = a in ...   # refers to outer 'a'.
  //   Value *InitVal;
  //   if (Init) {
  //     InitVal = Init->codegen();
  //     if (!InitVal)
  //       return nullptr;
  //   } else { // If not specified, use 0.0.
  //     InitVal = ConstantFP::get(*TheContext, APFloat(0.0));
  //   }

  //   AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, VarName);
  //   Builder->CreateStore(InitVal, Alloca);

  //   // Remember the old variable binding so that we can restore the binding when
  //   // we unrecurse.
  //   OldBindings.push_back(NamedValues[VarName]);

  //   // Remember this binding.
  //   NamedValues[VarName] = Alloca;
  // }

  // // Codegen the body, now that all vars are in scope.
  // Value *BodyVal = Body->codegen();
  // if (!BodyVal)
  //   return nullptr;

  // // Pop all our variables from scope.
  // for (unsigned i = 0, e = VarNames.size(); i != e; ++i)
  //   NamedValues[VarNames[i].first] = OldBindings[i];

  // // Return the body computation.
  // return BodyVal;

}

#endif
