#pragma once

#include "Sentence.h"

#include <stack>

struct OpConst
{
    std::vector<Token*>::iterator first;
    int tokensNum;

    byte *bytes;
    int bytesNum;

    OpConst(): first(nullptr), tokensNum(0), bytes(nullptr), bytesNum(0) {};
    OpConst(std::vector<Token*>::iterator, int);
    bool evaluateExpr();

    int evaluateOp(std::stack<std::string> &, std::stack<int> &, bool &);
    int evaluate(std::vector<Token*>::iterator, std::vector<Token*>::iterator, bool &);
};

class Operand
{
// friend class Sentence;
friend class SyntaxAnalyzer;
friend class GrammarAnalyzer;
friend class GenerateListing;
public:
    enum class OpSize
    {
        NONE,
        BYTE,
        WORD,
        DWORD
    };

    Operand(std::vector<Token*>::iterator);
    ~Operand() {};

    void analyzeAttributes(Sentence *);

private:
    std::vector<Token*>::iterator m_FirstToken;
    int m_TokensNum;

    Token *m_Register;
    Token *m_SizePtr;
    Token *m_SegPrefix;
    Label *m_Label;
    Token *m_AddrReg1;
    Token *m_AddrReg2;
    OpConst *m_Const;

    OpSize m_OperandSize;
};