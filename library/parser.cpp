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
    auto tokn = m_CurTok;
    uptr<StmtNode> res = nullptr;

    if (tokn.type == Token::Type::TOKN_ID) {
      if ((res = ParseVarDecl())) return res;
    }
    
    res = ParseExpr();
    expect(Token::Type::TOKN_SEMI);
    return res;
  }

  uptr<StmtNode> Parser::ParseVarDecl() {
    auto types = ParseType();
    if (types.empty()) return nullptr;
    auto name = expect(Token::Type::TOKN_ID);
    auto tokn = m_CurTok;
    uptr<VarDeclStmt> res = nullptr;
    if (tokn.type == Token::Type::TOKN_EQUALS) {
      expect(Token::Type::TOKN_EQUALS);
      auto expr = ParseExpr();
      res = std::make_unique<VarDeclStmt>(name, std::move(expr));
    }
    else if (tokn.type == Token::Type::TOKN_SEMI) res = std::make_unique<VarDeclStmt>(name, nullptr);
    else throw std::runtime_error("Unexpected token \"" + Token::ToString(m_CurTok.type) + "\", expected \"TOKN_SEMI\" or \"TOKN_EQUALS\"!");
    expect(Token::Type::TOKN_SEMI);

    return res ? std::move(res) : nullptr;
  }

  uptr<ExprNode> Parser::ParseExpr() {
    return ParseAssignmentExpr();
  }

  uptr<ExprNode> Parser::ParseAssignmentExpr() {
    auto left = ParseBinExpr();
    
    if (m_CurTok.type == Token::Type::TOKN_EQUALS) {
      expect(Token::Type::TOKN_EQUALS);
      
      return std::make_unique<AssignmentExpr>(std::move(left), std::move(ParseAssignmentExpr()));
    }
    
    return left;
  }

  uptr<ExprNode> Parser::ParseBinExpr() {
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
    case Token::Type::TOKN_ID:     return std::make_unique<IdentExpr>(advance().value.value());
    case Token::Type::TOKN_NUM:    return std::make_unique<NumLitExpr>(advance());
    case Token::Type::TOKN_STRING: return std::make_unique<StrLitExpr>(advance());
    default:                       return nullptr;
    }
  }

  std::vector<std::string> Parser::ParseType() {
    std::vector<std::string> types{};
    while (m_CurTok.type == Token::Type::TOKN_ID && Types.find(m_CurTok.value.value()) != Types.end()) types.push_back(advance().value.value());
    return types;
  }

}
