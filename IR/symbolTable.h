#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

class symbolTable {
public:
    std::vector<std::map<string, int>> table;
    symbolTable();
    ~symbolTable();
    void newScope();
    void exitScope();
    void insertSymbol(string name, int value);
    int lookupSymbol(string name);
};

#endif