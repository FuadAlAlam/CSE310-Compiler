#pragma once

#include "ExpressionBaseVisitor.h"

class EvalVisitor : public ExpressionBaseVisitor {
public:
    std::any visitStart(ExpressionParser::StartContext *ctx) override;
    std::any visitAdd(ExpressionParser::AddContext *ctx) override;
    std::any visitExprTerm(ExpressionParser::ExprTermContext *ctx) override;
    std::any visitMul(ExpressionParser::MulContext *ctx) override;
    std::any visitTermFactor(ExpressionParser::TermFactorContext *ctx) override;
    std::any visitParen(ExpressionParser::ParenContext *ctx) override;
    std::any visitInt(ExpressionParser::IntContext *ctx) override;
};
