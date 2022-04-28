#ifndef DOT_H
#define DOT_H

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "../IR/IR.h"
#include "../IR/SSA.h"

using namespace std;

class Node {
public:
    string name;
    string shape = "record";
    string label;

    Node() = default;
    ~Node() = default;
};

class Edge {
public:
    string start, end;
    string color, style, label, type;

    Edge() = default;
    ~Edge() = default;
    Edge(string n1, string n2) {start = n1; end = n2;}
};

class Dot {
public:
    map<string, Node> nodes;
    vector<Edge> edges;
    int counter = 0;

    Dot() = default;
    ~Dot() = default;

    void addNode(string name);
    void addCFGEdge(string n1, string n2, string kind);
    void addDomEdge(string n1, string n2);
    void addDFEdge(string n1, string n2);

    void addInstruction(string name, Instruction* ins);

    void dump(string filename);
};

#endif