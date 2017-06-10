#pragma once

#include <string>

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

enum class AnalyzeType
{
    NONE,
    WARNING,
    ERROR
};

enum class LabelType
{
    NONE,
    NUMBER,
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
    unsigned int number;
    Token(): type(TokenType::NONE), number(0) {};
    Token(std::string _name, TokenType _type, unsigned int _number): name(_name), type(_type), number(_number) {};
};

struct Segment
{
    Token *token;
    unsigned int length;

    Segment(): token(nullptr), length(0) {}
};

struct Label
{
    Token *token;
    LabelType type;
    unsigned int value;
    Segment *segment;

    Label(Token *_token): token(_token), type(LabelType::NONE), value(0), segment(nullptr) {}
    Label(): token(nullptr), type(LabelType::NONE), value(0), segment(nullptr) {}
};

typedef unsigned char byte;