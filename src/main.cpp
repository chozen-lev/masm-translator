#include <iostream>
#include <fstream>

#include "Sentence.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "GrammarAnalyzer.h"
#include "types.h"

#define VERSION "17.6.11"

int main(int argc, char* argv[])
{
    std::string path_source, path_listing;

    if (argc > 1)
    {
        for (unsigned short i = 3; i < argc; i++) {
            std::cout << "Extra file name ignored" << std::endl;
        }

        if (argc >= 2) {
            path_source = *(argv + 1);
        }

        if (argc >= 3) {
            path_listing = *(argv + 2);
        }
    }

    path_source = "test.asm";
    path_listing = "test.lst";

    std::cout << "Translator version " << VERSION << std::endl;
    // std::cout << "Powered by Lev Bazylskyi KV-51 2017" << std::endl;

    while (path_source.empty())
    {
        std::cout << std::endl << "Source filename [.asm]: ";
        getline(std::cin, path_source);

        if (path_source.empty()) {
            std::cout << "Line invalid, start again" << std::endl;
        }
    }

    if (path_source.rfind(".asm") == std::string::npos) {
        path_source += ".asm";
    }

    if (path_listing.empty())
    {
        std::cout << "Source listing  [" + path_source.substr(0, path_source.length() - 4) + ".lst]: ";
        getline(std::cin, path_listing);

        if (path_listing.empty()) {
            path_listing = path_source.substr(0, path_source.length() - 4);
        }
    }

    if (path_listing.rfind(".lst") == std::string::npos) {
        path_listing += ".lst";
    }

    std::ifstream FileSource(path_source);

    if (FileSource.is_open() == false) {
        std::cout << "Unable to open input file: " + path_source << std::endl;
    }
    else
    {
        std::ofstream FileListing(path_listing);
        size_t lineNum = 0;
        std::string line;

        // analyzers
        LexicalAnalyzer LexAnalyzer;
        SyntaxAnalyzer SynAnalyzer;
        GrammarAnalyzer GramAnalyzer;

        // sentences with all attributes
        std::vector<Sentence*> sentences;

        // tables
        std::vector<Label*> labels;
        std::vector<Label*> segments;

        Label *activeSeg = nullptr;// maybe stack of segments ?

        while (getline(FileSource, line))
        {
            lineNum++;

            sentences.push_back(new Sentence(lineNum, line));

            LexAnalyzer.analyzeLine(sentences.back());
            SynAnalyzer.analyzeStruct(sentences.back());
            GramAnalyzer.analyzeStruct(sentences.back(), activeSeg, labels, segments);

            if (!sentences.back()->getError().empty()) {
                std::cout << sentences.back()->getError() << std::endl;
            }

            if (!sentences.back()->getTokens().empty() && sentences.back()->getTokens().front()->name.compare("END") == 0) {
                break;
            }
        }

        FileSource.close();
        FileListing.close();
    }

    std::cout << std::endl;

    return 0;
}