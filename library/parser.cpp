#include "parser.h"

namespace UraniumLang {

  Parser::Parser(std::unique_ptr<Lexer> lex)
    : m_Lexer(std::move(lex)) { Initialize(); }

  Parser::Parser(const std::string &filepath)
    : m_Lexer(std::make_unique<Lexer>(filepath)) { Initialize(); }

  uptr<ProgNode> Parser::Parse() {
    std::vector<uptr<StmtNode>> statements{};
    while (m_CurTok.type != Token::Type::TOKN_EOF) {
      // std::cout << m_CurTok << std::endl;
      statements.push_back(std::move(ParseStmt()));
    }

    return std::make_unique<ProgNode>(std::move(statements));
  }

  // private:

  void Parser::Initialize() {
    m_Tokens = m_Lexer->GetTokens();

    m_CurTok = m_Tokens[m_Index = 0];
  }

  uptr<StmtNode> Parser::ParseStmt() {
    return ParseExpr();
  }

  uptr<ExprNode> Parser::ParseExpr() {
    auto left = ParsePrimExpr();

    while (m_CurTok.type != Token::Type::TOKN_EOF && GetTokPrecedence(m_CurTok.type) >= 0) {
      auto op = advance().type;
      auto right = ParsePrimExpr();
      left = std::make_unique<BinExpr>(std::move(left), std::move(right), op);
    }

    return left;
  }

  uptr<ExprNode> Parser::ParsePrimExpr() {
    auto tknTy = m_CurTok.type;

    switch (tknTy)
    {
    case Token::Type::TOKN_ID: {
      return std::make_unique<IdentExpr>(advance().value.value());
    }
    case Token::Type::TOKN_NUM: return std::make_unique<NumLitExpr>(advance());
    default: {
      return nullptr;
    }
    }
  }

}
