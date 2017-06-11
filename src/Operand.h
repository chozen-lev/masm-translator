#pragma once

#include <iostream>
#include "types.h"
#include "ExprEvaluate.hpp"
#include <vector>
#include <iomanip>

class Sentence;

class Operand
{
    friend class Sentence;
    friend class SyntaxAnalyzer;
    friend class GrammarAnalyzer;

private:
    struct OpConst
    {
        std::vector<Token*>::iterator first;
        unsigned int numTokens;
        byte *bytes;
        unsigned int bytesNum;
        OpConst(std::vector<Token*>::iterator it, unsigned int number): first(it), numTokens(number), bytes(nullptr), bytesNum(0) { evaluateExpr(); };
        OpConst(): first(nullptr), numTokens(0), bytes(nullptr), bytesNum(0) {};
        bool evaluateExpr()  // TODO: вынести реализацию в cpp
        {
            ExprEvaluate expr;
            bool error = false;
            int value;

            if ((*first)->type == TokenType::CONST_TEXT)
            {
                bytesNum = (*first)->name.size();
                bytes = new byte[bytesNum];
                std::copy((*first)->name.begin(), (*first)->name.end(), bytes);
            }
            else
            {
                value = expr.evaluate(((*first)->name == "+" || (*first)->name == "-") ? first + 1 : first, first + numTokens, error);;

                if (error) {
                    return false;
                }

                if (value < 0x10000) {
                    if (value < 0x100) bytesNum = 1;// 8 bit
                    else bytesNum = 2; // 16 bit
                } else {
                    if (value < 0x100000000L) bytesNum = 4; // 32 bit
                    else  bytesNum = 8;// 64 bit
                }
                bytes = new byte[bytesNum];
                if ((*first)->name == "-") {
                    value = -value;
                }

                for (int i = 0; i < bytesNum; i++) {
                    bytes[bytesNum - i - 1] = (value >> (i * 8));
                }
            }

            // if ((*first)->type == TokenType::CONST_TEXT)
            //     std::cout << "\"" << (*first)->name << "\" = ";
            // else std::cout << std::dec << value << " = ";

            // for (int i = 0; i < bytesNum; i++) {
            //     std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)bytes[i] << " ";
            // }

            // std::cout << std::endl;
            // std::cout << std::endl;
            // // std::cout << std::endl;

            return true;
        }
    };

    unsigned int m_iFirstLex;
    unsigned int m_LexNum;

    Token *m_Register;
    Token *m_SizePtr;
    Token *m_SegPrefix;
    Label *m_Label;
    Token *m_AddrReg1;
    Token *m_AddrReg2;
    OpConst *m_Const;

public:
    Operand(unsigned int);
    ~Operand() {};

    void analyzeAttributes(Sentence *);
};