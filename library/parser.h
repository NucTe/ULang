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

  class TermNode : public ExprNode {
  public:
    TermNode(Token tokn) : m_Tokn(tokn) {}
  private:
    Token m_Tokn{};
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

  class TypeNode {
  public:
    TypeNode() = default;
    ~TypeNode() = default;
    TypeNode(std::vector<Token> tokens) : m_Tokens(std::move(tokens)) {}
  private:
    std::vector<Token> m_Tokens{}; // Allow types to be (for example long long, unsigned long, etc...)
  };

  class VarDefNode : public StmtNode {
  public:
    VarDefNode(uptr<TypeNode> type, const std::string &name)
      : m_Type(std::move(type)), m_Name(name), m_Expr(nullptr) {}

    inline void SetExpr(uptr<ExprNode> expr) { m_Expr = std::move(expr); }

    inline const uptr<TypeNode>& GetType() const { return m_Type; }
    inline const std::string& GetName() const { return m_Name; }
    inline const uptr<ExprNode>& GetExpr() const { return m_Expr; }

  private:
    uptr<TypeNode> m_Type{};
    std::string m_Name{};
    uptr<ExprNode> m_Expr;
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

  Result<uptr<ProgNode>> Parse();
  
  private: // Functions
  
  Result<uptr<TypeNode>> ParseType();
  Result<uptr<ExprNode>> ParseExpr();
  Result<uptr<StmtNode>> ParseStmt();
  
  private: // Variables
  
  inline Token peek(int off = 0) { return m_Tokens[m_Index + off]; }
  
  inline Token consume() { 
    if (m_Index >= m_Tokens.size()) 
      exit((std::cerr << "Failed to parse! Tried to reach token out of bounds." << std::endl, 1));
    return m_CurTok = m_Tokens[++m_Index]; 
  }
  
  inline Result<Token> consume(Token::Type type) {
    if (m_CurTok.type == type) return Result<Token>(consume());
    return 
      Result<Token>(UraniumLang::Error(Error::ErrCode::FAILED));
  }

 inline bool tryConsume(const std::vector<Token::Type> &types) {
    (void)consume();
    for (auto t : types)
      if (m_CurTok.type == t) return true;
    return false;
  }
  
  private:
  
  size_t m_Index = 0;
  Token m_CurTok{};
  std::vector<Token> m_Tokens{};
  std::unique_ptr<Lexer> m_Lexer{};
  
  };

}

#endif
