#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>

std::unordered_map<std::string, int> precedence = {
    {"*", 3},
    {"/", 3},
    {"+", 2},
    {"-", 2}
};
std::unordered_set<std::string> functions = {"pow", "abs", "max", "min"};

std::vector<std::string> tokenize(const std::string& infix) {
    std::vector<std::string> tokens;
    std::string number;
    std::string func;
    for (char ch : infix) {
        if (std::isdigit(ch)) number += ch;
        else {
            if (!number.empty()) {
                tokens.push_back(number);
                number.clear();
            }
            if (std::isspace(ch)) {
                if (!func.empty()) {
                    tokens.emplace_back(func);
                    func.clear();
                }
            }
            else if (ch == '(' || ch == ')') {
                if (!func.empty()) {
                    tokens.emplace_back(func);
                    func.clear();
                }
                tokens.emplace_back(1, ch);
            }
            else func += ch;
        }
    }
    if (!func.empty()) tokens.push_back(func);
    if (!number.empty()) tokens.push_back(number);
    return tokens;
}

std::vector<std::string> shunting_yard(const std::string& infix) {
    std::stack<std::string> op_stack;
    std::vector<std::string> output;
    auto tokens = tokenize(infix);

    for (const std::string& token : tokens) {
        if (std::isdigit(token[0])) output.push_back(token);
        else if (functions.contains(token)) op_stack.push(token);
        else if (precedence.contains(token)) {
            while (!op_stack.empty() && op_stack.top() != "(" && precedence[op_stack.top()] >= precedence[token]) {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(token);
        }
        else if (token == ",") {
            while (!op_stack.empty() && op_stack.top() != "(") {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
        }
        else if (token == "(") op_stack.push(token);
        else if (token == ")") {
            while (!op_stack.empty() && op_stack.top() != "(") {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.pop();
            if (!op_stack.empty() && functions.contains(op_stack.top())) {
                output.push_back(op_stack.top());
                op_stack.pop();
            }
        }
    }
    while (!op_stack.empty()) {
        output.push_back(op_stack.top());
        op_stack.pop();
    }
    return output;
}

float calculate(const std::string& infix) {
    auto tokens = shunting_yard(infix);
    std::stack<float> S;
    for (const std::string& token : tokens) {
        if (std::isdigit(token[0])) {
            S.push(std::stof(token));
        }
        else {
            float b = S.top(); S.pop();
            float a = S.top(); S.pop();
            if (token == "+") S.push(a + b);
            else if (token == "-") S.push(a - b);
            else if (token == "*") S.push(a * b);
            else if (token == "/") S.push(a / b);
            else if (token == "pow") S.push(pow(a, b));
            else if (token == "abs") S.push(std::abs(a));
            else if (token == "max") S.push(std::max(a, b));
            else if (token == "min") S.push(std::min(a, b));
        }
    }
    return S.top();
}

int main() {
    auto input = "max(min(3 * 2, 2), 2)";
    for (const auto& token : tokenize(input)) {
        std::cout << token << " ";
    }
    std::cout << std::endl;

    auto result = shunting_yard(input);
    for (const auto& token : result) {
        std::cout << token << " ";
    }
    std::cout << std::endl;

    std::cout << calculate(input) << std::endl;
    return 0;
}