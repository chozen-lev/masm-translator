#include <regex>

#include "LexicalAnalyzer.h"

void LexicalAnalyzer::analyzeLine(std::string &line)
{
    parseLine(line);
    analyzeLexems(m_Lexems.back());
}

std::regex regex_lexems("(\"[^\"]*\"|'[^']*'|[\\w]+|\\d+|[,:\\(\\)\\[\\]\\*+-;])");

void LexicalAnalyzer::parseLine(std::string &line)
{
    size_t lastIndex;
    char lastQuote = 0; 

    for (lastIndex = 0; lastIndex < line.length(); ++lastIndex) {
        if (lastQuote)
        {
            if (line[lastIndex] == lastQuote) {
                lastQuote = 0;
            }
            continue;
        }
        else if (line[lastIndex] == ';') {
            break;
        }
        else if (line[lastIndex] == '"' || line[lastIndex] == '\'')
        {
            lastQuote = line[lastIndex];
            continue;
        }

        line[lastIndex] = toupper(line[lastIndex]);
    }

    std::smatch result;
    std::string::const_iterator searchStart(line.cbegin());

    m_Lexems.push_back(std::vector<std::string>());

    while (regex_search(searchStart, line.cbegin() + lastIndex, result, regex_lexems))
    {
        searchStart += result.position() + result.length();
        m_Lexems.back().push_back(result[0]);
    }

    m_Code.push_back(line);
}

std::regex regex_string("(\"[^\"]*\"|'[^]*')");
std::regex regex_binary("([01]+B)");
std::regex regex_hex("([0-9]{1}[A-F0-9]*H)");
std::regex regex_dec("([0-9]+D?)");
std::regex regex_reg8("(AL|CL|DL|BL|AH|CH|DH|BH)");
std::regex regex_reg32("(EAX|ECX|EDX|EBX|ESP|EBP|ESI|EDI)");
std::regex regex_command("(CLC|DEC|ADD|IDIV|CMP|AND|MOV|XOR|JAE)");
std::regex regex_operator("(PTR)");
std::regex regex_type("(BYTE|WORD|DWORD)");
std::regex regex_segment("(CS|DS|SS|ES|FS|GS)");
std::regex regex_directive("(END|SEGMENT|ENDS|EQU|DB|DW|DD)");
std::regex regex_singlechar("([,:()\\[\\]\\*+-]{1})");
std::regex regex_idendifier("([A-Z_@?$]{1}[A-Z0-9_@?$]*)");

void LexicalAnalyzer::analyzeLexems(std::vector<std::string> &lexems)
{
    m_Tokens.push_back(std::vector<Token>());
    std::vector<std::vector<Token>>::iterator vecToken = m_Tokens.end() - 1;

    for (std::vector<std::string>::iterator lex = lexems.begin(); lex != lexems.end(); ++lex)
    {
        vecToken->push_back(Token());

        if (std::regex_match(lex->c_str(), regex_string))
        {
            vecToken->back().type = TokenType::CONST_TEXT;
            *lex = lex->substr(1, lex->size() - 2);
        } else if (std::regex_match(lex->c_str(), regex_binary)) {
            vecToken->back().type = TokenType::CONST_BIN;
        } else if (std::regex_match(lex->c_str(), regex_hex)) {
            vecToken->back().type = TokenType::CONST_HEX;
        } else if (std::regex_match(lex->c_str(), regex_dec)) {
            vecToken->back().type = TokenType::CONST_DEC;
        } else if (std::regex_match(lex->c_str(), regex_reg8)) {
            vecToken->back().type = TokenType::REG8;
        } else if (std::regex_match(lex->c_str(), regex_reg32)) {
            vecToken->back().type = TokenType::REG32;
        } else if (std::regex_match(lex->c_str(), regex_command)) {
            vecToken->back().type = TokenType::COMMAND;
        } else if (std::regex_match(lex->c_str(), regex_operator)) {
            vecToken->back().type = TokenType::OPERATOR;
        } else if (std::regex_match(lex->c_str(), regex_type)) {
            vecToken->back().type = TokenType::TYPE;
        } else if (std::regex_match(lex->c_str(), regex_segment)) {
            vecToken->back().type = TokenType::SEGMENT;
        } else if (std::regex_match(lex->c_str(), regex_directive)) {
            vecToken->back().type = TokenType::DIRECTIVE;
        } else if (std::regex_match(lex->c_str(), regex_singlechar)) {
            vecToken->back().type = TokenType::SINGLE_CHAR;
        } else if (lex->length() <= 7 && std::regex_match(lex->c_str(), regex_idendifier)) {
            vecToken->back().type = TokenType::IDENTIFIER;
        } else {
            vecToken->back().type = TokenType::WRONG_LEXEM;
        }

        vecToken->back().name = *lex;
    }
}

const std::vector<std::string>& LexicalAnalyzer::getCodes() {
    return m_Code;
}

const std::vector<std::vector<std::string>>& LexicalAnalyzer::getLexems() {
    return m_Lexems;
}

const std::vector<std::vector<Token>>& LexicalAnalyzer::getTokens() {
    return m_Tokens;
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
            return "односимвольна";
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