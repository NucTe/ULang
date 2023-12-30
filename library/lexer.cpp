#include "lexer.h"

#include <fstream>

// TODO: Create custom exceptions

namespace UraniumLang {

  Lexer::Lexer(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to read file \"" + filepath + "\"!");
      return;
    }

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    m_Content = std::string(size, ' ');
    file.seekg(0);
    file.read(&m_Content[0], size);
  }

  void Lexer::SetContent(const std::string &content) {
    m_Content = content;
  }

  Token Lexer::GetTok() {
    Token tok{};
    tok.type = Token::Type::TOKN_EOF;
    skipSpaces();
    tok.line = m_Line;
    tok.col = m_Column;
    if (m_Index >= m_Content.size() && m_Char == '\0') return tok;

    if (isdigit(m_Char)) {
      tok.type = Token::Type::TOKN_INT;
      tok.value = "";
      while (isdigit(m_Char) || m_Char == '.') {
        tok.value.value() += m_Char;
        advance();
      }
    }

    else if (isalnum(m_Char)) {
      tok.type = Token::Type::TOKN_ID;
      tok.value = "";
      while (isalnum(m_Char)) {
        tok.value.value() += m_Char;
        advance();
      }
    }

    else if (m_Char == '"') {
      tok.value = "";
      advance();
      while (m_Char != '"' && m_Index < m_Content.size()) {
        tok.value.value() += m_Char;
        advance();
      }
      if (m_Char != '"') {
        throw std::runtime_error("Unexpected End Of File ");
      } else {
        tok.type = Token::Type::TOKN_STRING;
        advance();
      }
    }

    else if (m_Char == '\'') {
      tok.value = "";
      advance();
      tok.value.value() += m_Char;
      if (m_Char == '\\') {
        advance();
        tok.value.value() += m_Char;
      }
      advance();
      if (m_Char != '\'') { throw std::runtime_error(std::string("Unexpected character '")+m_Char+"' at " + std::to_string(m_Line) + ":" + std::to_string(m_Column) + ", expected: `'`!"); return tok; }
      advance();
      tok.type = Token::Type::TOKN_CHAR;
    }

    else {
      switch (m_Char) {
      case '(': { tok.type = Token::Type::TOKN_LPAREN; } break;
      case ')': { tok.type = Token::Type::TOKN_RPAREN; } break;
      case '{': { tok.type = Token::Type::TOKN_LBRACE; } break;
      case '}': { tok.type = Token::Type::TOKN_RBRACE; } break;
      case '[': { tok.type = Token::Type::TOKN_LBRACKET; } break;
      case ']': { tok.type = Token::Type::TOKN_RBRACKET; } break;
      case '<': { tok.type = Token::Type::TOKN_LT; } break;
      case '>': { tok.type = Token::Type::TOKN_GT; } break;
      case ';': { tok.type = Token::Type::TOKN_SEMI; } break;
      case ':': { tok.type = Token::Type::TOKN_COLON; } break;
      case '=': { tok.type = Token::Type::TOKN_EQUALS; } break;
      case '+': { tok.type = Token::Type::TOKN_PLUS; } break;
      case '-': { tok.type = Token::Type::TOKN_MINUS; } break;
      case '*': { tok.type = Token::Type::TOKN_STAR; } break;
      case '/': { tok.type = Token::Type::TOKN_FSLASH; } break;
      case '!': { tok.type = Token::Type::TOKN_EXMARK; } break;
      case '?': { tok.type = Token::Type::TOKN_QUMARK; } break;
      default: { throw std::runtime_error(std::string("Unexpected character '") + m_Char + "' at " + std::to_string(m_Line) + ":" + std::to_string(m_Column)); }
      }
      advance();
    }

    return tok;
  }

  // private:

  void Lexer::advance() {
    if (m_Content.size() <= m_Index && m_Char == '\0') return;
    m_Char = m_Content[m_Index++];
    m_Column++;
    if (m_Char == '\n') { m_Line++; m_Column = 0; }
  }

  void Lexer::skipSpaces() {
    while (m_Char == ' ' || m_Char == '\t' || m_Char == '\n' || iswspace(m_Char) || isspace(m_Char) || m_Char == '\r') advance();
  }

}
