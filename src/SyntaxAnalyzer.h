#pragma once

class Sentence;

class SyntaxAnalyzer
{
public:
    SyntaxAnalyzer() {};
    ~SyntaxAnalyzer() {};

    void analyzeStruct(Sentence *);
};