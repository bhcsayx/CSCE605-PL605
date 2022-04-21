#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "symbolTable.h"

symbolTable::symbolTable() {
    std::map<string, int> first;
    table.push_back(first);
}

symbolTable::~symbolTable() {}

void symbolTable::newScope() {
    std::map<string, int> previous, _new;
    previous = table.back();
    for(std::map<string, int> ::iterator iter = previous.begin(); iter != previous.end(); iter++) {
        _new.insert({iter->first, iter->second});
    }
    table.push_back(_new);
    // printf("new scope success\n");
}

void symbolTable::exitScope() {
    table.pop_back();
}

void symbolTable::insertSymbol(string name, int value) {
    std::map<string, int>& cur = table.back();
    std::map<string, int> ::iterator find_res = cur.find(name);
    if(find_res != cur.end())
        cur.erase(find_res);
    cur.insert({name, value});

}

int symbolTable::lookupSymbol(string name) {
    // printf("start...\n");
    auto test = table[0];
    // printf("lookup start: %d\n", 0);
    auto cur = table.back();
    // printf("cur get\n");
    std::map<string, int> ::iterator find_res = cur.find(name);
    if(find_res != cur.end())
        return find_res->second;
    else
        return -2;
}