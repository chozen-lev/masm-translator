#pragma once

#include "Sentence.h"
#include <map>
#include <stack>

class GrammarAnalyzer
{
private:
    const std::map<std::string, int>m_Reg8 = {{ "AL", 0 }, { "CL", 1 }, { "DL", 2 }, { "BL", 3 }, { "AH", 4 }, { "CH", 5 }, { "DH", 6 }, { "BH", 7 }};
    const std::map<std::string, int>m_Reg32 = {{ "EAX", 0 }, { "ECX", 1 }, { "EDX", 2 }, { "EBX", 3 }, { "ESP", 4 }, { "EBP", 5 }, { "ESI", 6 }, { "EDI", 7 }};

    const std::vector<std::string>m_SegMnem = { "CLC", "DEC", "ADD", "IDIV", "CMP", "AND", "MOV", "XOR", "JAE", "DB", "DW", "DD" };
    const std::map<std::string, int>m_SegPrefs = {{ "ES", 0x26 }, { "CS", 0x2e }, { "SS", 0x36 }, { "DS", 0x3e }, { "FS", 0x64 }, { "GS", 0x65 }, { "ESI", 6 }, { "EDI", 7 }};

    Label* findLabelByName(std::vector<Label*>, std::string);
    bool deleteLabelByName(std::vector<Label*> &, std::string);
    Label* findLabelByNameInStack(std::stack<Label*>, std::string);
public:
    GrammarAnalyzer() {};
    ~GrammarAnalyzer() {};

    void analyzeStruct(Sentence *, std::stack<Label*> &, std::vector<Label*> &, std::vector<Label*> &);
    void analyzeOffsets(Sentence *, std::stack<Label*> &, std::vector<Label*> &, std::vector<Label*> &);
};