#pragma once

#include <iostream>
#include <vector>

typedef unsigned char byte;

enum class TokenType
{
    NONE,
    IDENTIFIER,
    COMMAND,
    TYPE,
    OPERATOR,
    CONST_TEXT,
    CONST_BIN,
    CONST_DEC,
    CONST_HEX,
    SINGLE_CHAR,
    DIRECTIVE,
    SEGMENT,
    REG32,
    REG8,
    WRONG_LEXEM
};

enum class LabelType
{
    NONE,
    NUMBER,
    TEXT,
    LBYTE,
    LWORD,
    LDWORD,
    LNEAR,
    LFAR,
    SEGMENT
};

struct Token
{
    std::string name;
    TokenType type;
    int number;
    Token(): type(TokenType::NONE), number(0) {};
    Token(std::string _name, TokenType _type, int _number): name(_name), type(_type), number(_number) {};
};

struct Label
{
    Token *token;
    LabelType type;
    byte *value;
    int bytesNum;
    Label *segment;

    Label(Token *_token): token(_token), type(LabelType::NONE), value(nullptr), bytesNum(0), segment(nullptr) {}
    Label(): token(nullptr), type(LabelType::NONE), value(nullptr), bytesNum(0), segment(nullptr) {}
};

class Operand;

class Sentence
{
friend class Operand;
friend class LexicalAnalyzer;
friend class SyntaxAnalyzer;
friend class GrammarAnalyzer;
friend class GenerateListing;

private:
    std::string m_Line;
    std::vector<Token*> m_Tokens;

    int m_lineNum;

    Label *m_Label;
    Token *m_Mnem;
    std::vector<Operand*> m_Operands;

    byte* m_bytePtr;    // 1 byte
    byte* m_bytePrefix; // 1 byte
    byte* m_byteCmd;    // 1 byte
    byte* m_byteModRm;  // 1 byte
    byte* m_byteSib;    // 1 byte
    byte* m_byteDisp;   // 1 or 4 bytes
    byte* m_byteImm;    // 1,2 or 4 bytes

    int m_BytesNum;
    int m_DispBytesNum; // there isn't another way to determinate allocated mem=(

    std::string m_Warning;
    std::string m_Error;

public:
    Sentence(int, std::string);
    ~Sentence() {};
    std::string m_OriginalCode;

    std::string getError() const;
    std::string getWarning() const;
    std::vector<Token*> getTokens() const { return m_Tokens; }
    int getLineNum() const { return m_lineNum; }
    int getBytesNum() const;
};