//
//  main.cpp
//  lab5
//
//  Created by Vlad Gordiichuk on 1/12/19.
//  Copyright © 2019 Vlad Gordiichuk. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "node.h"

using namespace std;

class interpreter {
    hash_map variables;
    ast Tree;
    string filename;
public:
    interpreter(string f) : filename(f) {};
    void build_ast();
    Node *convert_to_ast(string);
    int GetPriority(string);
    double calculate();
    double calculate(Node*);
    ~interpreter() { variables.clear(); }
    void SetFile(string);
};

void interpreter::SetFile(string f) {
    filename = f;
}

int interpreter::GetPriority(string cur_operator) {
    if (cur_operator == "+" || cur_operator == "-")
        return 2;
    if (cur_operator == "*" || cur_operator == "/")
        return 3;
    if (cur_operator == "=" || cur_operator == "==" || cur_operator == "!=" || cur_operator == "<=" || cur_operator == ">=" || cur_operator == ">" || cur_operator == "<")
        return 1;
    if (cur_operator == "^")
        return 4;
    if (cur_operator == "if" || cur_operator == ";")
        return 5;
    return 0;
}

Node *interpreter::convert_to_ast(string input) {
    ast tree;
    stack<string> operators;
    string loc_operator, last_operator, buff;
    int i = 0, loc_priority, p, pp;
    
    while (i <= input.size() - 1) {
        if (input.substr(i, 2) == "if") {
            Node *IF = new (Node);
            IF->Operator = true;
            IF->data = "if";
            
            p = input.find(")", i + 3);
            buff = input.substr(i + 3, p - i - 3);
            
            IF->If = convert_to_ast(buff);
            p += 2;
            i = p;
            p = input.find(")", i);
            
            pp = input.find(";", i);
            buff = input.substr(i, pp - i);
            i = pp + 1;
            
            while (i < p - 1) {
                IF->Left.push_back(convert_to_ast(buff));
                pp = input.find(";", i);
                buff = input.substr(i, pp - i);
                i = pp + 1;
            }
            IF->Left.push_back(convert_to_ast(buff));
            p = p + 3;
            i = p;
            p = input.find(")", i);
            
            i--;
            pp = input.find(";", i);
            buff = input.substr(i, pp - i);
            i = pp + 1;
            
            while (i < p - 1) {
                IF->Right.push_back(convert_to_ast(buff));
                pp = input.find(";", i);
                buff = input.substr(i, pp - i);
                i = pp + 1;
            }
            
            IF->Right.push_back(convert_to_ast(buff));
            tree.make_if(IF);
            i++;
            continue;
        }
        
        if (input[i] == ')') {
            last_operator = operators.top();
            while (last_operator != "(") {
                operators.pop();
                tree.add_operator(last_operator);
                last_operator = operators.top();
            }
            operators.pop();
            i++;
            continue;
        }
        
        if (input[i] == '(') {
            operators.push("(");
            i++;
            continue;
        }
        
        if (isalpha(input[i]) || isdigit(input[i])) {
            loc_operator = "";
            while (isalpha(input[i]) || isdigit(input[i]) || input[i] == '.') {
                loc_operator += input[i];
                i++;
            }
            tree.add_value(loc_operator);
        } else {
            if (input[i + 1] == '=') {
                loc_operator = input.substr(i, 2);
                i += 2;
            } else {
                loc_operator = input[i];
                i++;
            }
            
            if (loc_operator == "-") {
                if (i == 0 || (!isdigit(input[i - 2]) && !isalpha(input[i - 2]))) {
                    tree.add_value("-1");
                    operators.push("*");
                    continue;
                }
            }
            
            loc_priority = GetPriority(loc_operator);
            while (!operators.empty() && loc_priority <= GetPriority(operators.top())) {
                last_operator = operators.top();
                operators.pop();
                tree.add_operator(last_operator);
            }
            
            operators.push(loc_operator);
        }
    }
    
    while (!operators.empty()) {
        last_operator = operators.top();
        operators.pop();
        tree.add_operator(last_operator);
    }
    
    tree.complete();
    return tree.GetHead();
}

void interpreter::build_ast() {
    string line;
    ifstream myFile(filename);
    
    while (getline(myFile, line)) {
        line.erase(remove_if(line.begin(), line.end(), [](unsigned char x) {return std::isspace(x);}), line.end());
        
        if (myFile.eof())
            break;
        
        for (int i = 0; i < line.length(); i++) {
            if (line.at(i) == '=')
                variables.insert(pair<string, double>(line.substr(0, i), stof(line.substr(i + 1, line.length() - i - 2))));
        }
    }
    
    myFile.close();
    Tree.set_head(convert_to_ast(line));
}

double interpreter::calculate() {
    return calculate(Tree.GetHead());
}

double interpreter::calculate(Node* node) {
    if (node->Operator) {
        if (node->data == "+")
            return calculate(node->Right[0]) + calculate(node->Left[0]);
        
        if (node->data == "-")
            return calculate(node->Right[0]) - calculate(node->Left[0]);
        
        if (node->data == "*") {
            double r = calculate(node->Right[0]);
            if (r == 0)
                return 0;
            else
                return r * calculate(node->Left[0]);
        }
        
        if (node->data == "/") {
            double r = calculate(node->Right[0]);
            if (r == 0)
                return 0;
            
            double l = calculate(node->Left[0]);
            if (l == 0) {
                return 1000000000000;
            } else
                return r / l;
        }
        
        if (node->data == "^") {
            double r = calculate(node->Right[0]);
            
            if (r == 0 || r == 1)
                return r;
            else
                return pow(r, calculate(node->Left[0]));
        }
        
        if (node->data == "if") {
            if (calculate(node->If)) {
                for (int i = 0; i < node->Left.size() - 1; ++i)
                    calculate(node->Left[i]);
                double res = calculate(node->Left[node->Left.size() - 1]);
                return res;
            } else {
                for (int i = 0; i < node->Right.size() - 1; ++i)
                    calculate(node->Right[i]);
                return calculate(node->Right[node->Right.size() - 1]);
            }
        }
        
        if (node->data == "=") {
            double res = calculate(node->Left[0]);
            variables[node->Right[0]->data] = res;
            return 0;
        }
        
        if (node->data == "==")
            return calculate(node->Right[0]) == calculate(node->Left[0]);
        if (node->data == "!=")
            return calculate(node->Right[0]) != calculate(node->Left[0]);
        if (node->data == ">=")
            return calculate(node->Right[0]) >= calculate(node->Left[0]);
        if (node->data == "<=")
            return calculate(node->Right[0]) <= calculate(node->Left[0]);
        if (node->data == ">")
            return calculate(node->Right[0]) > calculate(node->Left[0]);
        if (node->data == "<")
            return calculate(node->Right[0]) < calculate(node->Left[0]);
    } else if (variables.find(node->data) == variables.end()) {
        return stof(node->data);
    } else  {
        return variables.find(node->data)->second;
    }
    
    return 0;
}


int main() {
    string file = "test.txt";
    interpreter R(file);
    R.build_ast();
    cout << "result = " << R.calculate() << endl;
    return 0;
}
