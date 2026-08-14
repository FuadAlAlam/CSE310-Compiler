#include "EvalListener.h"

void EvalListener::exitAdd(ExpressionParser::AddContext *ctx){
    int left = values[ctx->expression()];
    int right = values[ctx->term()];
    values[ctx] = left + right;
}

void EvalListener::exitExprTerm(ExpressionParser::ExprTermContext *ctx){
    values[ctx] = values[ctx->term()];
}

void EvalListener::exitMul(ExpressionParser::MulContext *ctx){
    int left = values[ctx->term()];
    int right = values[ctx->factor()];
    values[ctx] = left * right;
}

void EvalListener::exitTermFactor(ExpressionParser::TermFactorContext *ctx){
    values[ctx] = values[ctx->factor()];
}

void EvalListener::exitParen(ExpressionParser::ParenContext *ctx){
    values[ctx] = values[ctx->expression()];
}

void EvalListener::exitInt(ExpressionParser::IntContext *ctx){
    values[ctx] = std::stoi(ctx->INT()->getText());
}

void EvalListener::exitStart(ExpressionParser::StartContext *ctx){
    finalResult = values[ctx->expression()];
}
