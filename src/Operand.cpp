#include "Operand.h"

Operand::Operand(std::vector<Token> &tokens)
{
    m_Tokens = tokens;

    if (tokens.size() == 1)
    {
        if (tokens.front().type == TokenType::REG32) {
            m_OpType = OpType::REG32;
        }
        else if (tokens.front().type == TokenType::REG8) {
            m_OpType = OpType::REG8;
        }
        else if (tokens.front().type == TokenType::CONST_BIN || tokens.front().type == TokenType::CONST_DEC || tokens.front().type == TokenType::CONST_HEX || tokens.front().type == TokenType::CONST_TEXT) {
            m_OpType = OpType::IMM;
        }
    }
    // else if (tokens.size() > 1)
    // {
    //     std::vector<Token>::iterator token = tokens.begin();

    //     while (token != tokens.end() && token->name != "[")
    //     {

    //         token++;
    //     }
    // }
}

std::vector<Token>& Operand::getTokens()
{
    return m_Tokens;
}