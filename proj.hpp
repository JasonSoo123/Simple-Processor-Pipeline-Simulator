#ifndef PROJ_H
#define PROJ_H

#include <fstream>
#include <iostream>
#include <string>

struct Instruction{

    enum class instructionType {IF, ID, EX, MEM, WB};
    int instruction_dependency; // 0 if none, else the instruction number it is depended on.
    struct Instruction *next;
    struct Instruction *prev;

};

struct InstructionQueue{

    struct Instruction *head;
    struct Instruction *tail;

};

struct Pipeline{

    struct InstructionQueue *IF_queue;
    struct InstructionQueue *ID_queue;
    struct InstructionQueue *EX_queue;
    struct InstructionQueue *MEM_queue;
    struct InstructionQueue *WB_queue;

    int finish_count;
    
};


#endif