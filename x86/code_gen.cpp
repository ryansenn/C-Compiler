//
// Created by Ryan Senoune on 2024-07-08.
//

#include "code_gen.h"

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Program> p) {

    file << "section .text" << std::endl;
    file << "global start" << std::endl;
    for (auto d : p->decls){
        d->accept(*this);
    }
    file << "start:" << std::endl;
    file << "call main" << std::endl;
    file << "mov rdi, rax" << std::endl;
    file << "mov rax, 0x2000001" << std::endl;
    file << "syscall" << std::endl;

    return NO_REGISTER;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<FuncDecl> f) {

    file << f->name << ":" << std::endl;
    f->block->accept(*this);

    return NO_REGISTER;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Block> f) {
    for (auto s : f->stmts){
        s->accept(*this);
    }

    return NO_REGISTER;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Return> f) {

    if (f->expr.has_value()){
        std::shared_ptr<Register> r = f->expr->get()->accept(*this);
        file << "mov rax, " + r->name << std::endl;
        freeRegister(r);
    }
    file << "ret" << std::endl;

    return NO_REGISTER;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Primary> p){

    std::shared_ptr<Register> r = getRegister();

    switch (p->token->token_type) {
        case TT::INT_LITERAL:
            file << "mov " + r->name + ", " + p->token->value << std::endl;
            break;
        case TT::CHAR_LITERAL:
            //file << "mov " + r->name + ", " + p->token->value << std::endl;
            break;
        case TT::IDENTIFIER:
            //file << "mov " + r->name + ", " + p->token->value << std::endl;
            break;
    }

    return r;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Binary> b){
    std::shared_ptr<Register> r1 = b->expr1->accept(*this);
    std::shared_ptr<Register> r2 = b->expr2->accept(*this);

    std::shared_ptr<Register> r = getRegister();

    switch (b->op->token_type) {
        case TT::PLUS:
            file << "mov " + r->name + ", " + r1->name << std::endl;
            file << "add " + r->name + ", " + r2->name << std::endl;
            break;
        case TT::MINUS:
            break;
        case TT::ASTERISK:
            break;
        case TT::DIV:
            break;
        case TT::ASSIGN:
            break;
        case TT::REM:
            break;
        case TT::LE:
            break;
        case TT::LT:
            break;
        case TT::GE:
            break;
        case TT::GT:
            break;
        case TT::EQ:
            break;
        case TT::NE:
            break;
        case TT::LOGOR:
            break;
        case TT::LOGAND:
            break;

    }

    freeRegister(r1);
    freeRegister(r2);

    return r;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<If> f) {
    return NULL;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<While>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Break>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Continue>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<VarDecl>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Subscript>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Member>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Call>){
    return NULL;
}

std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Unary>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<TypeCast>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<Type>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<FunProto>){
    return NULL;
}
std::shared_ptr<Register> CodeGen::visit(std::shared_ptr<StructDecl>){
    return NULL;
}