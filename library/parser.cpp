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
      // NamedValues[m_Name] = OldBinding; - once we implement scopes = pop variables
    } 
    else {
      OldGBinding = GlobalValues[m_Name];
      GlobalValues[m_Name] = GlobalVar.value();
      // GlobalValues[m_Name] = OldGBinding; - once we implement scopes = pop variables
    }

    return (isLocal ? (Value*)NamedValues[m_Name] : (Value*)GlobalValues[m_Name]);
  }

  GlobalVariable *VarDefExprAST::CreateGlobal() {
    GlobalVariable* gVar = (GlobalVariable*)TheModule->getOrInsertGlobal(m_Name.c_str(), Builder->getDoubleTy());
    gVar->setLinkage(GlobalValue::PrivateLinkage);
    gVar->setAlignment(Align(alignof(double)));

    Value *InitVal = !m_Expr ? ConstantFP::get(*Context, APFloat(0.0)) : m_Expr->Generate();
    gVar->setInitializer(cast<Constant>(InitVal));

    return gVar;
  }

  Value *VariableExprAST::Generate() {
    if (GlobalValues.find(m_Name) != GlobalValues.end()) {
      auto var = GlobalValues[m_Name];
      if (!var) {
          throw std::runtime_error("Global variable pointer is null: \"" + m_Name + "\"");
      }
      return Builder->CreateLoad(var->getValueType(), var, m_Name.c_str());
    } else {
      if (NamedValues.find(m_Name) == NamedValues.end()) {
        throw std::runtime_error("Unknown variable name: \"" + m_Name + "\"");
      }
      AllocaInst *A = NamedValues[m_Name];
      if (!A) {
        throw std::runtime_error("Local variable pointer is null: \"" + m_Name + "\"");
      }
      return Builder->CreateLoad(A->getAllocatedType(), A, m_Name.c_str());
    }
  }

  Value *BinOpExprAST::Generate() {
    Value *L = m_LHS->Generate();
    Value *R = m_RHS->Generate();
    if (!L || !R)
      return nullptr;

    switch (m_OpCode) {
    case Token::Type::TOKN_PLUS:
      return Builder->CreateFAdd(L, R, "addtmp");
    case Token::Type::TOKN_MINUS:
      return Builder->CreateFSub(L, R, "subtmp");
    case Token::Type::TOKN_STAR:
      return Builder->CreateFMul(L, R, "multmp");
    case Token::Type::TOKN_FSLASH:
      return Builder->CreateFDiv(L, R, "divtmp");
    default: throw std::runtime_error("Invalid binary operator " + Token::ToString(m_OpCode) + "!");
    }
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
    while (peek().type != Token::Type::TOKN_EOF) {
      prog->AddStmt(ParseStmt());
      if (!consume(Token::Type::TOKN_SEMI)) {
        throw ParserUnexpectedTokEx(Token::Type::TOKN_SEMI, m_CurTok);
      }
    }
    return prog;
  }

  std::unique_ptr<StmtAST> Parser::ParseStmt() {
    if (m_CurTok.type == Token::Type::TOKN_ID) {
      auto tok = m_CurTok;
      if (peek().type == Token::Type::TOKN_ID) {
        auto varDef = std::make_unique<VarDefExprAST>(tok, m_CurTok.value.value());
        Token::Type type = m_CurTok.type;
        if (consume(Token::Type::TOKN_EQUALS)) varDef->SetExpr(ParseExpr());
        return varDef;
      } else if (m_CurTok.type == Token::Type::TOKN_LPAREN) {
        // TODO: Function Call
      }
    } else {
      throw ParserUnexpectedTokEx(Token::Type::TOKN_ID, m_CurTok);
    }
    throw std::runtime_error("Failed to parse!");
    return nullptr;
  }

  std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, 
                                                 std::unique_ptr<ExprAST> LHS) {
    while (true) {
      int TokPrec = GetTokPrecedence(peek().type);
      if (TokPrec < ExprPrec) return LHS;

      Token::Type BinOp = m_CurTok.type;

      std::unique_ptr<ExprAST> RHS = ParseTerm();
      if (!RHS)
        return nullptr;

      int NextPrec = GetTokPrecedence(peek().type);
      if (TokPrec < NextPrec) {
        RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
        if (!RHS)
          return nullptr;
      }

      LHS = std::make_unique<BinOpExprAST>(std::move(LHS), std::move(RHS), BinOp);
    }
  }

  std::unique_ptr<ExprAST> Parser::ParseExpr() {
    if (peek().type == Token::Type::TOKN_ID) return std::make_unique<VariableExprAST>(m_CurTok.value.value());
    auto lhs = ParseTerm();
    if (!lhs) {return nullptr;}
    
    return ParseBinOpRHS(0, std::move(lhs));
  }

  std::unique_ptr<TermAST> Parser::ParseTerm() {
    Token::Type type = m_CurTok.type;
    if (type == Token::Type::TOKN_INT || type == Token::Type::TOKN_ID) {
      return std::make_unique<TermAST>(m_CurTok);
    }
    return nullptr;
  }

}
