#ifndef PROJ_H
#define PROJ_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

struct Instruction
{

    unsigned long instruction_address;
    int cycle_inserted;
    int instructionType; // 1 Integer, 2 Floating, 3 branch, 4 load, 5 store, 6 dummy node
    unsigned long instruction_dependency[2]; // 0 if none, else the instruction number it is depended on.
    struct Instruction *next;
    struct Instruction *prev;

};

struct InstructionQueue
{

    struct Instruction *head;
    struct Instruction *tail;
    int count;//number of instructions 

};

struct Pipeline
{

    struct InstructionQueue *stall_queue;
    struct InstructionQueue *IF_queue;
    struct InstructionQueue *ID_queue;
    struct InstructionQueue *EX_queue;
    struct InstructionQueue *MEM_queue;
    struct InstructionQueue *WB_queue;

    unsigned long latest_instruction_address_finished;

    int instructions_count; // number of instructions in the whole pipeline

    int cycle_count; //current cycle 

    int finish_count;//number of instructions finished

    int integer_count;
    int floating_count;
    int branch_count;
    int load_count;
    int store_count;
    
};

struct Pipeline *InitalizePipeline(int width);
struct Instruction *NewInstruction(unsigned long address, int cycle_count, int type, unsigned long dependency1, unsigned long dependency2) ;
void Insert_Queue(struct InstructionQueue *InstructionQueue, struct Instruction *Instruction);
unsigned long Delete_WB_Instruction(struct InstructionQueue *InstructionQueue);
void Delete_Instruction(struct InstructionQueue *InstructionQueue);
void ProcessWB(struct Pipeline *Pipeline, int width);
void ProcessMEM(struct Pipeline *Pipeline, int width);
void ProcessEX(struct Pipeline *Pipeline, int width);
void ProcessID(struct Pipeline *Pipeline, int width);
void ProcessIF(struct Pipeline *Pipeline, int width);
void Simulate_Cycle(struct Pipeline *Pipeline, int width);
bool isBranchin_IF_ID_EX(struct Pipeline *Pipeline);

#endif