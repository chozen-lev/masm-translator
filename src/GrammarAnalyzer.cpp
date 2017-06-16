#include "GrammarAnalyzer.h"

#include "Operand.h"
#include <algorithm>
#include <cstring>

void GrammarAnalyzer::analyzeStruct(Sentence *sentence, std::stack<Label*> &activeSegs, std::vector<Label*> &labels, std::vector<Label*> &segments)
{
    if (sentence->m_Tokens.empty() || !sentence->m_Error.empty()) {
        return;
    }

    if (sentence->m_Mnem != nullptr && sentence->m_Mnem->name == "EQU")
    {
        if (sentence->m_Label == nullptr)
        {
            sentence->m_Error = "error: Expected: instruction ot directive";
            return;
        }

        if (findLabelByName(segments, sentence->m_Label->token->name) != nullptr)
        {
            sentence->m_Error = "error: Symbol already different kind: " + sentence->m_Label->token->name;
            return;
        }

        if (sentence->m_Operands.size() != 1)
        {
            sentence->m_Error = "error: Expected: one operand";
            return;
        }

        if (findLabelByName(labels, sentence->m_Label->token->name) != nullptr)
        {
            sentence->m_Error = "error: Redefinition of symbol: " + sentence->m_Label->token->name;
            return;
        }

        if (sentence->m_Operands.front()->m_AddrReg1 != nullptr)
        {
            sentence->m_Error = "error: Improper operand type";
            return;
        }

        if (sentence->m_Operands.front()->m_Const == nullptr)
        {
            // skip, we will analyze it in step 2
            if (sentence->m_Operands.front()->m_Label != nullptr)
            {
                labels.push_back(sentence->m_Label);
                return;
            }

            sentence->m_Error = "error: Improper operand type";
            return;
        }

        sentence->m_Label->bytesNum = 2;
        sentence->m_Label->type = LabelType::NUMBER;

        if (sentence->m_Operands.front()->m_Const->bytesNum > 4)
        {
            sentence->m_Label->bytesNum = 4;
            if ((*sentence->m_Operands.front()->m_FirstToken)->type == TokenType::CONST_TEXT)
            {
                sentence->m_Label->bytesNum = sentence->m_Operands.front()->m_Const->bytesNum;
                sentence->m_Label->type = LabelType::TEXT;
            }
        }
        else if (sentence->m_Operands.front()->m_Const->bytesNum > 2)
        {
            sentence->m_Label->bytesNum = 4;
        }

        sentence->m_Label->value = new byte[sentence->m_Label->bytesNum]();    // () set bytes to zero
        memcpy(sentence->m_Label->value, sentence->m_Operands.front()->m_Const->bytes, sentence->m_Operands.front()->m_Const->bytesNum);

        labels.push_back(sentence->m_Label);

        return;
    }

    if (sentence->m_Mnem != nullptr && sentence->m_Mnem->name == "SEGMENT")
    {
        if (sentence->m_Label == nullptr)
        {
            sentence->m_Error = "error: Expected: instruction ot directive";
            return;
        }
        
        if (!activeSegs.empty() && (findLabelByNameInStack(activeSegs, sentence->m_Label->token->name) != nullptr))
        {
            sentence->m_Error = "error: Block nesting error";
            return;
        }

        if (findLabelByName(labels, sentence->m_Label->token->name) != nullptr)
        {
            sentence->m_Error = "error: Symbol already different kind: " + sentence->m_Label->token->name;
            return;
        }

        Label *buff;
        if ((buff = findLabelByName(segments, sentence->m_Label->token->name)) != nullptr)
        {
            delete sentence->m_Label;
            sentence->m_Label = buff;
            activeSegs.push(sentence->m_Label);
            return;
        }

        sentence->m_Label->type = LabelType::SEGMENT;
        sentence->m_Label->value = new byte[4]();
        activeSegs.push(sentence->m_Label);
        segments.push_back(sentence->m_Label);

        return;
    }

    if (sentence->m_Mnem != nullptr && sentence->m_Mnem->name == "ENDS")
    {
        if (sentence->m_Label == nullptr)
        {
            sentence->m_Error = "error: Expected: instruction ot directive";
            return;
        }

        if (activeSegs.empty() || activeSegs.top()->token->name != sentence->m_Label->token->name)
        {
            sentence->m_Error = "error: Block nesting error";
            return;
        }

        Label *buff;
        if ((buff = findLabelByName(segments, sentence->m_Label->token->name)) == nullptr)
        {
            sentence->m_Error = "error: Block nesting error";
            return;
        }

        delete sentence->m_Label;
        sentence->m_Label = buff;

        activeSegs.pop();

        return;
    }

    if (sentence->m_Mnem != nullptr && sentence->m_Mnem->name == "END") {
        return;
    }

    if (!activeSegs.empty())
    {
        if (sentence->m_Label != nullptr)
        {
            if (findLabelByName(segments, sentence->m_Label->token->name) != nullptr)
            {
                sentence->m_Error = "error: Symbol already different kind: " + sentence->m_Label->token->name;
                return;
            }

            if (findLabelByName(labels, sentence->m_Label->token->name) != nullptr)
            {
                sentence->m_Error = "error: Redefinition of symbol";
                return;
            }

            if (sentence->m_Tokens[sentence->m_Label->token->number]->name == ":" && sentence->m_Tokens.size() > 2 && sentence->m_Mnem == nullptr)
            {
                sentence->m_Error = "error: Expected: instruction ot directive";
                return;
            }

            sentence->m_Label->type = LabelType::LNEAR;     // there is no label directive in task. DEFAULT: NEAR
            sentence->m_Label->value = new byte[4]();       // 4 bytes for 32-bit segment
            memcpy(sentence->m_Label->value, activeSegs.top()->value, 4);
            sentence->m_Label->segment = activeSegs.top();
            labels.push_back(sentence->m_Label);
        }

        if (sentence->m_Mnem == nullptr) {
            return;
        }

        if (sentence->m_Mnem->name == "DB")
        {
            if (sentence->m_Label != nullptr) {
                sentence->m_Label->type = LabelType::LBYTE;
            }

            if (sentence->m_Operands.size() != 1)
            {
                sentence->m_Error = "error: Expected: one operand";
                return;
            }

            if (sentence->m_Operands.front()->m_AddrReg1 != nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands.front()->m_Const == nullptr)
            {
                // skip, we will analyze it in step 2
                if (sentence->m_Operands.front()->m_Label != nullptr) {
                    return;
                }

                sentence->m_Error = "error: Improper operand type";
                return;
            }

            sentence->m_DispBytesNum = sentence->m_Operands.front()->m_Const->bytesNum;

            if (sentence->m_Operands.front()->m_Const->bytesNum > 1 && (*sentence->m_Operands.front()->m_FirstToken)->type != TokenType::CONST_TEXT) {
                sentence->m_Error = "error: Value out of range";
            }

            sentence->m_byteDisp = new byte[sentence->m_DispBytesNum]();

            memcpy(sentence->m_byteDisp, sentence->m_Operands.front()->m_Const->bytes, sentence->m_DispBytesNum);

            return;
        }
        
        if (sentence->m_Mnem->name == "DW")
        {
            if (sentence->m_Label != nullptr) {
                sentence->m_Label->type = LabelType::LWORD;
            }

            if (sentence->m_Operands.size() != 1)
            {
                sentence->m_Error = "error: Expected: one operand";
                return;
            }

            if (sentence->m_Operands.front()->m_AddrReg1 != nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands.front()->m_Const == nullptr)
            {
                // skip, we will analyze it in step 2
                if (sentence->m_Operands.front()->m_Label != nullptr) {
                    return;
                }

                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands.front()->m_Const->bytesNum > 2)
            {
                sentence->m_Error = "error: Value out of range";
                return;
            }

            int bytesNum = (sentence->m_Operands.front()->m_Const->bytesNum == 1) ? 1 : 2;

            sentence->m_DispBytesNum = 2;

            sentence->m_byteDisp = new byte[2]();
            memcpy((bytesNum == 1) ? sentence->m_byteDisp + 1 : sentence->m_byteDisp, sentence->m_Operands.front()->m_Const->bytes, bytesNum);

            return;
        }

        if (sentence->m_Mnem->name == "DD")
        {
            if (sentence->m_Label != nullptr) {
                sentence->m_Label->type = LabelType::LDWORD;
            }

            if (sentence->m_Operands.size() != 1)
            {
                sentence->m_Error = "error: Expected: one operand";
                return;
            }

            if (sentence->m_Operands.front()->m_AddrReg1 != nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands.front()->m_Const == nullptr)
            {
                // skip, we will analyze it in step 2
                if (sentence->m_Operands.front()->m_Label != nullptr) {
                    return;
                }

                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands.front()->m_Const->bytesNum > 4)
            {
                sentence->m_Error = "error: Division by 0 or overflow";
                return;
            }

            sentence->m_DispBytesNum = 4;

            sentence->m_byteDisp = new byte[4]();
            memcpy(sentence->m_byteDisp, sentence->m_Operands.front()->m_Const->bytes, sentence->m_Operands.front()->m_Const->bytesNum);

            return;
        }

        if (sentence->m_Mnem->name == "CLC")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0xf8;

            if (!sentence->m_Operands.empty())
            {
                sentence->m_Error = "error: Expected: without any operands";
                return;
            }

            return;
        }

        if (sentence->m_Mnem->name == "DEC")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0xfe;

            if (sentence->m_Operands.size() != 1)
            {
                sentence->m_Error = "error: Expected: one operand";
                return;
            }

            // такое условие только для моей адресации
            if (sentence->m_Operands.front()->m_AddrReg1 == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }
            
            sentence->m_byteModRm = new byte();
            sentence->m_byteSib = new byte();

            if (sentence->m_Operands.front()->m_Const != nullptr)
            {
                sentence->m_byteDisp = new byte[(sentence->m_DispBytesNum = sentence->m_Operands.front()->m_Const->bytesNum == 1 ? 1 : 4)]();
                memcpy(sentence->m_byteDisp, sentence->m_Operands.front()->m_Const->bytes, sentence->m_Operands.front()->m_Const->bytesNum);
            }

            if (sentence->m_Operands.front()->m_SegPrefix != nullptr)
            {
                if (sentence->m_Operands.front()->m_AddrReg1->name == "EBP" || sentence->m_Operands.front()->m_AddrReg1->name == "ESP")
                {
                    if (sentence->m_Operands.front()->m_SegPrefix->name != "SS") {
                        sentence->m_bytePrefix = new byte();
                        sentence->m_bytePrefix[0] = m_SegPrefs.at(sentence->m_Operands.front()->m_SegPrefix->name);
                    }
                }
                else if (sentence->m_Operands.front()->m_SegPrefix->name != "DS") {
                    sentence->m_bytePrefix = new byte();
                    sentence->m_bytePrefix[0] = m_SegPrefs.at(sentence->m_Operands.front()->m_SegPrefix->name);
                }
            }

            // 111 - additional opcode, 100 - [][] addressing mode
            sentence->m_byteModRm[0] = 0x0c;

            // disp bytes num
            if (sentence->m_DispBytesNum == 1) {
                sentence->m_byteModRm[0] += 0x40;
            }
            else if (sentence->m_DispBytesNum == 4) {
                sentence->m_byteModRm[0] += 0x80;
            }

            if (sentence->m_Operands.front()->m_Label == nullptr && sentence->m_Operands.front()->m_SizePtr == nullptr)
            {
                sentence->m_Error = "error: Operand must have size";
                return;
            }

            return;
        }

        if (sentence->m_Mnem->name == "ADD")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0x02;

            if (sentence->m_Operands.size() != 2)
            {
                sentence->m_Error = "error: Expected: two operand";
                return;
            }

            if (sentence->m_Operands[0]->m_Register == nullptr || sentence->m_Operands[1]->m_Register == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if ((*sentence->m_Operands[0]->m_FirstToken)->type != (*sentence->m_Operands[1]->m_FirstToken)->type)
            {
                sentence->m_byteCmd[0] = 0x03;  // one of them is 32 bit register
                sentence->m_Warning = "warning: Operand types must match";
            }
            else if ((*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG32) {
                sentence->m_byteCmd[0] = 0x03;  // both are 32 bit registers
            }

            sentence->m_byteModRm = new byte();

            return;
        }

        if (sentence->m_Mnem->name == "IDIV")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0xF6;

            if (sentence->m_Operands.size() != 1)
            {
                sentence->m_Error = "error: Expected: one operand";
                return;
            }

            if (sentence->m_Operands[0]->m_Register == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            sentence->m_byteModRm = new byte();

            // 11 - reg mode, 111 - additional opcode, *** - reg num
            sentence->m_byteModRm[0] = 0xf8;

            if ((*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG32) {
                sentence->m_byteCmd[0] = 0xF7;
                sentence->m_byteModRm[0] += m_Reg32.at(sentence->m_Operands[0]->m_Register->name);
            }
            else {
                sentence->m_byteModRm[0] += m_Reg8.at(sentence->m_Operands[0]->m_Register->name);
            }

            return;
        }

        if (sentence->m_Mnem->name == "CMP")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0x3a;

            if (sentence->m_Operands.size() != 2)
            {
                sentence->m_Error = "error: Expected: two operand";
                return;
            }

            if (sentence->m_Operands[0]->m_Register == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands[1]->m_Label == nullptr && sentence->m_Operands[1]->m_AddrReg1 == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands[1]->m_SegPrefix != nullptr)
            {
                if (sentence->m_Operands[1]->m_AddrReg1->name == "EBP" || sentence->m_Operands[1]->m_AddrReg1->name == "ESP")
                {
                    if (sentence->m_Operands[1]->m_SegPrefix->name != "SS") {
                        sentence->m_bytePrefix = new byte();
                        sentence->m_bytePrefix[0] = m_SegPrefs.at(sentence->m_Operands[1]->m_SegPrefix->name);
                    }
                }
                else if (sentence->m_Operands[1]->m_SegPrefix->name != "DS") {
                    sentence->m_bytePrefix = new byte();
                    sentence->m_bytePrefix[0] = m_SegPrefs.at(sentence->m_Operands[1]->m_SegPrefix->name);
                }
            }

            sentence->m_byteModRm = new byte();
            sentence->m_byteSib = new byte();

            if (sentence->m_Operands[1]->m_Const != nullptr)
            {
                sentence->m_byteDisp = new byte[(sentence->m_DispBytesNum = sentence->m_Operands[1]->m_Const->bytesNum == 1 ? 1 : 4)]();
                memcpy(sentence->m_byteDisp, sentence->m_Operands[1]->m_Const->bytes, sentence->m_Operands[1]->m_Const->bytesNum);
            }

            return;
        }

        if (sentence->m_Mnem->name == "AND")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0x80;

            if (sentence->m_Operands.size() != 2)
            {
                sentence->m_Error = "error: Expected: two operands";
                return;
            }

            if (sentence->m_Operands[0]->m_AddrReg1 == nullptr || (sentence->m_Operands[1]->m_Const == nullptr && sentence->m_Operands[1]->m_Label == nullptr))
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands[0]->m_SizePtr == nullptr && sentence->m_Operands[0]->m_Label == nullptr)
            {
                sentence->m_Error = "error: Operand must have size";
                return;
            }
            
            sentence->m_byteModRm = new byte();
            sentence->m_byteSib = new byte();

            if (sentence->m_Operands[1]->m_Const != nullptr)
            {
                sentence->m_byteDisp = new byte[(sentence->m_DispBytesNum = sentence->m_Operands[1]->m_Const->bytesNum == 1 ? 1 : 4)]();
                memcpy(sentence->m_byteDisp, sentence->m_Operands[1]->m_Const->bytes, sentence->m_Operands[1]->m_Const->bytesNum);
            }

            return;
        }

        if (sentence->m_Mnem->name == "MOV")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0xc7;

            if (sentence->m_Operands[0]->m_Register == nullptr || (sentence->m_Operands[1]->m_Const == nullptr && sentence->m_Operands[1]->m_Label == nullptr))
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if ((*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG8)
            {
                sentence->m_byteCmd[0] = 0xb0;
                sentence->m_byteCmd[0] += m_Reg8.at(sentence->m_Operands[0]->m_Register->name);
            }
            else
            {   // for 32 bit registers
                sentence->m_byteCmd[0] = 0xb8;
                sentence->m_byteCmd[0] += m_Reg32.at(sentence->m_Operands[0]->m_Register->name);
            }

            if (sentence->m_Operands[1]->m_Const != nullptr)
            {
                sentence->m_byteDisp = new byte[(sentence->m_DispBytesNum = (*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG8 ? 1 : 4)]();
                memcpy(sentence->m_byteDisp, sentence->m_Operands[1]->m_Const->bytes, (sizeof(sentence->m_byteDisp) > sentence->m_Operands[1]->m_Const->bytesNum ? 1 : sizeof(sentence->m_byteDisp)));
            }

            return;
        }

        if (sentence->m_Mnem->name == "XOR")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0x30;

            if (sentence->m_Operands.size() != 2)
            {
                sentence->m_Error = "error: Expected: two operands";
                return;
            }

            if (sentence->m_Operands[0]->m_AddrReg1 == nullptr || sentence->m_Operands[1]->m_Register == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            if (sentence->m_Operands.front()->m_SegPrefix != nullptr)
            {
                if (sentence->m_Operands.front()->m_AddrReg1->name == "EBP" || sentence->m_Operands.front()->m_AddrReg1->name == "ESP")
                {
                    if (sentence->m_Operands.front()->m_SegPrefix->name != "SS") {
                        sentence->m_bytePrefix = new byte();
                        sentence->m_bytePrefix[0] = m_SegPrefs.at(sentence->m_Operands.front()->m_SegPrefix->name);
                    }
                }
                else if (sentence->m_Operands.front()->m_SegPrefix->name != "DS") {
                    sentence->m_bytePrefix = new byte();
                    sentence->m_bytePrefix[0] = m_SegPrefs.at(sentence->m_Operands.front()->m_SegPrefix->name);
                }
            }

            sentence->m_byteModRm = new byte();
            sentence->m_byteSib = new byte();

            if (sentence->m_Operands.front()->m_Const != nullptr)
            {
                sentence->m_byteDisp = new byte[(sentence->m_DispBytesNum = sentence->m_Operands.front()->m_Const->bytesNum == 1 ? 1 : 4)]();
                memcpy(sentence->m_byteDisp, sentence->m_Operands.front()->m_Const->bytes, sentence->m_Operands.front()->m_Const->bytesNum);
            }

            return;
        }

        if (sentence->m_Mnem->name == "JAE")
        {
            sentence->m_byteCmd = new byte();
            sentence->m_byteCmd[0] = 0x0f;

            if (sentence->m_Operands.size() != 1)
            {
                sentence->m_Error = "error: Expected: one operand";
                return;
            }

            if (sentence->m_Operands.front()->m_Label == nullptr)
            {
                sentence->m_Error = "error: Improper operand type";
                return;
            }

            sentence->m_DispBytesNum = 1;

            if (findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name) == nullptr)
            {
                sentence->m_DispBytesNum = 4;
                sentence->m_byteModRm = new byte(); // second command byte
            }

            sentence->m_byteDisp = new byte[sentence->m_DispBytesNum]();

            return;
        }

        return;
    }

    if (sentence->m_Label != nullptr && sentence->m_Tokens[sentence->m_Label->token->number]->name == ":")
    {
        sentence->m_Error = "error: Missing or unreachable CS";
        return;
    }

    if (sentence->m_Mnem != nullptr)
    {
        if ((std::find(m_SegMnem.begin(), m_SegMnem.end(), sentence->m_Mnem->name)) != m_SegMnem.end())
        {
            sentence->m_Error = "error: Data emitted with no segment";
            return;
        }
    }
}

void GrammarAnalyzer::analyzeOffsets(Sentence *sentence, std::stack<Label*> &activeSegs, std::vector<Label*> &labels, std::vector<Label*> &segments)
{
    if (sentence->m_Tokens.empty() || !sentence->m_Error.empty()) {
        return;
    }

    if (sentence->m_Mnem != nullptr)
    {
        if (sentence->m_Mnem->name == "SEGMENT") {
            activeSegs.push(sentence->m_Label);
        }
        else if (sentence->m_Mnem->name == "ENDS") {
            activeSegs.pop();
        }

        if (sentence->m_Label != nullptr && (sentence->m_Tokens[sentence->m_Label->token->number]->name == "DB" || sentence->m_Tokens[sentence->m_Label->token->number]->name == "DW" || sentence->m_Tokens[sentence->m_Label->token->number]->name == "DD")) {
            memcpy(sentence->m_Label->value, activeSegs.top()->value, 4);
        }
    }

    if (!sentence->m_Operands.empty())
    {
        if (sentence->m_Mnem->name == "EQU" && sentence->m_Operands.front()->m_Label != nullptr)
        {
            Label *buff;
            if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
            {
                if ((buff = findLabelByName(segments, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }
            }

            if (buff->value == nullptr)
            {
                sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                return;
            }

            delete sentence->m_Operands.front()->m_Label;
            sentence->m_Operands.front()->m_Label = buff;

            sentence->m_Label->type = sentence->m_Operands.front()->m_Label->type;
            sentence->m_Label->bytesNum = sentence->m_Operands.front()->m_Label->bytesNum;
            sentence->m_Label->value = sentence->m_Operands.front()->m_Label->value;
        }
        else if (sentence->m_Mnem->name == "DB" && sentence->m_Operands.front()->m_Label != nullptr)
        {
            Label *buff;
            if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
            {
                if ((buff = findLabelByName(segments, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }
            }

            if (buff->value == nullptr)
            {
                sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                return;
            }

            if (buff->type == LabelType::TEXT) {
                sentence->m_DispBytesNum = buff->bytesNum;
            }
            else {
                if (buff->bytesNum > 2 || ((buff->value[1] << 8) | buff->value[0]) > 255)
                {
                    sentence->m_Error = "error: Value out of range";
                    return;
                }

                sentence->m_DispBytesNum = 1;
            }
            
            sentence->m_byteDisp = new byte[sentence->m_DispBytesNum]();

            memcpy(sentence->m_byteDisp, buff->value, sentence->m_DispBytesNum);
        }
        else if (sentence->m_Mnem->name == "DW" && sentence->m_Operands.front()->m_Label != nullptr)
        {
            Label *buff;
            if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
            {
                if ((buff = findLabelByName(segments, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }
            }

            if (buff->value == nullptr)
            {
                sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                return;
            }

            if (buff->bytesNum > 2)
            {
                sentence->m_Error = "error: Value out of range";
                return;
            }

            sentence->m_DispBytesNum = 2;
            
            sentence->m_byteDisp = new byte[sentence->m_DispBytesNum]();

            memcpy(sentence->m_byteDisp, buff->value, sentence->m_DispBytesNum);
        }
        else if (sentence->m_Mnem->name == "DD" && sentence->m_Operands.front()->m_Label != nullptr)
        {
            Label *buff;
            if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
            {
                if ((buff = findLabelByName(segments, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }
            }

            if (buff->value == nullptr)
            {
                sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                return;
            }

            if (buff->bytesNum > 4)
            {
                sentence->m_Error = "error: Value out of range";
                return;
            }

            sentence->m_DispBytesNum = 4;
            
            sentence->m_byteDisp = new byte[sentence->m_DispBytesNum]();

            memcpy(sentence->m_byteDisp, buff->value, sentence->m_DispBytesNum);
        }
        else if (sentence->m_Mnem->name == "DEC")
        {
            Label *buff;
            if (sentence->m_Operands.front()->m_Label != nullptr)
            {
                if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }

                delete sentence->m_Operands.front()->m_Label;
                sentence->m_Operands.front()->m_Label = buff;

                if (sentence->m_DispBytesNum <= 1)
                {
                    int val = 0;
                    val = (sentence->m_DispBytesNum == 1) ? sentence->m_byteDisp[0] : 0;
                    delete sentence->m_byteDisp;
                    sentence->m_byteDisp = new byte[4]();
                    sentence->m_byteDisp[0] = val;
                    sentence->m_DispBytesNum = 4;
                }

                *((int*)sentence->m_byteDisp) += *((int*)sentence->m_Operands.front()->m_Label->value);
            }
            
            if (sentence->m_Operands.front()->m_SizePtr != nullptr)
            {
                if (sentence->m_Operands.front()->m_SizePtr->name != "BYTE")
                {
                    sentence->m_byteCmd[0] = 0xff;
                }

                if (sentence->m_Operands.front()->m_SizePtr->name == "WORD")
                {
                    sentence->m_bytePtr = new byte();
                    sentence->m_bytePtr[0] = 0x66;
                }
            }
            else 
            {
                if (sentence->m_Operands.front()->m_Label->type != LabelType::LBYTE) {
                    sentence->m_byteCmd[0] = 0xff;
                }

                if (sentence->m_Operands.front()->m_Label->type == LabelType::LWORD)
                {
                    sentence->m_bytePtr = new byte();
                    sentence->m_bytePtr[0] = 0x66;
                }
            }

            // 001 - additional opcode, 100 - [][] addressing mode
            sentence->m_byteModRm[0] = 0x0c;

            // disp bytes num
            if (sentence->m_DispBytesNum == 1) {
                sentence->m_byteModRm[0] += 0x40;
            }
            else if (sentence->m_DispBytesNum == 4) {
                sentence->m_byteModRm[0] += 0x80;
            }

            sentence->m_byteSib[0] = (m_Reg32.at(sentence->m_Operands.front()->m_AddrReg2->name) << 3) + m_Reg32.at(sentence->m_Operands.front()->m_AddrReg1->name);
        }
        else if (sentence->m_Mnem->name == "ADD")
        {
            sentence->m_byteModRm[0] = 0xc0; // both operands are reg

            if ((*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG32) {
                sentence->m_byteModRm[0] += m_Reg32.at(sentence->m_Operands[0]->m_Register->name) << 3;
            } else {
                sentence->m_byteModRm[0] += m_Reg8.at(sentence->m_Operands[0]->m_Register->name) << 3;
            }

            if ((*sentence->m_Operands[1]->m_FirstToken)->type == TokenType::REG32) {
                sentence->m_byteModRm[0] += m_Reg32.at(sentence->m_Operands[1]->m_Register->name);
            } else {
                sentence->m_byteModRm[0] += m_Reg8.at(sentence->m_Operands[1]->m_Register->name);
            }

            // sentence->m_byteModRm[0] += (m_Reg32.at(sentence->m_Operands[0]->m_Register->name) << 3) + m_Reg32.at(sentence->m_Operands[1]->m_Register->name);
        }
        else if (sentence->m_Mnem->name == "CMP")
        {
            Label *buff;
            if (sentence->m_Operands[1]->m_Label != nullptr)
            {
                if ((buff = findLabelByName(labels, sentence->m_Operands[1]->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands[1]->m_Label->token->name;
                    return;
                }

                delete sentence->m_Operands[1]->m_Label;
                sentence->m_Operands[1]->m_Label = buff;

                if (sentence->m_DispBytesNum <= 1)
                {
                    int val = 0;
                    val = (sentence->m_DispBytesNum == 1) ? sentence->m_byteDisp[0] : 0;
                    delete sentence->m_byteDisp;
                    sentence->m_byteDisp = new byte[4]();
                    sentence->m_byteDisp[0] = val;
                    sentence->m_DispBytesNum = 4;
                }

                *((int*)sentence->m_byteDisp) += *((int*)sentence->m_Operands[1]->m_Label->value);
            }

            if ((*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG32)
            {
                sentence->m_byteCmd[0] = 0x3b;

                if (sentence->m_Operands[1]->m_SizePtr != nullptr)
                {
                    if (sentence->m_Operands[1]->m_SizePtr->name != "DWORD") {
                        sentence->m_Warning = "warning: Operand types must match";
                    }
                }
                else if (sentence->m_Operands[1]->m_Label != nullptr && sentence->m_Operands[1]->m_Label->type != LabelType::LDWORD)
                {
                    sentence->m_Warning = "warning: Operand types must match";
                }
            }
            else
            {
                if (sentence->m_Operands[1]->m_SizePtr != nullptr)
                {
                    if (sentence->m_Operands[1]->m_SizePtr->name != "BYTE") {
                        sentence->m_Warning = "warning: Operand types must match";
                    }
                }
                else if (sentence->m_Operands[1]->m_Label != nullptr && sentence->m_Operands[1]->m_Label->type != LabelType::LBYTE)
                {
                    sentence->m_Warning = "warning: Operand types must match";
                }
            }

            // 100 - [][] addressing mode
            sentence->m_byteModRm[0] = 0x04;

            // reg
            if ((*sentence->m_Operands[0]->m_FirstToken)->type == TokenType::REG8) {
                sentence->m_byteModRm[0] += m_Reg8.at(sentence->m_Operands[0]->m_Register->name) << 3;
            } else {
                sentence->m_byteModRm[0] += m_Reg32.at(sentence->m_Operands[0]->m_Register->name) << 3;
            }

            // disp bytes num
            if (sentence->m_DispBytesNum == 1) {
                sentence->m_byteModRm[0] += 0x40;
            }
            else if (sentence->m_DispBytesNum == 4) {
                sentence->m_byteModRm[0] += 0x80;
            }

            sentence->m_byteSib[0] = (m_Reg32.at(sentence->m_Operands[1]->m_AddrReg2->name) << 3) + m_Reg32.at(sentence->m_Operands[1]->m_AddrReg1->name);
        }
        else if (sentence->m_Mnem->name == "AND")
        {
            Label *buff;
            if (sentence->m_Operands.front()->m_Label != nullptr)
            {
                if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }

                delete sentence->m_Operands.front()->m_Label;
                sentence->m_Operands.front()->m_Label = buff;

                if (sentence->m_DispBytesNum <= 1)
                {
                    int val = 0;
                    val = (sentence->m_DispBytesNum == 1) ? sentence->m_byteDisp[0] : 0;
                    delete sentence->m_byteDisp;
                    sentence->m_byteDisp = new byte[4]();
                    sentence->m_byteDisp[0] = val;
                    sentence->m_DispBytesNum = 4;
                }

                *((int*)sentence->m_byteDisp) += *((int*)sentence->m_Operands.front()->m_Label->value);
            }

            if (sentence->m_Operands.front()->m_SizePtr != nullptr)
            {
                if (sentence->m_Operands.front()->m_SizePtr->name != "BYTE")
                {
                    sentence->m_byteCmd[0] = 0x81;
                }

                if (sentence->m_Operands.front()->m_SizePtr->name == "WORD")
                {
                    sentence->m_bytePtr = new byte();
                    sentence->m_bytePtr[0] = 0x66;
                }
            }
            else 
            {
                if (sentence->m_Operands.front()->m_Label->type != LabelType::LBYTE) {
                    sentence->m_byteCmd[0] = 0x81;
                }

                if (sentence->m_Operands.front()->m_Label->type == LabelType::LWORD)
                {
                    sentence->m_bytePtr = new byte();
                    sentence->m_bytePtr[0] = 0x66;
                }
            }
        }
        else if (sentence->m_Mnem->name == "XOR")
        {
            Label *buff;
            if (sentence->m_Operands.front()->m_Label != nullptr)
            {
                if ((buff = findLabelByName(labels, sentence->m_Operands.front()->m_Label->token->name)) == nullptr)
                {
                    sentence->m_Error = "error: Symbol not defined: " + sentence->m_Operands.front()->m_Label->token->name;
                    return;
                }

                delete sentence->m_Operands.front()->m_Label;
                sentence->m_Operands.front()->m_Label = buff;

                if (sentence->m_DispBytesNum <= 1)
                {
                    int val = 0;
                    val = (sentence->m_DispBytesNum == 1) ? sentence->m_byteDisp[0] : 0;
                    delete sentence->m_byteDisp;
                    sentence->m_byteDisp = new byte[4]();
                    sentence->m_byteDisp[0] = val;
                    sentence->m_DispBytesNum = 4;
                }

                *((int*)sentence->m_byteDisp) += *((int*)sentence->m_Operands.front()->m_Label->value);
            }

            if ((*sentence->m_Operands[1]->m_FirstToken)->type == TokenType::REG32)
            {
                sentence->m_byteCmd[0] = 0x31;

                if (sentence->m_Operands[0]->m_SizePtr != nullptr)
                {
                    if (sentence->m_Operands[0]->m_SizePtr->name != "DWORD") {
                        sentence->m_Warning = "warning: Operand types must match";
                    }
                }
                else if (sentence->m_Operands[0]->m_Label != nullptr && sentence->m_Operands[0]->m_Label->type != LabelType::LDWORD)
                {
                    sentence->m_Warning = "warning: Operand types must match";
                }
            }
            else
            {
                if (sentence->m_Operands[0]->m_SizePtr != nullptr)
                {
                    if (sentence->m_Operands[0]->m_SizePtr->name != "BYTE") {
                        sentence->m_Warning = "warning: Operand types must match";
                    }
                }
                else if (sentence->m_Operands[0]->m_Label != nullptr && sentence->m_Operands[0]->m_Label->type != LabelType::LBYTE)
                {
                    sentence->m_Warning = "warning: Operand types must match";
                }
            }

            // 100 - [][] addressing mode
            sentence->m_byteModRm[0] = 0x04;

            // reg
            if ((*sentence->m_Operands[1]->m_FirstToken)->type == TokenType::REG8) {
                sentence->m_byteModRm[0] += m_Reg8.at(sentence->m_Operands[1]->m_Register->name) << 3;
            } else {
                sentence->m_byteModRm[0] += m_Reg32.at(sentence->m_Operands[1]->m_Register->name) << 3;
            }

            // disp bytes num
            if (sentence->m_DispBytesNum == 1) {
                sentence->m_byteModRm[0] += 0x40;
            }
            else if (sentence->m_DispBytesNum == 4) {
                sentence->m_byteModRm[0] += 0x80;
            }

            sentence->m_byteSib[0] = (m_Reg32.at(sentence->m_Operands.front()->m_AddrReg2->name) << 3) + m_Reg32.at(sentence->m_Operands.front()->m_AddrReg1->name);
        }
    }

    if (sentence->m_Label != nullptr && sentence->m_Tokens[sentence->m_Label->token->number]->name == ":") {
        memcpy(sentence->m_Label->value, activeSegs.top()->value, 4);
    }

    if (!activeSegs.empty()) {
        *((int*)activeSegs.top()->value) += sentence->getBytesNum();
    }
}

Label* GrammarAnalyzer::findLabelByName(std::vector<Label*> table, std::string name)
{
    for (std::vector<Label*>::iterator label = table.begin(); label != table.end(); label++)
    {
        if ((*label)->token->name == name) {
            return *label;
        }
    }

    return nullptr;
}

Label* GrammarAnalyzer::findLabelByNameInStack(std::stack<Label*> st, std::string name)
{
    while (!st.empty())
    {
        if (st.top()->token->name == name) {
            return st.top();
        }
        st.pop();
    }

    return nullptr;
}