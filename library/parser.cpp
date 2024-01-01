#include "parser.h"

namespace UraniumLang {


  Parser::Parser(std::unique_ptr<Lexer> lex)
    : m_Lexer(std::move(lex)) {}

  Parser::Parser(const std::string &filepath)
    : m_Lexer(std::make_unique<Lexer>(filepath)) {}

  Result<uptr<ProgNode>> Parser::Parse() {
    Result<uptr<ProgNode>> result{};
    m_Tokens = m_Lexer->GetTokens();
    
    Result<uptr<StmtNode>> curStmtRes{};
    bool loop = true;
    while (loop) {
      curStmtRes = ParseStmt();
      if (!curStmtRes().first) {
        loop = false;
        result.SetError(curStmtRes.GetErr());
      }
    }

    return result;
  }

  // private:

  Result<uptr<StmtNode>> Parser::ParseStmt() {
    Result<uptr<StmtNode>> result(UraniumLang::Error(UraniumLang::Error::ErrCode::NOT_IMPL));
    // TODO: Implement
    return result;
  }

}
