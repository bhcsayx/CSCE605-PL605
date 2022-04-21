#ifndef GRAPHVIZ_H
#define GRAPHVIZ_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../IR/IR.h"

using namespace std;

string val2txt(Value val);
string op2txt(enum OpCode op);

vector<string> dump2txt(Module mod);
// void dump2dot(Module mod);

#endif