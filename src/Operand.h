#pragma once

#include <iostream>
#include "types.h"
#include "ExprEvaluate.hpp"
#include <vector>

class Sentence;

class Operand
{
    friend class Sentence;
    friend class SyntaxAnalyzer;

private:
    struct OpConst
    {
        std::vector<Token*>::iterator first;
        unsigned int numTokens;
        int value;
        unsigned int bytesNum;
        OpConst(std::vector<Token*>::iterator it, unsigned int number): first(it), numTokens(number), value(0), bytesNum(0) {};
        OpConst(): first(nullptr), numTokens(0), value(0), bytesNum(0) {};
        bool evaluateExpr()  // TODO: вынести реализацию в cpp
        {
            ExprEvaluate expr;
            bool error = false;

            expr.evaluate(((*first)->name == "+" || (*first)->name == "-") ? first + 1 : first, first + numTokens, error);

            if (error) {
                return false;
            }

            if ((*first)->name == "-") {
                value = -value;
            }

            bytesNum = value / 256 + 1;

            return true;
        }
    };

    unsigned int m_iFirstLex;
    unsigned int m_LexNum;

    Token *m_Register;
    Token *m_SizePtr;
    Token *m_SegPrefix;
    Token *m_Label;
    Token *m_AddrReg1;
    Token *m_AddrReg2;
    OpConst *m_Const;

public:
    Operand(unsigned int);
    ~Operand() {};

    void analyzeAttributes(Sentence *);
};