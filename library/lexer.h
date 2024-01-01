#ifndef ULANG_LEXER_H
#define ULANG_LEXER_H

#include "utilities.h" // TODO: Use

#include <iostream>
#include <vector>
#include <optional>

namespace UraniumLang {

  struct Token {
    enum class Type {
      TOKN_ID,
      TOKN_INT, TOKN_STRING, TOKN_CHAR,
      TOKN_LPAREN, TOKN_RPAREN, TOKN_LBRACE, TOKN_RBRACE, TOKN_LBRACKET, TOKN_RBRACKET,
      TOKN_LT, TOKN_GT, // TOKN_LessThan (<), TOKN_GraterThank (>)
      TOKN_SEMI, TOKN_COLON,
      TOKN_EQUALS, TOKN_PLUS, TOKN_MINUS, TOKN_STAR, TOKN_FSLASH, TOKN_EXMARK, TOKN_QUMARK,
      TOKN_EOF
    } type;
    std::optional<std::string> value;
    int line, col;
    static std::string ToString(Type type) {
      switch (type)
      {
      case Type::TOKN_ID:       return "TOKN_ID";
      case Type::TOKN_INT:      return "TOKN_INT";
      case Type::TOKN_STRING:   return "TOKN_STRING";
      case Type::TOKN_CHAR:     return "TOKN_CHAR";
      case Type::TOKN_LPAREN:   return "TOKN_LPAREN";
      case Type::TOKN_RPAREN:   return "TOKN_RPAREN";
      case Type::TOKN_LBRACE:   return "TOKN_LBRACE";
      case Type::TOKN_RBRACE:   return "TOKN_RBRACE";
      case Type::TOKN_LBRACKET: return "TOKN_LBRACKET";
      case Type::TOKN_RBRACKET: return "TOKN_RBRACKET";
      case Type::TOKN_LT:       return "TOKN_LT";
      case Type::TOKN_GT:       return "TOKN_GT";
      case Type::TOKN_SEMI:     return "TOKN_SEMI";
      case Type::TOKN_COLON:    return "TOKN_COLON";
      case Type::TOKN_EQUALS:   return "TOKN_EQUALS";
      case Type::TOKN_PLUS:     return "TOKN_PLUS";
      case Type::TOKN_MINUS:    return "TOKN_MINUS";
      case Type::TOKN_STAR:     return "TOKN_STAR";
      case Type::TOKN_FSLASH:   return "TOKN_FSLASH";
      case Type::TOKN_EXMARK:   return "TOKN_EXMARK";
      case Type::TOKN_QUMARK:   return "TOKN_QUMARK";
      case Type::TOKN_EOF:      return "<EndOfFile>";
      }
      return "<NONE>";
    }
  };

  class Lexer {
  public:
  Lexer() = default;
  Lexer(const std::string &filepath);

  void SetContent(const std::string &content);
  std::vector<Token> GetTokens();
  private:
  Token GetTok();
  void advance();
  void skipSpaces();
  private:
    std::string m_Content = "";
    char m_Char = ' ';
    int m_Index = 0, m_Line = 0, m_Column = 0;
  };

}

#endif
