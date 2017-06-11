#include "GrammarAnalyzer.h"
#include "Sentence.h"
#include <algorithm>

void GrammarAnalyzer::analyzeStruct(Sentence *sentence, Label* &activeSeg, std::vector<Label*> &labels, std::vector<Label*> &segments)
{
    if (sentence->m_Tokens.empty() || !sentence->m_Error.empty()) {
        return;
    }

    if (activeSeg == nullptr && sentence->m_Mnem != nullptr && sentence->m_Mnem->name != "EQU" && sentence->m_Mnem->name != "END")
    {
        if (sentence->m_Mnem->name == "SEGMENT")
        {
            if (sentence->m_Label == nullptr)
            {
                sentence->m_Error = "8Gramar error on line " + std::to_string(sentence->m_lineNum);
                return;
            }

            // find() нужно проверять по токенам меток, а не по меткам. TODO:исправить
            if (std::find(segments.begin(), segments.end(), sentence->m_Label) != segments.end())
            {
                // activeSeg = sentence->m_Label;
                sentence->m_Error = "7Gramar error on line " + std::to_string(sentence->m_lineNum);
                return;
            }

            sentence->m_Label->type = LabelType::SEGMENT;
            sentence->m_Label->value = new byte[4]; // 4 bytes for 32-bit segment
            activeSeg = sentence->m_Label;
            segments.push_back(sentence->m_Label);
            return;
        }

        // must be in segment
        sentence->m_Error = "6Gramar error on line " + std::to_string(sentence->m_lineNum);
        return;
    }

    // next are in segment(except EQU and END)
    if (sentence->m_Mnem != nullptr && sentence->m_Mnem->name == "ENDS")
    {
        if (sentence->m_Label == nullptr)
        {
            sentence->m_Error = "5Gramar error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        if (std::find(labels.begin(), labels.end(), sentence->m_Label) == segments.end())
        {
            sentence->m_Error = "4Gramar error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        activeSeg = nullptr;

        return;
    }

    if (sentence->m_Mnem != nullptr && sentence->m_Mnem->name == "EQU")
    {
        if (sentence->m_Label == nullptr)
        {
            sentence->m_Error = "3Gramar error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        if (sentence->m_Operands.empty())
        {
            sentence->m_Error = "2Gramar error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        // if (std::find(labels.begin(), labels.end(), sentence->m_Label) != labels.end())
        // {
        //     sentence->m_Error = "Gramar error on line " + std::to_string(sentence->m_lineNum);
        //     return;
        // }

        sentence->m_Label->type = LabelType::NUMBER;
        sentence->m_Label->value = sentence->m_Operands.front()->m_Const->bytes;// bytes must be reversed. TODO
        labels.push_back(sentence->m_Label);
        return;
    }

    if (sentence->m_Label != nullptr)
    {
        if (std::find(labels.begin(), labels.end(), sentence->m_Label) != labels.end())
        {
            sentence->m_Error = "1Gramar error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        // label
        sentence->m_Label->type = LabelType::LNEAR; // there is no label directive in task. DEFAULT: NEAR
        sentence->m_Label->value = new byte[4]; // 4 bytes for 32-bit segment
        // (unsigned int)*sentence->m_Label->value = (unsigned int)*sentence->m_Operands.front()->m_Const->bytes;
        sentence->m_Label->segment = activeSeg;
        labels.push_back(sentence->m_Label);
    }

    if (sentence->m_Mnem == nullptr) {
        return;
    }

    if (sentence->m_Mnem->name == "END")
    {
        if (sentence->m_Operands.size() > 1) {
            sentence->m_Error = "11Gramar error on line " + std::to_string(sentence->m_lineNum);
        }
        else if (sentence->m_Operands.size() == 1 && sentence->m_Operands.front()->m_Label != nullptr)
        {
            // find() нужно проверять по токенам меток, а не по меткам. TODO:исправить
            if (sentence->m_Operands.front()->m_LexNum != 1/* || std::find(labels.begin(), labels.end(), sentence->m_Operands.front()->m_Label) == labels.end()*/) {
                sentence->m_Error = "12Gramar error on line " + std::to_string(sentence->m_lineNum);
            }
        }

        return;
    }

    if (sentence->m_Mnem->name == "DB")
    {

        return;
    }
    
    if (sentence->m_Mnem->name == "DW")
    {

        return;
    }
    
    if (sentence->m_Mnem->name == "DD")
    {

        return;
    }
    
    if (sentence->m_Mnem->name == "CLC")
    {

        return;
    }

    if (sentence->m_Mnem->name == "DEC")
    {

        return;
    }

    if (sentence->m_Mnem->name == "ADD")
    {

        return;
    }

    if (sentence->m_Mnem->name == "IDIV")
    {

        return;
    }

    if (sentence->m_Mnem->name == "CMP")
    {

        return;
    }

    if (sentence->m_Mnem->name == "AND")
    {

        return;
    }

    if (sentence->m_Mnem->name == "MOV")
    {

        return;
    }

    if (sentence->m_Mnem->name == "XOR")
    {

        return;
    }

    if (sentence->m_Mnem->name == "JAE")
    {

        return;
    }
}