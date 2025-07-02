#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <ranges>
#include <iomanip>
#include <sstream>

std::unordered_map<std::string, int> precedence = {
    {"*", 3},
    {"/", 3},
    {"+", 2},
    {"-", 2}
};
std::unordered_set<std::string> functions = {"pow", "abs", "max", "min"};
std::unordered_map<std::string, double> global_variables;
struct UserFunction {
    std::vector<std::string> args;
    std::vector<std::string> expr;
};
std::unordered_map<std::string, UserFunction> user_functions;

void variable_definition(const std::string& input);
void print_variables();
void function_definition(const std::string& input);
void print_functions();
std::vector<std::string> tokenize(const std::string& infix);
double calculate(std::vector<std::string>& tokens, const std::unordered_map<std::string, double>& variables);


[[noreturn]] int main() {
    auto get_input = []() {
        std::string input;
        std::getline(std::cin, input);

        if (input == "vars") {
            print_variables();
            return;
        }
        if (input == "defs") {
            print_functions();
            return;
        }
        if (input.starts_with("var ")) {
            variable_definition(input);
            return;
        }
        if (input.starts_with("def ")) {
            function_definition(input);
            return;

        }
        input.erase(std::ranges::remove_if(input, ::isspace).begin(), input.end());
        auto tokens = tokenize(input);
        const double result = calculate(tokens, global_variables);
        if (result == std::numeric_limits<double>::infinity()) std::cout << "Incorrect expression" << std::endl;
        else std::cout << std::setprecision(10) << result << std::endl;
    };
    while (true) {
        get_input();
    }
}

bool is_digit(const std::string& str) {
    return std::ranges::all_of(str, [](const char ch) { return (std::isdigit(ch) || ch == '.'); });
}

std::vector<std::string> tokenize(const std::string& infix) {
    std::vector<std::string> tokens;
    std::string tk;
    for (char ch : infix) {
        if (ch == '(' || ch == ')' || ch == ',' or precedence.contains(std::string(1, ch))) {
            if (!tk.empty()) {
                tokens.emplace_back(tk);
                tk.clear();
            }
            tokens.emplace_back(1, ch);
        }
        else tk += ch;
        }
    if (!tk.empty()) tokens.push_back(tk);
    return tokens;
}

std::vector<std::string> shunting_yard(const std::vector<std::string>& tokens, const std::unordered_map<std::string, double>& variables) {
    std::stack<std::string> op_stack;
    std::vector<std::string> output;

    for (const std::string& token : tokens) {
        if (is_digit(token) || variables.contains(token)) output.push_back(token);
        else if (functions.contains(token) || user_functions.contains(token)) op_stack.push(token);
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
        else return {};
    }
    while (!op_stack.empty()) {
        output.push_back(op_stack.top());
        op_stack.pop();
    }
    return output;
}

double calculate(std::vector<std::string>& tokens, const std::unordered_map<std::string, double>& variables) {
    tokens = shunting_yard(tokens, variables);
    if (tokens.empty()) return std::numeric_limits<double>::infinity();

    std::stack<double> S;
    for (const std::string& token : tokens) {
        if (is_digit(token)) {
            S.push(std::stod(token));
        }
        else if (variables.contains(token)) {
            S.push(variables.at(token));
        }
        else if (functions.contains(token) || precedence.contains(token)) {
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
        else if (user_functions.contains(token)) {
            UserFunction func = user_functions[token];
            std::unordered_map<std::string, double> inner_variables;

            for (const auto& arg : func.args) {
                if (S.empty()) return std::numeric_limits<double>::infinity();
                inner_variables[arg] = S.top();
                S.pop();
            }
            S.push(calculate(func.expr, inner_variables));
        }
        else return std::numeric_limits<double>::infinity();
    }
    return S.top();
}


void variable_definition(const std::string& input) {
    if (size_t eq_pos = input.find('='); eq_pos != std::string::npos) {
        std::string var_name = input.substr(4, eq_pos - 4);
        var_name.erase(std::ranges::remove_if(var_name, ::isspace).begin(), var_name.end());
        std::string expr = input.substr(eq_pos + 1);
        expr.erase(std::ranges::remove_if(expr, ::isspace).begin(), expr.end());

        auto tokens = tokenize(expr);
        const double value = calculate(tokens, global_variables);
        if (value == std::numeric_limits<double>::infinity()) std::cout << "Incorrect expression" << std::endl;
        else global_variables[var_name] = value;
    }
    else std::cout << "Missing '=' in variable assignment" << std::endl;
}

void print_variables() {
    for (const auto& [key, value] : global_variables) {
        std::cout << key << " = " << std::setprecision(10) << value << std::endl;
    }
}

void function_definition(const std::string& input) {
    size_t paren_open = input.find('(');
    size_t paren_close = input.find(')', paren_open);
    size_t braces_open = input.find('{');
    size_t braces_end = input.find('}', braces_open);


    if (!(paren_open == std::string::npos || paren_close == std::string::npos ||
        braces_open == std::string::npos || braces_end == std::string::npos)) {
        std::string func_name = input.substr(4, paren_open-4);
        func_name.erase(std::ranges::remove_if(func_name, ::isspace).begin(), func_name.end());

        auto split_by_comma = [](const std::string& inpt) {
            std::vector<std::string> result;
            std::istringstream ss(inpt);
            std::string item;
            while (std::getline(ss, item, ',')) result.push_back(item);
            std::ranges::reverse(result);
            return result;
        };

        std::string args_string = input.substr(paren_open + 1, paren_close - paren_open - 1);
        args_string.erase(std::ranges::remove_if(args_string, ::isspace).begin(), args_string.end());
        const auto args = split_by_comma(args_string);

        std::string expr = input.substr(braces_open + 1, braces_end-braces_open-1);
        expr.erase(std::ranges::remove_if(expr, ::isspace).begin(), expr.end());

        auto func = UserFunction();
        func.expr = tokenize(expr);
        func.args = args;

        user_functions[func_name] = func;
        return;
        }
    std::cout << "Invalid function definition" << std::endl;
}

void print_functions() {
    for (const auto& [name, value] : user_functions) {
        std::cout << name << "(";
        bool first = true;
        for (const auto &arg: value.args) {
            if (!first) std::cout << ", ";
            std::cout << arg;
            first = false;
        }
        std::cout << ") { ";
        for (const auto& token : value.expr) std::cout << token;
        std::cout << " }" << std::endl;
    }
}
