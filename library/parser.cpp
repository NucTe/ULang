#include "parser.h"

namespace UraniumLang {

  static std::unique_ptr<IRBuilder<>> Builder = std::make_unique<IRBuilder<>>(*Context);

  Value *VarDefStmtAST::Generate() {
    if (m_Expr) { // Define and set
      return m_Expr->Generate();
    }
    return nullptr;
  }

  Value *ProgramAST::Generate() {
    Value *val = nullptr;
    for (auto &stmt : m_Stmts) val = stmt->Generate();
    return val;
  }

  Parser::Parser(std::unique_ptr<Lexer> lex)
    : m_Lexer(std::move(lex)) {}

  Parser::Parser(const std::string &filepath)
    : m_Lexer(std::make_unique<Lexer>(filepath)) {}

  std::unique_ptr<ProgramAST> Parser::Parse() {
    Context = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("UraniumLang JIT", *Context);
    Builder = std::make_unique<IRBuilder<>>(*Context);

    std::unique_ptr<ProgramAST> program 
      = ParseProgram();
    return program;
  }

  // private:

  std::unique_ptr<ProgramAST> Parser::ParseProgram() {
    auto prog = std::make_unique<ProgramAST>();
    while (peek().type != Token::Type::TOKN_EOF) prog->AddStmt(ParseStmt());
    return prog;
  }

  std::unique_ptr<StmtAST> Parser::ParseStmt() {
    if (m_CurTok.type == Token::Type::TOKN_ID) {
      auto tok = m_CurTok;
      if (peek().type == Token::Type::TOKN_ID) {
        auto varDef = std::make_unique<VarDefStmtAST>(tok, m_CurTok.value.value());
        if (consume(Token::Type::TOKN_EQUALS)) { varDef->SetExpr(ParseExpr()); if (!consume(Token::Type::TOKN_SEMI)) { throw std::runtime_error("Expected TOKN_SEMI but instead got " + Token::ToString(m_CurTok.type)); } }
        else if (m_CurTok.type != Token::Type::TOKN_SEMI)
          throw std::runtime_error("Expected TOKN_SEMI or TOKN_EQUALS, instead got " 
            + Token::ToString(m_CurTok.type));
        return varDef;
      }
    }
    return nullptr;
  }

  std::unique_ptr<ExprAST> Parser::ParseExpr() {
    auto lhs = ParseTerm();
    if (!lhs) {return nullptr;}
    return std::move(lhs);
  }

  std::unique_ptr<TermAST> Parser::ParseTerm() {
    Token::Type type = peek().type;
    if (type == Token::Type::TOKN_INT || type == Token::Type::TOKN_ID) {
      return std::make_unique<TermAST>(m_CurTok);
    }
    return nullptr;
  }

}
