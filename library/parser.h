#ifndef ULANG_PARSER_H
#define ULANG_PARSER_H

#include "lexer.h"

#include <iostream>
#include <vector>
#include <memory>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>

using namespace llvm;

namespace UraniumLang {

  inline static std::unique_ptr<LLVMContext> Context{};
  inline static std::unique_ptr<Module> TheModule{};
  inline static std::unordered_map<std::string, Value*> NamedValues{}; // name, value

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

  class VarDefStmtAST : public StmtAST {
  public:
    inline VarDefStmtAST(Token type, const std::string &name) : m_Type(type), m_Name(name) {}
    inline void SetExpr(std::unique_ptr<ExprAST> expr) { m_Expr = std::move(expr); }
    virtual Value *Generate() override;
  private:
    Token m_Type{};
    std::string m_Name{};
    std::unique_ptr<ExprAST> m_Expr = nullptr;
  };

  class ProgramAST : public StmtAST {
  public:
    inline ProgramAST(std::vector<std::unique_ptr<StmtAST>> stmts = {}) : m_Stmts(std::move(stmts)) {}
    inline void AddStmt(std::unique_ptr<StmtAST> stmt) { m_Stmts.push_back(std::move(stmt)); }
    virtual Value *Generate() override;
  private:
    std::vector<std::unique_ptr<StmtAST>> m_Stmts;
  };

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
  std::unique_ptr<ExprAST> ParseExpr();
  std::unique_ptr<TermAST> ParseTerm();
  private:
  inline Token peek() { return (m_CurTok = m_Lexer->GetTok()); }
  inline bool consume(Token::Type type) {
    auto t = peek().type;
    return t == type;
  }
  private:
  Token m_CurTok{};
  std::unique_ptr<Lexer> m_Lexer{};
  };

}

#endif
