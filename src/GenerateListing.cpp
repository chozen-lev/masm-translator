#include "GenerateListing.h"

#include  <iomanip>

bool GenerateListing::printLine(std::ofstream &stream, Sentence *sentence, std::stack<Label*> activeSegs, std::string filename)
{
    if (sentence->m_Mnem != nullptr)
    {
        if (sentence->m_Mnem->name == "SEGMENT" || sentence->m_Mnem->name == "ENDS")
        {
            if (!sentence->getError().empty())
            {
                stream << "\t\t\t\t\t\t\t\t" << sentence->m_OriginalCode << std::endl;
                stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->getError() << std::endl;
                return false;
            }

            stream << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << *((int*)sentence->m_Label->value);
            stream << "\t\t\t\t\t\t\t" << sentence->m_OriginalCode << std::endl;

            if (!sentence->m_Warning.empty()) {
                stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->m_Warning << std::endl;
            }

            return true;;
        }
        else if (sentence->m_Mnem->name == "EQU")
        {
            stream << " = ";

            if (sentence->m_Label->bytesNum == 2) {
                stream << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << ((sentence->m_Label->value[1] << 8) | sentence->m_Label->value[0]) << "\t\t\t\t\t\t\t";
            }
            else if (sentence->m_Label->bytesNum == 4) {
                stream << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << ((sentence->m_Label->value[3] << 24) | (sentence->m_Label->value[2] << 16) | (sentence->m_Label->value[1] << 8) | sentence->m_Label->value[0]) << "\t\t\t\t\t\t";
            }
            else {
                stream << "TEXT" << "\t\t\t\t\t\t\t";
            }
            stream << sentence->m_OriginalCode << std::endl;

            if (!sentence->m_Warning.empty()) {
                stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->m_Warning << std::endl;
            }

            return true;
        }
        else if (sentence->m_Mnem->name == "END")
        {
            stream << "\t\t\t\t\t\t\t\t" << sentence->m_OriginalCode << std::endl;

            if (!sentence->getError().empty())
            {
                stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->getError() << std::endl;
                return false;
            }
            else if (!sentence->m_Warning.empty()) {
                stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->m_Warning << std::endl;
            }

            return true;
        }

        if (!activeSegs.empty()) {
            stream << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << *((int*)activeSegs.top()->value) - sentence->getBytesNum() << " ";
        }
        else {
            stream << "     ";
        }

        if (sentence->m_byteCmd != nullptr)
        {
            if (sentence->m_bytePrefix != nullptr) {
                stream << " " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)*sentence->m_bytePrefix << ":";
            }
            else {
                stream << "    ";
            }

            if (sentence->m_bytePtr != nullptr) {
                stream << " " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)*sentence->m_bytePtr << "|";
            }
            else {
                stream << "    ";
            }

            stream << " " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)*sentence->m_byteCmd;

            if (sentence->m_byteModRm != nullptr) {
                stream << " " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)*sentence->m_byteModRm;
            }
            else {
                stream << "   ";
            }

            if (sentence->m_byteSib != nullptr) {
                stream << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)*sentence->m_byteSib;
            }
            else {
                stream << "  ";
            }
        }
        else if (activeSegs.empty()) {
            stream << "                ";
        }

        if (sentence->m_byteDisp != nullptr)
        {
            if (sentence->m_DispBytesNum == 1)
            {
                stream << " " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << (int)sentence->m_byteDisp[0] << "      ";

                if (sentence->m_byteCmd == nullptr) {
                    stream << "                ";
                }
            }
            else if (sentence->m_DispBytesNum == 2)
            {
                stream << " " << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << *((int*)sentence->m_byteDisp) << "    ";

                if (sentence->m_byteCmd == nullptr) {
                    stream << "                ";
                }
            }
            else if (sentence->m_DispBytesNum == 4)
            {
                stream << " " << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << *((int*)sentence->m_byteDisp);

                if (sentence->m_byteCmd == nullptr) {
                    stream << "                ";
                }
            }
            else { // db text
                // stream << "                         ";
                int i = 0;
                while (i < 8 && i < sentence->m_DispBytesNum)
                {
                    stream << " " << std::setfill('0') << std::setw(2) << std::hex << (int)sentence->m_byteDisp[i];
                    i++;
                }

                stream << "\t" << sentence->m_OriginalCode << std::endl;

                while (i < sentence->m_DispBytesNum)
                {
                    int j = 0;
                    stream << "     ";
                    while (j < 8 && i < sentence->m_DispBytesNum)
                    {
                        stream << " " << std::setfill('0') << std::setw(2) << std::hex << (int)sentence->m_byteDisp[i];
                        i++;
                        j++;
                    }
                    stream << std::endl;
                }


                if (!sentence->m_Warning.empty()) {
                    stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->m_Warning << std::endl;
                }

                return true;
            }
        }
        else {
            stream << "         ";
        }

        stream << "\t" << sentence->m_OriginalCode << std::endl;

        if (!sentence->getError().empty())
        {
            stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->getError() << std::endl;
            return false;
        }
        else if (!sentence->m_Warning.empty()) {
            stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->m_Warning << std::endl;
        }

        return true;
    }
    else if (sentence->m_Label != nullptr && !activeSegs.empty())
    {
        stream << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << *((int*)activeSegs.top()->value) << "\t\t\t\t\t\t\t\t";
        stream << sentence->m_OriginalCode << std::endl;
        return true;
    }

    if(!sentence->getError().empty())
    {
        if (sentence->m_Mnem == nullptr) {
            stream << "\t\t\t\t\t\t\t\t" << sentence->m_OriginalCode << std::endl;
        }
        stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->getError() << std::endl;
        return false;
    }
    else if (!sentence->m_Warning.empty()) {
        stream << filename << "(" << std::to_string(sentence->getLineNum()) << "): " << sentence->m_Warning << std::endl;
    }
    stream << std::endl;

    return false;
}

bool GenerateListing::printSegments(std::ofstream &stream, std::vector<Label*> table)
{
    stream << "                N a m e         " << "\tSize\tLength" << std::endl << std::endl;

    if (table.empty()) {
        stream << "\tNone" << std::endl;
        return true;
    }

    std::vector<Label*>::iterator label = table.begin();

    while (label != table.end())
    {
        stream << (*label)->token->name << " ";

        if ((*label)->token->name.size() % 2 != 0)
        {
            stream << " ";
        }

        for (int i = 0; i < (32 - (*label)->token->name.size()) / 2; i++) {
            stream << ". ";
        }

        stream << "\t" << "32 Bit";
        stream << "\t" << std::setfill('0') << std::setw(4) << std::hex << *((int*)(*label)->value);

        stream << std::endl;
        label++;
    }

    return true;
}


bool GenerateListing::printLabels(std::ofstream &stream, std::vector<Label*> table)
{
    stream << "                N a m e         " << "\tType\tValue\tAttr" << std::endl << std::endl;

    if (table.empty()) {
        stream << "\tNone" << std::endl;
        return true;
    }

    std::vector<Label*>::iterator label = table.begin();
    LabelType last = (*label)->type;

    while (label != table.end())
    {
        if (last != (*label)->type) {
            stream << std::endl;
        }
        last = (*label)->type;

        stream << (*label)->token->name << " ";

        if ((*label)->token->name.size() % 2 != 0)
        {
            stream << " ";
        }

        for (int i = 0; i < (32 - (*label)->token->name.size()) / 2; i++) {
            stream << ". ";
        }

        if ((*label)->type == LabelType::NUMBER) {
            stream << "\t" << "NUMBER";
        }
        else if ((*label)->type == LabelType::TEXT) {
            stream << "\t" << "TEXT";
        }
        else if ((*label)->type == LabelType::LBYTE) {
            stream << "\t" << "L BYTE";
        }
        else if ((*label)->type == LabelType::LWORD) {
            stream << "\t" << "L WORD";
        }
        else if ((*label)->type == LabelType::LDWORD) {
            stream << "\t" << "L DWORD";
        }
        else if ((*label)->type == LabelType::LNEAR) {
            stream << "\t" << "L NEAR";
        }
        else if ((*label)->type == LabelType::LFAR) {
            stream << "\t" << "L FAR";
        }

        stream << "\t" << std::setfill('0') << std::setw(4) << std::hex << *((int*)(*label)->value);

        if ((*label)->segment != nullptr) {
            stream << "\t" << (*label)->segment->token->name;            
        }

        stream << std::endl;
        label++;
    }

    return true;
}