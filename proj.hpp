#ifndef PROJ_H
#define PROJ_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

struct Instruction{

    unsigned long instruction_address;
    int cycle_inserted;
    int instructionType; // 1 Integer, 2 Floating, 3 branch, 4 load, 5 store
    unsigned long instruction_dependency[2]; // 0 if none, else the instruction number it is depended on.
    struct Instruction *next;
    struct Instruction *prev;

};

struct InstructionQueue{

    struct Instruction *head;
    struct Instruction *tail;
    int count;//number of instructions 

};

struct Pipeline{

    struct InstructionQueue *stall_queue;
    struct InstructionQueue *IF_queue;
    struct InstructionQueue *ID_queue;
    struct InstructionQueue *EX_queue;
    struct InstructionQueue *MEM_queue;
    struct InstructionQueue *WB_queue;

    unsigned long latest_instruction_address_finished;

    int instructions_count; // instructions in the whole pipeline

    int finish_count;
    
};

struct Pipeline *InitalizePipeline(int width);
struct Instruction *NewInstruction(unsigned long address, int type, unsigned long dependency1, unsigned long dependency2);
void Insert_Queue(struct InstructionQueue *InstructionQueue, struct Instruction *Instruction);
unsigned long Delete_WB_Instruction(struct InstructionQueue *InstructionQueue);
void Delete_Instruction(struct InstructionQueue *InstructionQueue);
void ProcessWB(struct Pipeline *Pipeline, int width);
void ProcessMEM(struct Pipeline *Pipeline, int width);
void ProcessEX(struct Pipeline *Pipeline, int width);
void ProcessID(struct Pipeline *Pipeline, int width);
void ProcessIF(struct Pipeline *Pipeline, int width);
int Simulate_Cycle(struct Pipeline *Pipeline, int cycle_count, int width);
bool isBranchin_IF_ID_EX(struct Pipeline *Pipeline);

#endif