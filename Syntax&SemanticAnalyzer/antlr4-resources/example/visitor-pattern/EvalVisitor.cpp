#include "EvalVisitor.h"

std::any EvalVisitor::visitStart(ExpressionParser::StartContext *ctx){
    int value = std::any_cast<int>(visit(ctx->expression()));
    return value;
}

std::any EvalVisitor::visitAdd(ExpressionParser::AddContext *ctx){
    int left = std::any_cast<int>(visit(ctx->expression()));
    int right = std::any_cast<int>(visit(ctx->term()));
    return left + right;
}

std::any EvalVisitor::visitExprTerm(ExpressionParser::ExprTermContext *ctx){
    return visit(ctx->term());
}

std::any EvalVisitor::visitMul(ExpressionParser::MulContext *ctx){
    int left = std::any_cast<int>(visit(ctx->term()));
    int right = std::any_cast<int>(visit(ctx->factor()));
    return left * right;
}

std::any EvalVisitor::visitTermFactor(ExpressionParser::TermFactorContext *ctx){
    return visit(ctx->factor());
}

std::any EvalVisitor::visitParen(ExpressionParser::ParenContext *ctx){
    return visit(ctx->expression());
}

std::any EvalVisitor::visitInt(ExpressionParser::IntContext *ctx){
    return std::stoi(ctx->INT()->getText());
}
