#pragma once

#include <fstream>
#include <vector>
#include <stack>
#include "Sentence.h"

class GenerateListing
{
private:
    
public:
    GenerateListing() {};
    ~GenerateListing() {};

    bool printLine(std::ofstream &, Sentence *, std::stack<Label*>, std::string);
    bool printSegments(std::ofstream &, std::vector<Label*>);
    bool printLabels(std::ofstream &, std::vector<Label*>);
};