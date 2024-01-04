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
    virtual const std::string &kind() = 0;
  private:
  };

  class ExprNode : public StmtNode {
  public:
    virtual ~ExprNode() = default;
    virtual const std::string &kind() = 0;
  private:
  };

  class IdentExpr : public ExprNode {
  public:
    IdentExpr(const std::string &symbol) : m_Symbol(symbol) {}
    virtual const std::string &kind() override { return "IdentExpr"; }
  private:
    std::string m_Symbol{};
  };

  class NumLitExpr : public ExprNode {
  public:
    NumLitExpr(const Token &value) : m_Value(value) {}
    virtual const std::string &kind() override { return "NumLitExpr"; }
  private:
    Token m_Value;
  };

  class BinExpr : public ExprNode {
  public:
    BinExpr(uptr<ExprNode> left, uptr<ExprNode> right, const std::string &op)
      : m_Left(std::move(left)), m_Right(std::move(right)), m_Op(op) {}
    virtual const std::string &kind() override { return "BinExpr"; }
  private:
    uptr<ExprNode> m_Left{}, m_Right{};
    std::string m_Op{};
  };

  class ProgNode : public StmtNode {
  public:
    ProgNode(std::vector<uptr<StmtNode>> stmts)
      : m_Stmts(std::move(stmts)) {}

    virtual const std::string &kind() override { return "ProgNode"; }
    inline const std::vector<uptr<StmtNode>> &GetStatements() { return m_Stmts; }
  private:
    std::vector<uptr<StmtNode>> m_Stmts{};
  };

  // TODO: add opdef (void op()() {})
  inline static std::map<Token::Type, int> BinopPrecedence = {
    { Token::Type::TOKN_PLUS, 10 },   // Add
    { Token::Type::TOKN_MINUS, 10 },  // Sub
    { Token::Type::TOKN_STAR, 20 },   // Mul
    { Token::Type::TOKN_FSLASH, 20 }, // Div
  };

  inline static std::map<std::string, std::string> Types = { // TODO: add typedef
    { "int", "int" },
    { "double", "double" },
    { "char", "char" },
    // { "int", "int" },
  };

  static int GetTokPrecedence(Token::Type type) {
    int TokPrec = BinopPrecedence[type];

    return TokPrec;
  }

  class Parser {
  public:
  
  Parser() = default;
  Parser(uptr<Lexer> lex);
  Parser(const std::string &filepath);
  ~Parser() = default;

  uptr<ProgNode> Parse();
  
  private: // Functions

  void Initialize();
  
  uptr<StmtNode> ParseStmt();
  uptr<ExprNode> ParseExpr();
  uptr<ExprNode> ParsePrimExpr();

  private:

  // Get current token without advancing
  // params:
  //  - off: Offset (default = 0)
  inline Token peek(size_t off = 0) {
    return m_Tokens[m_Index+off];
  }

  // Advance and get previous token
  inline Token advance() {
    Token tokn = m_CurTok;
    if (m_Tokens.size() > (m_Index+1)) m_CurTok = m_Tokens[++m_Index];
    return tokn;
  }
  
  private:
  
  size_t m_Index = 0;
  Token m_CurTok{};
  std::vector<Token> m_Tokens{};
  std::unique_ptr<Lexer> m_Lexer{};
  
  };

}

#endif
