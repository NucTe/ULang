#include "parser.h"

namespace UraniumLang {

  static std::unique_ptr<IRBuilder<>> Builder = std::make_unique<IRBuilder<>>(*Context);

  Value *VarDefExprAST::Generate() {
    AllocaInst *OldBinding;
    GlobalVariable *OldGBinding;

    Value *InitVal 
      = m_Expr == nullptr ? ConstantFP::get(*Context, APFloat(0.0))
        : m_Expr->Generate();
    if (!InitVal) throw std::runtime_error("Failed!");

    bool isLocal;
    std::optional<AllocaInst *> Alloca;
    std::optional<GlobalVariable *> GlobalVar;
    if ((isLocal = Builder->GetInsertBlock())) {
      Function *TheFunction = Builder->GetInsertBlock()->getParent();
      Alloca = CreateEntryBlockAlloca(TheFunction, m_Name);
      Builder->CreateStore(InitVal, Alloca.value());
    }
    else {
      GlobalVar = CreateGlobal();
    }

    if (!(Alloca.has_value() || GlobalVar.has_value())) throw std::runtime_error("Failed to VarDef!");

    if (isLocal) {
      OldBinding = NamedValues[m_Name];

      NamedValues[m_Name] = Alloca.value();

      NamedValues[m_Name] = OldBinding;
    } 
    else {
      OldGBinding = GlobalValues[m_Name];

      GlobalValues[m_Name] = GlobalVar.value();

      GlobalValues[m_Name] = OldGBinding;
    }

    return (isLocal ? (Value*)OldBinding : (Value*)OldGBinding);
  }

  GlobalVariable *VarDefExprAST::CreateGlobal() {
    TheModule->getOrInsertGlobal(m_Name, Builder->getDoubleTy());
    GlobalVariable* gVar = TheModule->getNamedGlobal(m_Name);
    gVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
    gVar->setAlignment(Align(alignof(double)));
    ConstantFP *init = ConstantFP::get(*Context, APFloat(0.0));
    gVar->setInitializer(init);
    return gVar;
  }

  Value *VariableExprAST::Generate() {
    AllocaInst *A = NamedValues[m_Name];
    if (!A) throw std::runtime_error("Unknow variable name: \"" + m_Name + "\""); // TODO: Make exception
    return Builder->CreateLoad(A->getAllocatedType(), A, m_Name.c_str());
  }

  Value *ProgramAST::Generate() {
    Value *val = nullptr;
    for (auto &stmt : m_Stmts) val = stmt->Generate();
    TheModule->print(errs(), nullptr);
    return val;
  }

  Parser::Parser(std::unique_ptr<Lexer> lex)
    : m_Lexer(std::move(lex)) {}

  Parser::Parser(const std::string &filepath)
    : m_Lexer(std::make_unique<Lexer>(filepath)) {}

  std::unique_ptr<ProgramAST> Parser::Parse() {
    Context = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("UraniumLang JIT", *Context);
    Builder = std::make_unique<IRBuilder<>>(*Context);

    std::unique_ptr<ProgramAST> program 
      = ParseProgram();
    return program;
  }

  // private:

  std::unique_ptr<ProgramAST> Parser::ParseProgram() {
    auto prog = std::make_unique<ProgramAST>();
    while (peek().type != Token::Type::TOKN_EOF) prog->AddStmt(ParseStmt());
    return prog;
  }

  std::unique_ptr<StmtAST> Parser::ParseStmt() {
    if (m_CurTok.type == Token::Type::TOKN_ID) {
      auto tok = m_CurTok;
      if (peek().type == Token::Type::TOKN_ID) {
        auto varDef = std::make_unique<VarDefExprAST>(tok, m_CurTok.value.value());
        if (consume(Token::Type::TOKN_EQUALS)) { varDef->SetExpr(ParseExpr()); if (!consume(Token::Type::TOKN_SEMI)) { throw ParserUnexpectedTokEx(Token::Type::TOKN_SEMI, m_CurTok); } }
        else if (m_CurTok.type != Token::Type::TOKN_SEMI)
          throw ParserUnexpectedTokEx(Token::Type::TOKN_SEMI, m_CurTok);
        return varDef;
      } 
      else if (m_CurTok.type == Token::Type::TOKN_LPAREN) {
        // TODO: Function Call
      }
    } else {
      throw ParserUnexpectedTokEx(Token::Type::TOKN_ID, m_CurTok);
    }
    return nullptr;
  }

  std::unique_ptr<ExprAST> Parser::ParseExpr() {
    if (peek().type == Token::Type::TOKN_ID) return std::make_unique<VariableExprAST>(m_CurTok.value.value());
    auto lhs = ParseTerm();
    if (!lhs) {return nullptr;}
    return std::move(lhs);
  }

  std::unique_ptr<TermAST> Parser::ParseTerm() {
    Token::Type type = m_CurTok.type;
    if (type == Token::Type::TOKN_INT || type == Token::Type::TOKN_ID) {
      return std::make_unique<TermAST>(m_CurTok);
    }
    return nullptr;
  }

}
