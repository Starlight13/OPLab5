//
//  main.cpp
//  lab5
//
//  Created by Vlad Gordiichuk on 2/12/19.
//  Copyright © 2019 Vlad Gordiichuk. All rights reserved.
//

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <stack>
typedef std::unordered_map<std::string, double>  hash_map;

using namespace std;


struct Node {
	bool Operator;
	string data;
	vector<Node *> Left, Right;
	Node *If;
};

class ast {
    Node *Head;
    stack<Node*> nodes;
    int tabs;
public:
    ast() : tabs(0), Head(nullptr) {};
    void add_value(string);
    void add_operator(string);
    void make_if(Node *);
    Node *GetHead();
    void set_head(Node *);
    void complete();
};

void ast::add_value(string data) {
    Node *t = new Node;
    t->Operator = false;
    t->data = data;
    t->If = nullptr;
    t->Right.clear();
    t->Left.clear();
    nodes.push(t);
}

void ast::add_operator(string data) {
    Node *l, *r, *t;
    l = nodes.top();
    nodes.pop();
    r = nodes.top();
    nodes.pop();
    t = new Node;
    t->Operator = true;
    t->data = data;
    t->Right.push_back(r);
    t->Left.push_back(l);
    nodes.push(t);
}

void ast::make_if(Node *IF) {
    nodes.push(IF);
}

void ast::complete() {
    Head = nodes.top();
}

Node *ast::GetHead() {
    return Head;
}

void ast::set_head(Node *new_head) {
    Head = new_head;
}

#endif
