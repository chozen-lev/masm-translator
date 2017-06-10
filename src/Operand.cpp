#include "Operand.h"
#include "Sentence.h"
#include <iostream>

Operand::Operand(unsigned int index): m_iFirstLex(index), m_LexNum(0)
{

}

void Operand::analyzeAttributes(Sentence *sentence)
{
    std::vector<Token*>::iterator token = sentence->m_Tokens.begin() + m_iFirstLex - 1;
    std::vector<Token*>::iterator end = sentence->m_Tokens.begin() + m_iFirstLex +  m_LexNum - 1;

    // register
    if ((*token)->type == TokenType::REG8 || (*token)->type == TokenType::REG32)
    {
        if (token + 1 != end) {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
        }

        m_Register = *token;
        return;
    }

    // string
    if ((*token)->type == TokenType::CONST_TEXT)
    {
        m_Const = new OpConst(token, 1);
        return;
    }

    // memory
    if ((*token)->type == TokenType::TYPE)
    {
        if (token + 1 == end)
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        m_SizePtr = *token;
        token += 2;
    }

    if (token != end && (*token)->type == TokenType::SEGMENT)
    {
        if (token + 1 == end || (*(token + 1))->name != ":")
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        m_SegPrefix = *token;
        token += 2;
    }

    if (token != end && (*token)->type == TokenType::IDENTIFIER)
    {
        m_Label = *token;
        token++;
    }

    if (token != end && (*token)->name == "[" && token + 1 != end)
    {
        token++;

        if ((*token)->type != TokenType::REG8 && (*token)->type != TokenType::REG32)
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        m_AddrReg1 = *token;
        token++;

        if (token == end || (*token)->name != "+" || token + 1 == end)
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        token++;

        if ((*token)->type != TokenType::REG8 && (*token)->type != TokenType::REG32)
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        m_AddrReg2 = *token;
        token++;

        if (token == end)
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        if ((*token)->name == "]")
        {
            if (token + 1 != end) {
                sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            }
            
            return;
        }

        m_Const = new OpConst;
        do
        {
            m_Const->numTokens++;

            if (m_Const->numTokens == 1) {
                m_Const->first = token;
            }

            token++;
        } while (token != end && (*token)->name != "]");

        if ((*token)->name != "]" && token + 1 != end)
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }

        if (!m_Const->evaluateExpr())
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }
    }

    if (token != end && ((*token)->name == "+" || (*token)->name == "-" || (*token)->name == "(" ||
                                                          (*token)->type == TokenType::CONST_BIN ||
                                                          (*token)->type == TokenType::CONST_DEC ||
                                                          (*token)->type == TokenType::CONST_HEX))
    {
        m_Const = new OpConst;
        do
        {
            m_Const->numTokens++;

            if (m_Const->numTokens == 1) {
                m_Const->first = token;
            }

            token++;
        } while (token != end);
        
        if (!m_Const->evaluateExpr())
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }
    }
}