#pragma once

#include "types.h"
#include "Operand.h"
#include <vector>

class Sentence
{
    friend class Operand;
    friend class LexicalAnalyzer;
    friend class SyntaxAnalyzer;

private:
    std::string m_OriginalCode;
    std::string m_Line;
    std::vector<Token*> m_Tokens;

    unsigned int m_lineNum;

    Label *m_Label;
    Token *m_Mnem;
    std::vector<Operand*> m_Operands;

    std::string m_Warning;
    std::string m_Error;

public:
    Sentence(unsigned int, std::string);
    ~Sentence() {};

    std::vector<Token*> getTokens() const;
    std::string getError() const;
};