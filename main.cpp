#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <ranges>
#include <iomanip>

std::unordered_map<std::string, int> precedence = {
    {"*", 3},
    {"/", 3},
    {"+", 2},
    {"-", 2}
};
std::unordered_set<std::string> functions = {"pow", "abs", "max", "min"};


bool is_digit(const std::string& str) {
    return std::ranges::all_of(str, [](const char ch) { return (std::isdigit(ch) || ch == '.'); });
}

std::vector<std::string> tokenize(const std::string& infix) {
    std::vector<std::string> tokens;
    std::string number;
    std::string func;
    for (char ch : infix) {
        if (std::isdigit(ch) || ch == '.') number += ch;
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
            else if (ch == '(' || ch == ')' || ch == '-') {
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
        if (is_digit(token)) output.push_back(token);
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

double calculate(const std::string& infix) {
    const auto tokens = shunting_yard(infix);
    if (tokens.empty()) return std::numeric_limits<double>::infinity();

    std::stack<double> S;
    for (const std::string& token : tokens) {
        if (is_digit(token)) {
            S.push(std::stod(token));
        }
        else {
            double b = S.top(); S.pop();
            if (token == "abs") S.push(std::abs(b));
            else {
                double a;
                if (!S.empty()) {
                    a = S.top();
                    S.pop();
                }
                else if (token == "-") a = 0.0;
                else return std::numeric_limits<double>::infinity();

                if (token == "+") S.push(a + b);
                else if (token == "-") S.push(a - b);
                else if (token == "*") S.push(a * b);
                else if (token == "/") S.push(a / b);
                else if (token == "pow") S.push(pow(a, b));
                else if (token == "max") S.push(std::max(a, b));
                else if (token == "min") S.push(std::min(a, b));
            }
        }
    }
    return S.top();
}

[[noreturn]] int main() {
    while (true) {
        std::string input;
        std::getline(std::cin, input);

        double result = calculate(input);
        if (result == std::numeric_limits<double>::infinity()) std::cout << "Wrong input" << std::endl;
        else std::cout << std::setprecision(10) << calculate(input) << std::endl;
    }
}