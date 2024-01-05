#ifndef ULANG_PARSER_H
#define ULANG_PARSER_H

#include "utilities.h"

#include "lexer.h"

#include <iostream>
#include <vector>
#include <map>
#include <memory>

// =============== [ AST Nodes ] ===============
//  Statements:
//   - Variable Declaration Statement
//  Expressions:
//   - Identifier Expression
//   - Number Literal Expression
//   - Binary Expression
// =============== [ AST Nodes ] ===============

namespace UraniumLang {

  class StmtNode {
  public:
    virtual ~StmtNode() = default;
  private:
  };

  class ExprNode : public StmtNode {
  public:
    virtual ~ExprNode() = default;
  private:
  };

  // =============== [ Exprs ] ===============
  class IdentExpr : public ExprNode {
  public:
    IdentExpr(const std::string &symbol) : m_Symbol(symbol) {}
  private:
    std::string m_Symbol{};
  };

  class NumLitExpr : public ExprNode {
  public:
    NumLitExpr(const Token &value) : m_Value(value) {}
  private:
    Token m_Value;
  };

  class StrLitExpr : public ExprNode {
  public:
    StrLitExpr(const Token &value) : m_Value(value) {}
  private:
    Token m_Value;
  };

  class BinExpr : public ExprNode {
  public:
    BinExpr(uptr<ExprNode> left, uptr<ExprNode> right, Token::Type op)
      : m_Left(std::move(left)), m_Right(std::move(right)), m_Op(op) {}
  private:
    uptr<ExprNode> m_Left{}, m_Right{};
    Token::Type m_Op{};
  };

  class AssignmentExpr : public ExprNode {
  public:
    AssignmentExpr(uptr<ExprNode> assigne, uptr<ExprNode> value) : m_Assigne(std::move(assigne)), m_Value(std::move(value)) {}
  private:
    uptr<ExprNode> m_Assigne{}, m_Value{};
  };
  // =============== [ Exprs ] ===============

  // =============== [ Stmts ] ===============
  class VarDeclStmt : public StmtNode {
  public:
    VarDeclStmt(Token ident, uptr<ExprNode> value) : m_Ident(ident), m_Value(std::move(value)) {}
  private:
    Token m_Ident{};
    uptr<ExprNode> m_Value{};
  };

  class ProgNode : public StmtNode {
  public:
    ProgNode(std::vector<uptr<StmtNode>> stmts)
      : m_Stmts(std::move(stmts)) {}

    inline const std::vector<uptr<StmtNode>> &GetStatements() { return m_Stmts; }
  private:
    std::vector<uptr<StmtNode>> m_Stmts{};
  };
  // =============== [ Stmts ] ===============

  // TODO: add opdef (void op()() {})
  //       Example:
  /*
   class Cat {
   public:
     // Code...
     void opdef()() {
       Print("Meow");
     }
     // ...Code
   };
  */
  inline static std::map<Token::Type, int> BinopPrecedence = {
    { Token::Type::TOKN_PLUS, 10 },   // Add
    { Token::Type::TOKN_MINUS, 10 },  // Sub
    { Token::Type::TOKN_STAR, 20 },   // Mul
    { Token::Type::TOKN_FSLASH, 20 }, // Div
  };

  // TODO: add typedef
  inline static std::map<std::string, std::string> Types = { // name | type (known for compiler and interpreter)
    { "const", "_const" },
    { "int", "int" },
    { "double", "double" },
    { "char", "char" },
    // { "int", "int" },
  };

  static int GetTokPrecedence(Token::Type type) {
    if (BinopPrecedence.find(type) == BinopPrecedence.end()) return -1;
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
  uptr<StmtNode> ParseVarDecl();
  uptr<ExprNode> ParseExpr();
  uptr<ExprNode> ParseBinExpr();
  uptr<ExprNode> ParseAssignmentExpr();
  uptr<ExprNode> ParsePrimExpr();
  std::vector<std::string> ParseType();

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

  inline Token expect(Token::Type type) {
    auto tokn = advance();
    if (tokn.type != type) throw std::runtime_error("Expected token \"" + Token::ToString(type) + "\", but instead got token \"" + Token::ToString(tokn.type) + "\"!");
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
