#include "Sentence.h"

Sentence::Sentence(int number, std::string line)
{
    m_lineNum = number;
    m_OriginalCode = line;
    m_Line = line;

    m_Label = nullptr;
    m_Mnem = nullptr;

    m_bytePtr = nullptr;
    m_bytePrefix = nullptr;
    m_byteCmd = nullptr;
    m_byteModRm = nullptr;
    m_byteSib = nullptr;
    m_byteDisp = nullptr;
    m_byteImm = nullptr;

    m_BytesNum = 0;
    m_DispBytesNum = 0;
}

std::string Sentence::getError() const {
    return m_Error;
}

std::string Sentence::getWarning() const {
    return m_Error;
}

int Sentence::getBytesNum() const
{
    int bytesNum = 0;

    if (m_bytePtr != nullptr) {
        bytesNum += 1;
    }
    
    if (m_bytePrefix != nullptr) {
        bytesNum += 1;
    }
    
    if (m_byteCmd != nullptr) {
        bytesNum += 1;
    }
    
    if (m_byteModRm != nullptr) {
        bytesNum += 1;
    }
    
    if (m_byteSib != nullptr) {
        bytesNum += 1;
    }

    if (m_byteDisp != nullptr) {
        bytesNum += m_DispBytesNum;
    }

    return bytesNum;
}