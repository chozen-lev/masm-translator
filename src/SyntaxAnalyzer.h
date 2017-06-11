#pragma once

#include "types.h"
#include "Sentence.h"
#include <vector>

class SyntaxAnalyzer
{
private:
    std::vector<Sentence> m_Sentences;

    // tables
    std::vector<Label*> m_Labels;
    std::vector<Label*> m_Segments;
    
public:
    SyntaxAnalyzer() {};
    ~SyntaxAnalyzer() {};

    void analyzeStruct(Sentence *);
};