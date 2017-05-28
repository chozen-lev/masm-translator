#pragma once

#include "types.h"
#include <vector>

class LexicalAnalyzer
{
private:
    std::vector<std::string> m_Code;
    std::vector<std::vector<std::string>> m_Lexems;
    std::vector<std::vector<Token>> m_Tokens;

    void parseLine(std::string &);
    void analyzeLexems(std::vector<std::string> &);
    
public:
    LexicalAnalyzer() {};
    ~LexicalAnalyzer() {};

    void analyzeLine(std::string &);
    const std::vector<std::string>& getCodes();
    const std::vector<std::vector<std::string>>& getLexems();
    const std::vector<std::vector<Token>>& getTokens();
    const std::string typeString(TokenType type) const;
};
