#include "Operand.h"

#include <sstream>
#include <map>
#include "Sentence.h"

Operand::Operand(std::vector<Token*>::iterator first)
{
    m_FirstToken = first;
    m_TokensNum = 1;

    m_Register = nullptr;
    m_SizePtr = nullptr;
    m_SegPrefix = nullptr;
    m_Label = nullptr;
    m_AddrReg1 = nullptr;
    m_AddrReg2 = nullptr;
    m_Const = nullptr;

    m_OperandSize = OpSize::NONE;
}

void Operand::analyzeAttributes(Sentence *sentence)
{
    std::vector<Token*>::iterator token = m_FirstToken;
    std::vector<Token*>::iterator end = m_FirstToken +  m_TokensNum - 1;

    // register
    if ((*token)->type == TokenType::REG8 || (*token)->type == TokenType::REG32)
    {
        if (token + 1 != end) {
            sentence->m_Error = "error: Operand expected";
        }

        m_Register = *token;
        return;
    }

    // string
    if ((*token)->type == TokenType::CONST_TEXT)
    {
        if (m_TokensNum > 1) {
            sentence->m_Warning = "warning: Extra characters on line";
        }

        m_Const = new OpConst(token, 1);
        return;
    }

    if ((*token)->type == TokenType::OPERATOR)
    {
        sentence->m_Error = "error: Operand expected";
        return;
    }
    // memory
    else if ((*token)->type == TokenType::TYPE)
    {
        if (token + 1 == end || (*(token + 1))->type != TokenType::OPERATOR)
        {
            sentence->m_Error = "error: Operator expected";
            return;
        }

        m_SizePtr = *token;
        token += 2;
    }

    if (token != end && (*token)->type == TokenType::SEGMENT)
    {
        if (token + 1 == end || (*(token + 1))->name != ":")
        {
            sentence->m_Error = "error: Illegal use of register";
            return;
        }

        m_SegPrefix = *token;
        token += 2;
    }

    if (token != end && (*token)->type == TokenType::IDENTIFIER)
    {
        m_Label = new Label(*token);
        token++;
    }

    if (token != end)
    {
        if ((*token)->name == "[")
        {
            token++;

            if (token == end)
            {
                sentence->m_Error = "error: Open parenthesis or bracket";
                return;
            }

            if ((*token)->type != TokenType::REG8 && (*token)->type != TokenType::REG32)
            {
                sentence->m_Error = "error: Operand expected";
                return;
            }

            m_AddrReg1 = *token;
            token++;

            if (token == end || (*token)->name != "+" || token + 1 == end)
            {
                sentence->m_Error = "error: Operator expected";
                return;
            }

            token++;

            if ((*token)->type != TokenType::REG8 && (*token)->type != TokenType::REG32)
            {
                sentence->m_Error = "error: Operand expected";
                return;
            }

            m_AddrReg2 = *token;
            token++;

            if (token == end)
            {
                sentence->m_Error = "error: Open parenthesis on bracket";
                return;
            }

            if ((*token)->name == "]")
            {
                if (token + 1 != end) {
                    sentence->m_Warning = "warning: Extra characters on line"; // error ?
                }
                
                return;
            }

            m_Const = new OpConst;

            do
            {
                m_Const->tokensNum++;

                if (m_Const->tokensNum == 1) {
                    m_Const->first = token;
                }

                token++;
            } while (token != end && (*token)->name != "]");

            if (m_Const->evaluateExpr() == false)
            {
                sentence->m_Error = "error: Displacement expression syntax";
                return;
            }

            if ((*token)->name != "]")
            {
                sentence->m_Error = "error: Open parenthesis or bracket";
                return;
            }

            if (token + 1 != end) {
                sentence->m_Warning = "warning: Extra characters on line";
            }

            return;
        }

        // immediate
        if (((*token)->name == "+" || (*token)->name == "-" || (*token)->name == "(" || (*token)->type == TokenType::CONST_BIN || (*token)->type == TokenType::CONST_DEC || (*token)->type == TokenType::CONST_HEX))
        {
            m_Const = new OpConst;
            do
            {
                m_Const->tokensNum++;

                if (m_Const->tokensNum == 1) {
                    m_Const->first = token;
                }

                token++;
            } while (token != end);
            
            if (!m_Const->evaluateExpr())
            {
                sentence->m_Error = "error: Immediate expression syntax";
                return;
            }

            return;
        }
    }
}

Operand::OpConst::OpConst(std::vector<Token*>::iterator _first, int _number): first(_first), tokensNum(_number), bytes(nullptr), bytesNum(0) {
    evaluateExpr();
}

bool Operand::OpConst::evaluateExpr()
{
    if ((*first)->type == TokenType::CONST_TEXT)
    {
        bytesNum = (*first)->name.size();
        bytes = new byte[bytesNum];
        std::copy((*first)->name.begin(), (*first)->name.end(), bytes);
    }
    else
    {
        bool error = false;
        int value = evaluate(((*first)->name == "+" || (*first)->name == "-") ? first + 1 : first, first + tokensNum, error);;

        if (error) {
            return false;
        }

        if (value < 0x10000)
        {
            if (value < 0x100) { 
                bytesNum = 1;   // 8 bits
            }
            else {
                bytesNum = 2;   // 16 bits
            }
        } else {
            if (value < 0x100000000L) {
                bytesNum = 4;   // 32 bits
            }
            else return false;  // more than 32 bits
        }

        bytes = new byte[bytesNum];

        if ((*first)->name == "-") {
            value = -value;
        }

        // memcpy() ?
        for (int i = 0; i < bytesNum; i++) {
            bytes[bytesNum - i - 1] = (value >> (i * 8));
        }
    }

    return true;
}

int Operand::OpConst::evaluateOp(std::stack<std::string> &ops, std::stack<int> &vals, bool &error)
{
    int v = vals.top(); vals.pop();

    if (!ops.empty())
    {
        std::string op = ops.top(); ops.pop();

        if (op == "+")
        {
            if (vals.empty())
            {
                error = true;
                return 0;
            }
            v = vals.top() + v;
            vals.pop();
        }
        else if (op == "-")
        {
            if (vals.empty())
            {
                error = true;
                return 0;
            }
            v = vals.top() - v;
            vals.pop();
        }
        else if (op == "*")
        {
            if (vals.empty())
            {
                error = true;
                return 0;
            }
            v = vals.top() * v;
            vals.pop();
        }
        else if (op == "/")
        {
            if (vals.empty())
            {
                error = true;
                return 0;
            }
            v = vals.top() / v;
            vals.pop();
        }
    }
    return v;
}

int Operand::OpConst::evaluate(std::vector<Token*>::iterator begin, std::vector<Token*>::iterator end, bool &error)
{
    static const std::map <std::string, int>OpPriority = {{ "(", 0 }, { "+", 1 }, { "-", 1 }, { "*", 2 }, { "/", 2 }};

    std::stack<std::string> ops;
    std::stack<int> vals;

    std::vector<Token*>::iterator it = begin;

    while (it != end)
    {
        if ((*it)->type == TokenType::CONST_BIN)
        {
            unsigned int x;
            x = std::strtol((*it)->name.c_str(), nullptr, 2);
            vals.push(x);
        }
        else if ((*it)->type == TokenType::CONST_DEC)
        {
            unsigned int x;
            std::stringstream ss;
            ss << (((*it)->name.back() == 'D') ? (*it)->name.substr(0, (*it)->name.size()) : (*it)->name);
            ss >> x;
            vals.push(x);
        }
        else if ((*it)->type == TokenType::CONST_HEX)
        {
            unsigned int x;
            std::stringstream ss;
            ss << std::hex << (*it)->name.substr(0, (*it)->name.size() - 1);
            ss >> x;
            vals.push(x);
        }
        else if ((*it)->name == "+" || (*it)->name == "-" || (*it)->name == "*" || (*it)->name == "/")
        {
            if (ops.empty() || OpPriority.at((*it)->name) > OpPriority.at(ops.top()))
            {
                ops.push((*it)->name);
                it++;
                continue;
            }

            vals.push(evaluateOp(ops, vals, error));

            if (error) {
                return -1;
            }

            ops.push((*it)->name);
        }
        else if ((*it)->name == "(") {
            ops.push((*it)->name);
        }
        else if ((*it)->name == ")")
        {
            std::string op = ops.top();

            do {
                vals.push(evaluateOp(ops, vals, error));
            } while (!ops.empty() && ops.top() != "(");

            if (error || ops.empty()) {
                error = true;
                return -1;
            }

            ops.pop();
        }
        it++;
    }

    while (!ops.empty() && !error) {
        vals.push(evaluateOp(ops, vals, error));
    }

    if (error || vals.size() > 1)
    {
        error = true;
        return -1;
    }
    return vals.top();
}