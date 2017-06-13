#pragma once

#include <iostream>

#include "Sentence.h"

class LexicalAnalyzer
{
public:
    LexicalAnalyzer() {};
    ~LexicalAnalyzer() {};

    void analyzeLine(Sentence *);
    const std::string typeString(TokenType type) const;
};