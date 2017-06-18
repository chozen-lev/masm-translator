#include <iostream>
#include <fstream>

#include "Sentence.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "GrammarAnalyzer.h"
#include "GenerateListing.h"

#define VERSION "17.6.18"

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

    // path_source = "test.asm";
    // path_listing = "test.lst";

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
        GenerateListing GenerListing;

        // sentences with all attributes
        std::vector<Sentence*> sentences;

        // tables
        std::vector<Label*> labels;
        std::vector<Label*> segments;

        std::stack<Label*> activeSegs;
        std::stack<Label*> openedSegs;

        bool end = false;

        while (getline(FileSource, line))
        {
            lineNum++;

            sentences.push_back(new Sentence(lineNum, line));

            LexAnalyzer.analyzeLine(sentences.back());
            SynAnalyzer.analyzeStruct(sentences.back());
            GramAnalyzer.analyzeStruct(sentences.back(), activeSegs, labels, segments);

            if (!sentences.back()->getTokens().empty() && sentences.back()->getTokens().front()->name.compare("END") == 0)
            {
                end = true;
                break;
            }
        }

        while(!activeSegs.empty())
        {
            openedSegs.push(activeSegs.top());
            activeSegs.pop();
        }

        std::vector<Sentence*>::iterator sentence = sentences.begin();

        while (sentence != sentences.end())
        {
            GramAnalyzer.analyzeOffsets(*sentence, activeSegs, labels, segments);

            sentence++;
        }

        while(!activeSegs.empty()) {
            activeSegs.pop();
        }

        for (std::vector<Label*>::iterator seg = segments.begin(); seg != segments.end(); seg++)
        {
            *((int*)(*seg)->value) = 0;
        }

        sentence = sentences.begin();

        int err_num = 0;
        int warn_num = 0;

        std::cout << std::endl;

        while (sentence != sentences.end())
        {
            GramAnalyzer.analyzeJAE(*sentence, activeSegs, labels, segments);
            GenerListing.printLine(FileListing, *sentence, activeSegs, path_source);

            if (!(*sentence)->getError().empty())
            {
                std::cout << path_source << "(" << std::to_string((*sentence)->getLineNum()) << "): "  << (*sentence)->getError() << std::endl;
                err_num++;
            }
            else if (!(*sentence)->getWarning().empty())
            {
                std::cout << path_source << "(" << std::to_string((*sentence)->getLineNum()) << "): "  << (*sentence)->getWarning() << std::endl;
                warn_num++;
            }

            sentence++;
        }

        FileListing << std::endl;

        if (!end)
        {
            FileListing << path_source << "(" << std::to_string(lineNum + 1) << "): warning: End of file, no END directive" << std::endl;
            std::cout << path_source << "(" << std::to_string(lineNum + 1) << "): warning: End of file, no END directive" << std::endl;
            warn_num++;
        }

        while(!openedSegs.empty())
        {
            FileListing << "Open segments: " << openedSegs.top()->token->name << std::endl;
            std::cout << "Open segments: " << openedSegs.top()->token->name << std::endl;
            err_num++;
            openedSegs.pop();
        }

        FileListing << std::endl << std::endl;

        GenerListing.printSegments(FileListing, segments);
        FileListing << std::endl << std::endl;
        GenerListing.printLabels(FileListing, labels);

        FileListing << std::endl << std::endl;

        std::cout << std::endl;

        std::cout << "      " << warn_num << " Warning Errors" << std::endl;
        std::cout << "      " << err_num << " Severe  Errors";

        FileListing << "      " << warn_num << " Warning Errors" << std::endl;
        FileListing << "      " << err_num << " Severe  Errors";

        FileSource.close();
        FileListing.close();
    }

    std::cout << std::endl;

    return 0;
}