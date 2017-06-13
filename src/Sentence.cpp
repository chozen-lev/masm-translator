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

    m_BytesNum = 0;
    m_DispBytesNum = 0;
}

std::string Sentence::getError() const {
    return m_Error;
}

std::string Sentence::getWarning() const {
    return m_Error;
}