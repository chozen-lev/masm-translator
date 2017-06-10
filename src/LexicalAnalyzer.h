#pragma once

#include "types.h"
#include <vector>

class LexicalAnalyzer
{
private:
    
public:
    LexicalAnalyzer() {};
    ~LexicalAnalyzer() {};

    void analyzeLine(Sentence *);
    const std::string typeString(TokenType type) const;
};

