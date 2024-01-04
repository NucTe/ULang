#include "parser.h"

namespace UraniumLang {

  Parser::Parser(std::unique_ptr<Lexer> lex)
    : m_Lexer(std::move(lex)) {}

  Parser::Parser(const std::string &filepath)
    : m_Lexer(std::make_unique<Lexer>(filepath)) {}

  Result<uptr<ProgNode>> Parser::Parse() {
    Result<uptr<ProgNode>> result{};
    m_Tokens = m_Lexer->GetTokens();
    m_CurTok = m_Tokens[(m_Index = 0)];

    std::vector<uptr<StmtNode>> stmts{};
    while (m_CurTok.type != Token::Type::TOKN_EOF) {
      Result<uptr<StmtNode>> curStmtRes = ParseStmt();
      Error err{};
      if (!(err = curStmtRes())) {
        result.SetError(err);
        break;
      }
      auto res = curStmtRes.GetResult();
      if (res.has_value()) stmts.push_back(std::move(res.value()));
    }
    uptr<ProgNode> prog = std::make_unique<ProgNode>(std::move(stmts));
    if (result()()) result.SetResult(std::move(prog));

    return result;
  }

  // private:

  Result<uptr<StmtNode>> Parser::ParseStmt() {
    Result<uptr<StmtNode>> result{};

    auto type = ParseType();
    if (type()()) {
      std::cout << "Czesc!" << std::endl;
    }
    else {
      result.SetError(Error::Failed("Failed to parse statment!"));
    }

    return result;
  }

  Result<uptr<TypeNode>> Parser::ParseType() {
    Result<uptr<TypeNode>> result{};
    std::vector<Token> tokens{};
    bool meow = false;

    while (m_CurTok.type == Token::Type::TOKN_ID && Types.find(*m_CurTok.value) != Types.end()) {
      meow = true; 
      tokens.push_back(m_CurTok); 
      auto res = consume(Token::Type::TOKN_ID);
      if (!res()) {
        result.SetError(res());
        return result;
      }
    }

    if (!meow) {
      result.SetError(Error::Failed("Got no type!"));
    } else {
      uptr<TypeNode> type = std::make_unique<TypeNode>(tokens);
      result.SetResult(std::move(type));
    }

    return result;
  }

}
