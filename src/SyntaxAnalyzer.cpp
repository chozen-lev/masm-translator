#include "SyntaxAnalyzer.h"
#include <iostream>

void SyntaxAnalyzer::analyzeStruct(Sentence *sentence)
{
    if (sentence->m_Tokens.empty() || !sentence->m_Error.empty()) {
        return;
    }

    std::vector<Token*>::iterator token = sentence->m_Tokens.begin();

    if ((*token)->type == TokenType::IDENTIFIER && token + 1 != sentence->m_Tokens.end())
    {
        if((*(token + 1))->name == ":" || (*(token + 1))->type == TokenType::DIRECTIVE)
        {
            sentence->m_Label = new Label(*token);
            token++;
        }
        else
        {
            sentence->m_Error = "Syntax error on line " + std::to_string(sentence->m_lineNum);
            return;
        }
    }

    if ((*token)->type == TokenType::COMMAND || (*token)->type == TokenType::DIRECTIVE)
    {
        sentence->m_Mnem = *token;
        token++;


        while (token != sentence->m_Tokens.end())
        {
            sentence->m_Operands.push_back(new Operand((*token)->number));

            do
            {
                sentence->m_Operands.back()->m_LexNum++;
                token++;

                if (token != sentence->m_Tokens.end() && (*token)->name == ",")
                {
                    token++;
                    break;
                }

            } while (token != sentence->m_Tokens.end());

            sentence->m_Operands.back()->analyzeAttributes(sentence);

            if (!sentence->m_Error.empty()) {
                return;
            }
        }
    }
}