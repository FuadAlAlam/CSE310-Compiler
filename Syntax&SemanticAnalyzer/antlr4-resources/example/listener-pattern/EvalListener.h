#pragma once

#include <unordered_map>
#include "ExpressionBaseListener.h"

using namespace antlr4;

class EvalListener : public ExpressionBaseListener {
public:
    int result() const { return finalResult; }

    void exitAdd(ExpressionParser::AddContext *ctx) override;
    void exitExprTerm(ExpressionParser::ExprTermContext *ctx) override;
    void exitMul(ExpressionParser::MulContext *ctx) override;
    void exitTermFactor(ExpressionParser::TermFactorContext *ctx) override;
    void exitParen(ExpressionParser::ParenContext *ctx) override;
    void exitInt(ExpressionParser::IntContext *ctx) override;
    void exitStart(ExpressionParser::StartContext *ctx) override;

private:
    // Listener callbacks return void, so per-node results are stashed here,
    // keyed by the context they were computed for, and picked back up when
    // the parent context exits.
    std::unordered_map<ParserRuleContext *, int> values;
    int finalResult = 0;
};
