#include "Sentence.h"

Sentence::Sentence(const std::vector<Token> &tokens)
{
    m_Tokens = tokens;

    divideSentence();
}

void Sentence::divideSentence()
{
    std::vector<Token>::iterator token = m_Tokens.begin();

    if (token == m_Tokens.end())
        return;

    if (token->type == TokenType::IDENTIFIER)
    {
        m_Label.token = *token;

        token++;

        if (token != m_Tokens.end() && token->type == TokenType::SINGLE_CHAR) {
            token++;
        }
    }

    if (token != m_Tokens.end() && (token->type == TokenType::DIRECTIVE || token->type == TokenType::COMMAND))
    {
        m_Command = *token;
        token++;
    }


    while (token != m_Tokens.end())
    {
        std::vector<Token> buff;

        do
        {
            buff.clear();
            buff.push_back(*token);
            token++;

            if (token != m_Tokens.end() && token->name == ",")
            {
                token++;
                break;
            }
        } while (token != m_Tokens.end());

        m_Operands.push_back(Operand(buff));
    }
}

void Sentence::generateAttributes()
{

}