#pragma once

#include "types.h"
#include "Operand.h"
#include <vector>

class Sentence
{
private:
    std::vector<Token> m_Tokens;

    Label m_Label;
    Token m_Command;
    std::vector<Operand> m_Operands;

    unsigned int m_Offset;
    unsigned int m_SentenceSize;

    std::string m_ModRm;
    std::string m_Sib;
    std::string m_Displacepment;
    std::string m_Imm;

    void divideSentence();
    void generateAttributes();

public:
    Sentence(const std::vector<Token> &);
    ~Sentence() {};

    void getSize() {};
};  