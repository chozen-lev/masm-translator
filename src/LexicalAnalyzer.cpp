#include <regex>
#include <iostream>

#include "Sentence.h"
#include "LexicalAnalyzer.h"

std::regex regex_lexems("(\"[^\"]*\"|'[^']*'|[\\w]+|\\d+|[,:\\(\\)\\[\\]\\*+-;])");

std::regex regex_string("(\"[^\"]*\"|'[^']*')");
std::regex regex_binary("([01]+B)");
std::regex regex_hex("([0-9]{1}[A-F0-9]*H)");
std::regex regex_dec("([0-9]+D?)");
std::regex regex_reg8("(AL|CL|DL|BL|AH|CH|DH|BH)");
std::regex regex_reg32("(EAX|ECX|EDX|EBX|ESP|EBP|ESI|EDI)");
std::regex regex_command("(CLC|DEC|ADD|IDIV|CMP|AND|MOV|XOR|JAE)");
std::regex regex_operator("(PTR)");
std::regex regex_type("(BYTE|WORD|DWORD|NEAR|FAR)");
std::regex regex_segment("(CS|DS|SS|ES|FS|GS)");
std::regex regex_directive("(END|SEGMENT|ENDS|EQU|DB|DW|DD)");
std::regex regex_singlechar("([,:()\\[\\]\\*+-]{1})");
std::regex regex_idendifier("([A-Z_@?$]{1}[A-Z0-9_@?$]{0,6})");

void LexicalAnalyzer::analyzeLine(Sentence *sentence)
{
    if (sentence->m_OriginalCode.empty()) {
        return;
    }

    unsigned int lastIndex;
    char lastQuote = 0;

    for (lastIndex = 0; lastIndex < sentence->m_Line.length(); ++lastIndex)
    {
        if (lastQuote)
        {
            if (sentence->m_Line[lastIndex] == lastQuote) {
                lastQuote = 0;
            }
            continue;
        }
        else if (sentence->m_Line[lastIndex] == ';') {
            break;
        }
        else if (sentence->m_Line[lastIndex] == '"' || sentence->m_Line[lastIndex] == '\'')
        {
            lastQuote = sentence->m_Line[lastIndex];
            continue;
        }

        sentence->m_Line[lastIndex] = toupper(sentence->m_Line[lastIndex]);
    }

    std::smatch result;
    std::string::const_iterator searchStart(sentence->m_Line.cbegin());
    Token *token;
    unsigned int number = 0;

    // std::cout << sentence->m_lineNum << ") " << "<" << sentence->m_OriginalCode << ">" << std::endl;

    while (regex_search(searchStart, sentence->m_Line.cbegin() + lastIndex, result, regex_lexems))
    {
        searchStart += result.position() + result.length();
        token = new Token;

        token->name = result[0];

        if (std::regex_match(token->name.c_str(), regex_string))
        {
            token->type = TokenType::CONST_TEXT;
            token->name = token->name.substr(1, token->name.size() - 2);
        } else if (std::regex_match(token->name.c_str(), regex_binary)) {
            token->type = TokenType::CONST_BIN;
        } else if (std::regex_match(token->name.c_str(), regex_hex)) {
            token->type = TokenType::CONST_HEX;
        } else if (std::regex_match(token->name.c_str(), regex_dec)) {
            token->type = TokenType::CONST_DEC;
        } else if (std::regex_match(token->name.c_str(), regex_reg8)) {
            token->type = TokenType::REG8;
        } else if (std::regex_match(token->name.c_str(), regex_reg32)) {
            token->type = TokenType::REG32;
        } else if (std::regex_match(token->name.c_str(), regex_command)) {
            token->type = TokenType::COMMAND;
        } else if (std::regex_match(token->name.c_str(), regex_operator)) {
            token->type = TokenType::OPERATOR;
        } else if (std::regex_match(token->name.c_str(), regex_type)) {
            token->type = TokenType::TYPE;
        } else if (std::regex_match(token->name.c_str(), regex_segment)) {
            token->type = TokenType::SEGMENT;
        } else if (std::regex_match(token->name.c_str(), regex_directive)) {
            token->type = TokenType::DIRECTIVE;
        } else if (std::regex_match(token->name.c_str(), regex_singlechar)) {
            token->type = TokenType::SINGLE_CHAR;
        } else if (std::regex_match(token->name.c_str(), regex_idendifier)) {
            token->type = TokenType::IDENTIFIER;
        } else {
            token->type = TokenType::WRONG_LEXEM;
        }

        token->number = ++number;

        sentence->m_Tokens.push_back(token);

        // std::cout << token->number << ". " << token->name << " : " << typeString(token->type) << std::endl;
    }

    // std::cout << std::endl;
}

const std::string LexicalAnalyzer::typeString(TokenType type) const
{
    switch (type)
    {
        case TokenType::IDENTIFIER: {
            return "Ідентифікатор користувача або невизначений";
        }
        case TokenType::COMMAND: {
            return "Ідентифікатор мнемокоду машинної інструкції";
        }
        case TokenType::TYPE: {
            return "Ідентифікатор типу";
        }
        case TokenType::OPERATOR: {
            return "Ідентифікатор оператора визначення типу";
        }
        case TokenType::CONST_TEXT: {
            return "Текстова константа";
        }
        case TokenType::CONST_BIN: {
            return "Двійкова константа";
        }
        case TokenType::CONST_DEC: {
            return "Десяткова константа";
        }
        case TokenType::CONST_HEX: {
            return "Шістнадцяткова константа";
        }
        case TokenType::SINGLE_CHAR: {
            return "Односимвольна константа";
        }
        case TokenType::DIRECTIVE: {
            return "Ідентифікатор директиви";
        }
        case TokenType::SEGMENT: {
            return "Ідентифікатор сегментного регістра";
        }
        case TokenType::REG32: {
            return "Ідентифікатор 32-розрядного регістра даних";
        }
        case TokenType::REG8: {
            return "Ідентифікатор 8-розрядного регістра даних";
        }
        case TokenType::WRONG_LEXEM: {
            return "Помилкова лексема";
        }
    }

    return "None";
}