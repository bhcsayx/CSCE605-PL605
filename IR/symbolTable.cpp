#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "symbolTable.h"

symbolTable::symbolTable() {
    std::map<string, int> first;
    self.table.push_back(first);
}

symbolTable::~symbolTable() {}

void symbolTable::newScope() {
    std::map<string, int> previous, _new;
    previous = self.table.back();
    for(iter = self.table.begin(); iter != self.table.end(); iter++) {
        _new.insert({iter->first, iter->second});
    }
    self.table.push_back(_new);
}

void symbolTable::exitScope() {
    self.table.pop();
}

void insertSymbol(string name, int value) {
    std::map<string, int> cur = self.table.back();
    std::map<string, int> ::iterator find_res = cur.find(name);
    if(find_res != cur.end())
        cur.erase(find_res);
    cur.insert({name, value});
}

int lookupSymbol(string name) {
    std::map<string, int> cur = self.table.back();
    std::map<string, int> ::iterator find_res = cur.find(name);
    if(find_res != cur.end())
        return find_res->second;
    else
        return -2;
}