#pragma once

#include "types.h"
#include <vector>

class Operand
{
private:
    std::vector<Token> m_Tokens;
    OpType m_OpType;
    Token m_Label;
    std::vector<Token> m_Address;

public:
    Operand(std::vector<Token> &);
    ~Operand() {};

    std::vector<Token>& getTokens();
};