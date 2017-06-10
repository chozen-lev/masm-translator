#include <stack>
#include <vector>
#include <sstream>
#include <map>

class ExprEvaluate
{
private:

    static int evaluateOp(std::stack<std::string> &ops, std::stack<int> &vals, bool &error)
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

public:
    ExprEvaluate() {};
    ~ExprEvaluate() {};

    static int evaluate(std::vector<Token*>::iterator begin, std::vector<Token*>::iterator end, bool &error)
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
};