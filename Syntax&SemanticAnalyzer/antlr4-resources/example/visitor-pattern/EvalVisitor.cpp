#include"EvalVisitor.h"

any EvalVisitor::visitStart(ExpressionParser::StartContext *ctx){
    int value = any_cast<int>(visit(ctx->expression()));
    return value;
}

any EvalVisitor::visitAdd(ExpressionParser::AddContext *ctx){
    int left = any_cast<int>(visit(ctx->expression()));
    int right = any_cast<int>(visit(ctx->term()));
    return left + right;
}

any EvalVisitor::visitExprTerm(ExpressionParser::ExprTermContext *ctx){
    return visit(ctx->term());
}

any EvalVisitor::visitMul(ExpressionParser::MulContext *ctx){
    int left = any_cast<int>(visit(ctx->term()));
    int right = any_cast<int>(visit(ctx->factor()));
    return left * right;
}

any EvalVisitor::visitTermFactor(ExpressionParser::TermFactorContext *ctx){
    return visit(ctx->factor());
}

any EvalVisitor::visitParen(ExpressionParser::ParenContext *ctx){
    return visit(ctx->expression());
}

any EvalVisitor::visitInt(ExpressionParser::IntContext *ctx){
    return stoi(ctx->INT()->getText());
}
