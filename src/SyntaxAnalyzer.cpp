#include "SyntaxAnalyzer.h"
#include <iostream>

void SyntaxAnalyzer::analyzeLine(const std::string &line, const std::vector<Token> &tokens)
{
    m_Code.push_back(line);
    m_Tokens.push_back(tokens);

    m_Sentences.push_back(Sentence(tokens));
}