#include "parser.h"

namespace UraniumLang {

  static std::unique_ptr<IRBuilder<>> Builder = std::make_unique<IRBuilder<>>(*Context);

  Value *VarDefExprAST::Generate() {
    AllocaInst *OldBinding;

    Value *InitVal 
      = m_Expr == nullptr ? ConstantFP::get(*Context, APFloat(0.0))
        : m_Expr->Generate();
    if (!InitVal) throw std::runtime_error("Failed!");

    AllocaInst *Alloca = nullptr;
    Function *TheFunction = Builder->GetInsertBlock() ? Builder->GetInsertBlock()->getParent() : GlobalScope;
    Alloca = CreateEntryBlockAlloca(TheFunction, m_Name);
    
    if (!Alloca) throw std::runtime_error("Failed to VarDef!");
    Builder->CreateStore(InitVal, Alloca);

    OldBinding = NamedValues[m_Name];

    NamedValues[m_Name] = Alloca;

    NamedValues[m_Name] = OldBinding;

    return OldBinding;
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
    // Global Scope (for global variables)
    {
      FunctionType *FT = FunctionType::get(Type::getVoidTy(*Context), {}, false);
      GlobalScope = Function::Create(FT, Function::PrivateLinkage, "global", TheModule.get());
      BasicBlock *BB = BasicBlock::Create(*Context, "entry", GlobalScope);
      Builder->SetInsertPoint(BB);
    }

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
