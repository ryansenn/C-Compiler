//
// Created by Ryan Senoune on 2024-06-30.
//

#include "name_analysis.h"

std::shared_ptr<Symbol> NameAnalysis::get(std::string identifier) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].find(identifier) != scopes[i].end()) {
            return scopes[i][identifier];
        }
    }
    return std::shared_ptr<Symbol>(nullptr);
}

std::shared_ptr<Symbol> NameAnalysis::get_local(std::string identifier) {
    if (scopes[scopes.size() - 1].find(identifier) != scopes[scopes.size() - 1].end()) {
        return scopes[scopes.size() - 1][identifier];
    }
    return std::shared_ptr<Symbol>(nullptr);
}

void NameAnalysis::put(std::string identifier, std::shared_ptr<Symbol> symbol) {
    scopes[scopes.size() - 1][identifier] = symbol;
}

int NameAnalysis::align(int offset, int alignment) {
    return offset + ((alignment - (offset % alignment)) % alignment);
}

void NameAnalysis::visit(std::shared_ptr<FuncDecl> func) {
    if (get_local(func->name)) {
        if (get_local(func->name)->type == Symbol::Type::PROTO) {
            std::shared_ptr<FunProto> funProto = std::dynamic_pointer_cast<FunProto>(get_local(func->name)->decl);

            if (*(funProto->type) != *(func->type)){
                throw semantic_exception("Conflicting return type in function '" + func->name + "'", func->type->token);
            }

            if (funProto->args.size() != func->args.size()){
                throw semantic_exception("Conflicting number of arguments in function '" + func->name + "'", func->type->token);
            }

            for (int i=0;i<funProto->args.size();i++){
                if (*(func->args[i]->type) != *(funProto->args[i]->type)){
                    throw semantic_exception("Conflicting type in argument '" + func->args[i]->name + "' in function '" + func->name + "'", func->type->token);
                }
            }

            get_local(func->name)->type = Symbol::Type::FUNC;
            get_local(func->name)->decl = func;
        } else {
            throw semantic_exception("Identifier '" + func->name + "' has already been declared in the same scope", func->type->token);
        }
    }
    func->type->accept(*this);
    std::shared_ptr<Symbol> funcSymbol = std::make_shared<Symbol>(Symbol::Type::FUNC, func);
    put(func->name, funcSymbol);
    currFunc = funcSymbol;

    scopes.emplace_back();

    for (auto a : func->args){
        a->accept(*this);
        a->is_local = true;
    }
    for (auto s : func->block->stmts){
        s->accept(*this);
    }

    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<FunProto> funProto) {
    if (get_local(funProto->name)) {
        throw semantic_exception("Function prototype '" + funProto->name + "' has already been declared in the same scope", funProto->type->token);
    }
    put(funProto->name, std::make_shared<Symbol>(Symbol::Type::PROTO, funProto));

    scopes.emplace_back();
    for (auto a : funProto->args){
        a->accept(*this);
    }
    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<Call> call) {
    std::shared_ptr<Symbol> funcDecl = get(call->identifier->value);
    if (!funcDecl) {
        throw semantic_exception("Function '" + call->identifier->value + "' is not declared", call->identifier);
    }
    call->symbol = funcDecl;

    for (auto a : call->args){
        a->accept(*this);
    }
}

void NameAnalysis::visit(std::shared_ptr<VarDecl> varDecl) {
    if (get_local(varDecl->name)) {
        throw semantic_exception("Identifier '" + varDecl->name + "' has already been declared in the same scope", varDecl->type->token);
    }
    varDecl->type->accept(*this);
    put(varDecl->name, std::make_shared<Symbol>(Symbol::Type::VAR, varDecl));
}

void NameAnalysis::visit(std::shared_ptr<Primary> primary) {
    if (primary->token->token_type == TT::IDENTIFIER) {
        std::shared_ptr<Symbol> varDecl = get(primary->token->value);
        if (!varDecl) {
            throw semantic_exception("Variable '" + primary->token->value + "' is not declared", primary->token);
        }
        primary->symbol = varDecl;
    }
}

void NameAnalysis::visit(std::shared_ptr<StructDecl> structDecl) {
    if (get(structDecl->name + "$struct")) {
        throw semantic_exception("Struct '" + structDecl->name + "' has already been declared");
    }

    put(structDecl->name + "$struct", std::make_shared<Symbol>(Symbol::Type::STRUCT, structDecl));

    scopes.emplace_back();
    for (auto v : structDecl->varDecls) {
        v->accept(*this);
    }


    // assigning offsets
    int alignment;
    int maxAlignment = 0;
    int offset = 0;

    for (auto v : structDecl->varDecls){
        alignment = v->type->size;
        maxAlignment = std::max(maxAlignment, alignment);

        offset = align(offset, alignment);
        v->offset = offset;
        offset += v->type->size;
    }

    offset = align(offset, maxAlignment);
    structDecl->size = offset;

    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<Unary> unary) {
    unary->expr1->accept(*this);
}

void NameAnalysis::visit(std::shared_ptr<Binary> binary) {
    binary->expr1->accept(*this);
    binary->expr2->accept(*this);
}

void NameAnalysis::visit(std::shared_ptr<Subscript> subscript) {
    subscript->array->accept(*this);
    subscript->index->accept(*this);
}

void NameAnalysis::visit(std::shared_ptr<Member> member) {
    member->structure->accept(*this);
}

void NameAnalysis::visit(std::shared_ptr<TypeCast> typeCast){
    typeCast->expr1->accept(*this);
}

void NameAnalysis::visit(std::shared_ptr<Block> block) {
    scopes.emplace_back();
    for (auto s : block->stmts) {
        s->accept(*this);
    }
    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<If> i) {
    i->expr1->accept(*this);
    scopes.emplace_back();
    i->stmt1->accept(*this);
    scopes.pop_back();
    scopes.emplace_back();
    if (i->stmt2.has_value()){
        i->stmt2->get()->accept(*this);
    }
    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<While> w) {
    w->expr->accept(*this);
    scopes.emplace_back();
    w->stmt->accept(*this);
    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<Return> ret) {

    ret->funcDecl = currFunc;

    if (ret->expr.has_value()) {
        ret->expr->get()->accept(*this);
    }
}

void NameAnalysis::visit(std::shared_ptr<Program> program) {
    scopes.emplace_back();
    for (auto d : program->decls) {
        d->accept(*this);
    }
    scopes.pop_back();
}

void NameAnalysis::visit(std::shared_ptr<Type> t) {
    if (t->token->token_type == TT::STRUCT){
        for (auto pair : scopes[0]){
            if (pair.second->type == Symbol::Type::STRUCT && pair.first == t->name + "$struct"){
                t->symbol = pair.second;
            }
        }
        if (!t->symbol)
            throw semantic_exception("Type struct '"+ t->name +"' is not declared", t->token);
    }

    // size of type
    switch (t->token->token_type) {
        case TT::INT:
            t->size = 8;
            break;
        case TT::VOID:
            t->size = 0;
            break;
        case TT::CHAR:
            t->size = 8;
            break;
        case TT::STRUCT:
            t->size = std::dynamic_pointer_cast<StructDecl>(t->symbol->decl)->size;
            break;
        default:
            break;
    }

    if (t->pointerCount > 0){
        t->size = 8;
    }
}

void NameAnalysis::visit(std::shared_ptr<Continue> c) {
}

void NameAnalysis::visit(std::shared_ptr<Break> b) {
}