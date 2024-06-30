//
// Created by Ryan Senoune on 2024-05-21.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <variant>
#include <vector>
#include <string>
#include <optional>
#include "../lexer/token.h"

class Visitor{
public:
    virtual void visit(std::shared_ptr<class Program> program) = 0;
    virtual void visit(std::shared_ptr<class FuncDecl> func) = 0;
    virtual void visit(std::shared_ptr<class Block> block) = 0;
    virtual void visit(std::shared_ptr<class Return> ret) = 0;
    virtual void visit(std::shared_ptr<class If> block) = 0;
    virtual void visit(std::shared_ptr<class While> block) = 0;
    virtual void visit(std::shared_ptr<class Break> block) = 0;
    virtual void visit(std::shared_ptr<class Continue> block) = 0;
    virtual void visit(std::shared_ptr<class VarDecl> var) = 0;
    virtual void visit(std::shared_ptr<class Subscript> subscript) = 0;
    virtual void visit(std::shared_ptr<class Member> member) = 0;
    virtual void visit(std::shared_ptr<class Call> call) = 0;
    virtual void visit(std::shared_ptr<class Primary> primary) = 0;
    virtual void visit(std::shared_ptr<class Unary> unary) = 0;
    virtual void visit(std::shared_ptr<class Binary> binary) = 0;
    virtual void visit(std::shared_ptr<class Type> type) = 0;
    virtual void visit(std::shared_ptr<class FunProto> funProto) = 0;
    virtual void visit(std::shared_ptr<class StructDecl> funProto) = 0;
};

struct Symbol;

struct Stmt {
    virtual void accept(Visitor& visitor) = 0;
};

struct Expr : Stmt{
    virtual void accept(Visitor& visitor) = 0;
};

struct Decl {
    Decl(){}
    virtual void accept(Visitor& visitor) = 0;
};

struct Type : std::enable_shared_from_this<Type> {
public:
    std::shared_ptr<Token> token; // Inside of token we find the type (Token.token_type)
    std::string name;
    int pointerCount;
    std::vector<int> arraySize;
    std::shared_ptr<Symbol> symbol;

    Type(std::shared_ptr<Token> token) : token(std::move(token)), pointerCount(0), arraySize(0) {}

    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Subscript : Expr, std::enable_shared_from_this<Subscript> {
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;
    Subscript(std::shared_ptr<Expr> array, std::shared_ptr<Expr> index) : array(std::move(array)), index(std::move(index)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Member : Expr, std::enable_shared_from_this<Member> {
    std::shared_ptr<Expr> structure;
    std::string member;
    Member(std::shared_ptr<Expr> structure, std::string member) : structure(std::move(structure)), member(member) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};


struct Primary : Expr, std::enable_shared_from_this<Primary> {
    std::shared_ptr<Token> token; // Inside of the token, we find the type (token.token_type) and value (token.value)
    std::shared_ptr<Symbol> symbol;
    Primary(std::shared_ptr<Token> token) : token(std::move(token)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Call : Expr, std::enable_shared_from_this<Call> {
    std::shared_ptr<Token> identifier; // The operator is held into Token.value
    std::vector<std::shared_ptr<Expr>> args;
    std::shared_ptr<Symbol> symbol;
    Call(std::shared_ptr<Token> identifier) : identifier(std::move(identifier)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Unary : Expr, std::enable_shared_from_this<Unary> {
    std::variant<std::shared_ptr<Token>, std::shared_ptr<Type>> op; // The operator is held into Token.token_type
    std::shared_ptr<Expr> expr1;
    Unary(std::shared_ptr<Token> o, std::shared_ptr<Expr> e1) : op(std::move(o)), expr1(std::move(e1)) {}
    Unary(std::shared_ptr<Type> o, std::shared_ptr<Expr> e1) : op(std::move(o)), expr1(std::move(e1)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Binary : Expr, std::enable_shared_from_this<Binary> {
    std::shared_ptr<Expr> expr1;
    std::shared_ptr<Token> op; // The operator is held into Token.token_type
    std::shared_ptr<Expr> expr2;
    Binary(std::shared_ptr<Expr> e1, std::shared_ptr<Token> o, std::shared_ptr<Expr> e2) : expr1(std::move(e1)), op(std::move(o)), expr2(std::move(e2)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct VarDecl : Decl, Stmt, std::enable_shared_from_this<VarDecl> {
    std::shared_ptr<Type> type;
    std::string name;
    VarDecl(std::shared_ptr<Type> t, std::string& n) : name(n), type(std::move(t)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct StructDecl : Decl, std::enable_shared_from_this<StructDecl> {
    std::vector<std::shared_ptr<VarDecl>> varDecls;
    std::string name;
    StructDecl(std::string& n) : name(n) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Return : Stmt, std::enable_shared_from_this<Return> {
public:
    std::optional<std::shared_ptr<Expr>> expr;
    Return() {}
    Return(std::shared_ptr<Expr> e) : expr(std::move(e)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct If : Stmt, std::enable_shared_from_this<If> {
public:
    std::shared_ptr<Expr> expr1;
    std::shared_ptr<Stmt> stmt1;
    std::optional<std::shared_ptr<Stmt>> stmt2;
    If(std::shared_ptr<Expr> e1, std::shared_ptr<Stmt> s1) : expr1(std::move(e1)), stmt1(std::move(s1)) {}
    If(std::shared_ptr<Expr> e1, std::shared_ptr<Stmt> s1, std::shared_ptr<Stmt> s2) : expr1(std::move(e1)), stmt1(std::move(s1)), stmt2(std::move(s2)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct While : Stmt, std::enable_shared_from_this<While> {
public:
    std::shared_ptr<Expr> expr;
    std::shared_ptr<Stmt> stmt;
    While(std::shared_ptr<Expr> e, std::shared_ptr<Stmt> s) : expr(std::move(e)), stmt(std::move(s)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Block : Stmt, std::enable_shared_from_this<Block> {
    std::vector<std::shared_ptr<Stmt>> stmts;
    Block(std::vector<std::shared_ptr<Stmt>> s) : stmts(std::move(s)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Continue : Stmt, std::enable_shared_from_this<Continue> {
    Continue() {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Break : Stmt, std::enable_shared_from_this<Break> {
    Break() {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct FuncDecl : Decl, std::enable_shared_from_this<FuncDecl> {
    std::shared_ptr<Type> type;
    std::string name;
    std::vector<std::shared_ptr<VarDecl>> args;
    std::shared_ptr<Block> block;
    FuncDecl(std::shared_ptr<Type> t, std::string& n, std::vector<std::shared_ptr<VarDecl>> a, std::shared_ptr<Block> b) : name(n), type(std::move(t)), args(std::move(a)), block(std::move(b)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct FunProto : Decl, std::enable_shared_from_this<FunProto> {
    std::shared_ptr<Type> type;
    std::string name;
    std::vector<std::shared_ptr<VarDecl>> args;
    FunProto(std::shared_ptr<Type> t,std::string& n, std::vector<std::shared_ptr<VarDecl>> a) : name(n), type(std::move(t)), args(std::move(a)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};

struct Program : public std::enable_shared_from_this<Program>{
    std::vector<std::shared_ptr<Decl>> decls;
    Program(std::vector<std::shared_ptr<Decl>> d) : decls(std::move(d)) {}
    void accept(Visitor& visitor){
        visitor.visit(shared_from_this());
    }
};


class PrintVisitor : public Visitor{

    std::string space = "    ";
    std::string indent = "";
    int w = 1;

    void incr(){
        indent += space;
    }

    void decr(){
        if (indent.size() >= space.size()){
            indent.erase(indent.size() - space.size());
        }
    }

    void visit(std::shared_ptr<Program> program) override{
        std::cout << "Program (" << std::endl;
        incr();
        for (auto d : program->decls){
            std::cout << indent;
            d->accept(*this);
            std::cout << std::endl;
        }
        decr();
        std::cout << ")";
    }

    void visit(std::shared_ptr<FuncDecl> func) override{
        std::cout << "FuncDecl " << func->name << "(";
        bool first = true;
        for(auto a : func->args){
            if (!first){
                std::cout << ", ";
            }
            first = false;
            a->accept(*this);
        }
        std::cout <<") (";
        func->type->accept(*this);
        std::cout << ") (" << std::endl;
        incr();
        func->block->accept(*this);
        decr();
        std::cout << indent << ")";
    }

    void visit(std::shared_ptr<FunProto> funProto) override{
        std::cout << "FunProto " << funProto->name << "(";
        bool first = true;
        for(auto a : funProto->args){
            if (!first){
                std::cout << ", ";
            }
            first = false;
            a->accept(*this);
        }
        std::cout << ") (";
        funProto->type->accept(*this);
        std::cout << ")";

    }

    void visit(std::shared_ptr<Block> block) override{
        for (auto s : block->stmts){
            std::cout << indent;
            s->accept(*this);
            std::cout << std::endl;
        }
    }

    void visit(std::shared_ptr<If> i) override{
        std::cout << "If (";
        i->expr1->accept(*this);
        std::cout << ") (" << std::endl;
        incr();
        i->stmt1->accept(*this);
        decr();
        std::cout << indent << ")" << std::endl;

        if (i->stmt2.has_value()){
            std::cout << indent << "Else (" << std::endl;
            incr();
            i->stmt2->get()->accept(*this);
            decr();
            std::cout << indent << ")" << std::endl;
        }
    }

    void visit(std::shared_ptr<While> w) override{
        std::cout << "While (";
        w->expr->accept(*this);
        std::cout << ") (" << std::endl;
        incr();
        w->stmt->accept(*this);
        decr();
        std::cout << indent << ")";
    }

    void visit(std::shared_ptr<Continue> c) override{
        std::cout << "Continue";
    }

    void visit(std::shared_ptr<Break> b) override{
        std::cout << "Break";
    }

    void visit(std::shared_ptr<Return> ret) override{
        std::cout << "return( ";
        incr();
        if (ret->expr.has_value()){
            ret->expr.value()->accept(*this);
        }
        decr();
        std::cout << " )";
    }

    void visit(std::shared_ptr<VarDecl> varDecl) override{
        std::cout << "VarDecl(";
        varDecl->type->accept(*this);
        std::cout << ")";
    }

    void visit(std::shared_ptr<Type> type) override{
        std::cout << type->token->token_type << ", p count: " << type->pointerCount << ", array size: ";
        for (auto p:type->arraySize){
            std::cout << p << " ";
        }
    }

    void visit(std::shared_ptr<Call> call) override{
        std::cout << "Func Call " << call->identifier->value << " (";
        bool first = true;
        for (auto& arg: call->args){
            if (!first){
                std::cout << ", ";
            }
            first = false;
            arg->accept(*this);
        }
        std::cout << " )";
    }

    void visit(std::shared_ptr<StructDecl> structDecl) override{
        std::cout << "StructDecl " << structDecl->name << " (";
        for (int i=0;i<structDecl->varDecls.size();i++){
            if (i!=0){
                std::cout << ", ";
            }
            structDecl->varDecls[i]->accept(*this);
        }
        std::cout <<")";
    }

    void visit(std::shared_ptr<class Unary> unary) override{
        if (std::holds_alternative<std::shared_ptr<Type>>(unary->op)){
            std::cout << "(";
            std::get<std::shared_ptr<Type>>(unary->op)->accept(*this);
            std::cout << ")";
        }
        else{
            std::cout << std::get<std::shared_ptr<Token>>(unary->op)->token_type;
        }

        std::cout << "(";
        unary->expr1->accept(*this);
        std::cout << ")";
    }

    void visit(std::shared_ptr<class Binary> binary) override{
        std::cout << "(";
        binary->expr1->accept(*this);
        std::cout << " " << binary->op->token_type << " ";
        binary->expr2->accept(*this);
        std::cout << ")";
    }

    void visit(std::shared_ptr<Primary> primary) override{
        std::cout << primary->token->value;
    }

    void visit(std::shared_ptr<Subscript> subscript) override{
        std::cout << "(";
        subscript->array->accept(*this);
        std::cout << "[";
        subscript->index->accept(*this);
        std::cout << "]";
        std::cout << ")";
    }

    void visit(std::shared_ptr<Member> member) override{
        std::cout << "(";
        member->structure->accept(*this);
        std::cout << "." << member->member;
        std::cout << ")";
    }

};

#endif //COMPILER_AST_H
