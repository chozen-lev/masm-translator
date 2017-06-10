#include "Sentence.h"
#include <iostream>

Sentence::Sentence(unsigned int number, std::string line): m_Label(nullptr), m_Mnem(nullptr)
{
    m_lineNum = number;
    m_OriginalCode = line;
    m_Line = line;
}

std::vector<Token*> Sentence::getTokens() const {
    return m_Tokens;
}

std::string Sentence::getError() const {
    return m_Error;
}