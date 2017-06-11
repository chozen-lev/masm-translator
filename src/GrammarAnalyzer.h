#pragma once

#include "types.h"
#include <vector>

class Sentence;

class GrammarAnalyzer
{
private:
    
public:
    GrammarAnalyzer() {};
    ~GrammarAnalyzer() {};

    void analyzeStruct(Sentence *, Label* &, std::vector<Label*> &, std::vector<Label*> &);
};