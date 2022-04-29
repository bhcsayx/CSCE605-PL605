#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "../IR/IR.h"
#include "../IR/SSA.h"

#include "Dot.h"

using namespace std;

void Dot::addNode(string name) {
    Node n = Node();
    n.name = name;
    nodes.insert({name, n});
}

void Dot::addCFGEdge(string n1, string n2, string kind) {
    Edge e(n1, n2);
    e.type = "CFG";
    e.color = "";
    e.style = "";
    e.label = kind;
    edges.push_back(e);
}

void Dot::addDomEdge(string n1, string n2) {
    Edge e(n1, n2);
    e.type = "Dom";
    e.color = "blue";
    e.style = "dotted";
    e.label = "Dom";
    edges.push_back(e);
}

void Dot::addDFEdge(string n1, string n2) {
    Edge e(n1, n2);
    e.type = "DF";
    e.color = "green";
    e.style = "dotted";
    e.label = "DF";
    edges.push_back(e);
}

void Dot::addInstruction(string name, Instruction* ins) {
    Node node = nodes[name];
    string res;

    res.append(to_string(counter++));
    res.append(": ");
    // res.append(ins->dest->name.substr(1, ins->dest->name.length()));
    // res.append(": ");
    
    switch (ins->opcode) {
        case OpCode::NEG: {
            res.append("NEG ");
            break;
        }
        case OpCode::ADD: {
            res.append("ADD ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::SUB: {
            res.append("SUB ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::MUL: {
            res.append("MUL ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::DIV: {
            res.append("DIV ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::CMP: {
            res.append("CMP ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else {
                res.append(ins->op1->name.c_str());
            }
                
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::MOVE: {
            res.append("MOVE ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            // else if(ins->op1->type == Type::insRes) {
            //     res.append("%");
            //     res.append(to_string(ins->op1->index));
            // }
            else {
                res.append(ins->op1->name.c_str());
            }
            res.append(" ");
            res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::PHI: {
            res.append("PHI ");
            res.append(ins->op1->name.c_str());
            res.append(" ");
            res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::END: {
            res.append("END ");
            break;
        }
        case OpCode::BRA: {
            res.append("BRA [");
            res.append(to_string(ins->op1->value));
            res.append("]");
            break;
        }
        case OpCode::BEQ: {
            res.append("BEQ [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BNE: {
            res.append("BNE [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BLT: {
            res.append("BLT [");
            // res.append(ins->op2->name);
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BGT: {
            res.append("BGT [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BLE: {
            res.append("BLE [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BGE: {
            res.append("BGE [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::READ: {
            res.append("READ ");
            break;
        }
        case OpCode::WRITE: {
            res.append("WRITE ");
            res.append(ins->op1->name);
            break;
        }
        case OpCode::WRITENL: {
            res.append("WRITENL ");
            break;
        }
        case OpCode::CALL: {
            res.append("CALL ");
            res.append(ins->op1->name);
            for(auto arg: ins->callArgs) {
                res.append(" ");
                if(arg->type == Type::constVal) {
                    res.append("#");
                    res.append(to_string(arg->value));
                }
                else
                    res.append(arg->name);
            }
            break;
        }
        case OpCode::RET: {
            res.append("RET ");
            if(ins->op1->type != Type::empty) {
                if(ins->op1->type == Type::constVal) {
                    res.append("#");
                    res.append(to_string(ins->op1->value));
                }
            else
                res.append(ins->op1->name.c_str());
            }
            break;
        }
        
    }

    // if(ins->op1->type == Type::constVal)
    //     res.append(to_string(ins->op1->value));
    // else
    //     res.append(ins->op1->name.c_str());

    // res.append(" ");
    // if(ins->op2->type == Type::constVal)
    //     res.append(to_string(ins->op2->value));
    // else
    //     res.append(ins->op2->name.c_str());
    res.append("|");

    nodes[name].label.append(res.c_str());
}

void Dot::dump(string filename) {
    ofstream outfile;
    outfile.open(filename.c_str(), ios::out);

    outfile << "diGraph G { " << endl;
    for(auto n: nodes) {
        outfile << n.second.name << " [";
        outfile << "shape=" << n.second.shape << ", ";
        outfile << "label=\"<b>" << n.second.name << "| {";
        outfile << n.second.label.substr(0, n.second.label.length()-1) << "}\"];" << endl;
    }

    outfile << endl;

    for(auto e: edges) {
        if(e.type == "CFG") {
            outfile << e.start << ":s -> ";
            outfile << e.end << ":n [label=\"";
            outfile << e.label << "\"];\n";
        }
        if(e.type == "Dom") {
            outfile << e.start << ":b -> ";
            outfile << e.end << ":b [color=";
            outfile << e.color << ", style=";
            outfile << e.style << ", label=\"";
            outfile << e.label << "\"];\n";
        }
        if(e.type == "DF") {
            outfile << e.start << ":b -> ";
            outfile << e.end << ":b [color=";
            outfile << e.color << ", style=";
            outfile << e.style << ", label=\"";
            outfile << e.label << "\"];\n";
        }
    }
    outfile << endl << "\n}";
}