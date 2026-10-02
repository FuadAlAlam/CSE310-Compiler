#pragma once

#include"ExpressionBaseVisitor.h"

class EvalVisitor : public ExpressionBaseVisitor{
public:
    any visitStart(ExpressionParser::StartContext *ctx) override;
    any visitAdd(ExpressionParser::AddContext *ctx) override;
    any visitExprTerm(ExpressionParser::ExprTermContext *ctx) override;
    any visitMul(ExpressionParser::MulContext *ctx) override;
    any visitTermFactor(ExpressionParser::TermFactorContext *ctx) override;
    any visitParen(ExpressionParser::ParenContext *ctx) override;
    any visitInt(ExpressionParser::IntContext *ctx) override;
};
