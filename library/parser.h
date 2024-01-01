#ifndef ULANG_PARSER_H
#define ULANG_PARSER_H

#include "utilities.h"

#include "lexer.h"

#include <iostream>
#include <vector>
#include <map>
#include <memory>

namespace UraniumLang {

  class StmtNode {
  public:
    virtual ~StmtNode() = default;
  private:
  };

  class ExprNode {
  public:
    virtual ~ExprNode() = default;
  private:
  };

  class FuncCallNode : public StmtNode {
  public:
    FuncCallNode(const std::string &funcName, std::vector<uptr<ExprNode>> arguments) 
      : m_Name(funcName), m_Arguments(std::move(arguments)) {}
  
    inline const std::string &GetName() const { return m_Name; }
    inline const std::vector<uptr<ExprNode>> &GetArguments() const { return m_Arguments; }
  private:
    std::string m_Name{};
    std::vector<uptr<ExprNode>> m_Arguments{};
  };

  class VarDefNode : public StmtNode {
  public:
    VarDefNode(const std::string &type, const std::string &name, uptr<ExprNode> expr)
      : m_Type(type), m_Name(name), m_Expr(std::move(expr)) {}

    inline const std::string &GetName() const { return m_Name; }
    inline const std::string &GetType() const { return m_Type; }
    inline const ExprNode &GetExpr() const { return *m_Expr.get(); }
  private:
    std::string m_Type{};
    std::string m_Name{};
    uptr<ExprNode> m_Expr{};
  };

  class ScopeNode : public StmtNode {
  public:
    ScopeNode(std::vector<uptr<StmtNode>> stmts)
      : m_Stmts(std::move(stmts)) {}

    inline const std::vector<uptr<StmtNode>> &GetStatements() { return m_Stmts; }
  private:
    std::vector<uptr<StmtNode>> m_Stmts{};
  };

  // this is the same as scope lmfao, guess I could make it child of ScopeNode but nope
  class ProgNode : public StmtNode {
  public:
    ProgNode(std::vector<uptr<StmtNode>> stmts)
      : m_Stmts(std::move(stmts)) {}

    inline const std::vector<uptr<StmtNode>> &GetStatements() { return m_Stmts; }
  private:
    std::vector<uptr<StmtNode>> m_Stmts{};
  };

  inline static std::map<Token::Type, int> BinopPrecedence = {
    { Token::Type::TOKN_PLUS, 10 },   // Add
    { Token::Type::TOKN_MINUS, 10 },  // Sub
    { Token::Type::TOKN_STAR, 20 },   // Mul
    { Token::Type::TOKN_FSLASH, 20 }, // Div
  };

  static int GetTokPrecedence(Token::Type type) {
    int TokPrec = BinopPrecedence[type];

    return TokPrec;
  }

  class Parser {
  public:
  Parser() = default;
  Parser(std::unique_ptr<Lexer> lex);
  Parser(const std::string &filepath);
  ~Parser() = default;

  Result<uptr<ProgNode>> Parse();
  private: // Functions
  Result<uptr<StmtNode>> ParseStmt();
  private: // Variables
  inline Token peek(int off = 0) { return m_Tokens[m_Index + off]; }
  inline Token consume() { return m_Tokens[m_Index++]; }
  inline Result<Token> consume(Token::Type type) {
    if (consume().type == type) return Result<Token>(m_CurTok);
    else return Result<Token>(UraniumLang::Error(Error::ErrCode::FAILED));
  }
  private:
  size_t m_Index = 0;
  Token m_CurTok{};
  std::vector<Token> m_Tokens{};
  std::unique_ptr<Lexer> m_Lexer{};
  };

}

#endif
