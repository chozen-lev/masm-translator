#pragma once

#include <string>

enum class TokenType
{
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

enum class OpType
{
    REG8,
    REG32,
    IMM,
    MEM
};

struct Token
{
    std::string name;
    TokenType type;
};

struct Segment
{
    std::string name;
    unsigned int length;
};

struct Label
{
    Token token;
    unsigned int value;
    std::string segName;
};