#pragma once

#include "Sentence.h"
#include <map>
#include <stack>

class GrammarAnalyzer
{
private:
    const std::map<std::string, int>m_Reg8 = {{ "AL", 0 }, { "CL", 1 }, { "DL", 2 }, { "BL", 3 }, { "AH", 4 }, { "CH", 5 }, { "DH", 6 }, { "BH", 7 }};
    const std::map<std::string, int>m_Reg32 = {{ "EAX", 0 }, { "ECX", 1 }, { "EDX", 2 }, { "EBX", 3 }, { "ESP", 4 }, { "EBP", 5 }, { "ESI", 6 }, { "EDI", 7 }};

    const std::vector<std::string>m_SegMnen = { "CLC", "DEC", "ADD", "IDIV", "CMP", "AND", "MOV", "XOR", "JAE", "DB", "DW", "DD" };

    Label* findLabelByName(std::vector<Label*>, std::string);
    bool deleteLabelByName(std::vector<Label*> &, std::string);
    Label* findLabelByNameInStack(std::stack<Label*>, std::string);
public:
    GrammarAnalyzer() {};
    ~GrammarAnalyzer() {};

    void analyzeStruct(Sentence *, std::stack<Label*> &, std::vector<Label*> &, std::vector<Label*> &);
};