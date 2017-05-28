#pragma once

#include "types.h"
#include "Sentence.h"
#include <vector>

class SyntaxAnalyzer
{
private:
    std::vector<std::string> m_Code;
    std::vector<std::vector<Token>> m_Tokens;
    std::vector<Sentence> m_Sentences;

    std::vector<Label> m_Labels;
    std::vector<Segment> m_Segments;

    Segment m_CurrSeg;
    unsigned int m_CurrOffset;
    
public:
    SyntaxAnalyzer() {};
    ~SyntaxAnalyzer() {};

    void analyzeLine(const std::string &, const std::vector<Token> &);
};