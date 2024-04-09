#ifndef PROJ_H
#define PROJ_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip> 

using namespace std;

struct Instruction
{

    unsigned long instruction_address;
    int cycle_inserted;
    int instructionType; // 1 Integer, 2 Floating, 3 branch, 4 load, 5 store, 6 dummy node
    unsigned long instruction_dependency[3]; // 0 if none, else the instruction number it is depended on.
    struct Instruction *next;
    struct Instruction *prev;

};

struct InstructionQueue
{

    struct Instruction *head;
    struct Instruction *tail;
    int count;//number of instructions 

};

struct Address
{
    unsigned long address;
    struct Address *next;
    struct Address *prev;
};

struct AddressQueue
{
    struct Address *head;
    struct Address *tail;
};

struct Pipeline
{

    struct InstructionQueue *stall_queue;
    struct InstructionQueue *IF_queue;
    struct InstructionQueue *ID_queue;
    struct InstructionQueue *EX_queue;
    struct InstructionQueue *MEM_queue;
    struct InstructionQueue *WB_queue;

    struct AddressQueue *finsh_address_queue;

    unsigned long latest_instruction_address_finished;

    int cycle_count; //current cycle 

    int finish_count;//number of instructions finished

    int integer_count;
    int floating_count;
    int branch_count;
    int load_count;
    int store_count;
    
};

struct Pipeline *InitalizePipeline(int width);
struct Instruction *NewInstruction(unsigned long address, int cycle_count, int type,
unsigned long dependency1, unsigned long dependency2, unsigned long dependency3);
void Insert_Queue(struct InstructionQueue *InstructionQueue, struct Instruction *Instruction);
void Insert_Address(struct AddressQueue *address_queue, unsigned long instruction_address);
void Delete_Address(struct AddressQueue *address_queue, unsigned long instruction_address);
unsigned long Delete_WB_Instruction(struct InstructionQueue *InstructionQueue);
void Delete_Instruction(struct InstructionQueue *InstructionQueue);
void ProcessWB(struct Pipeline *Pipeline, int width);
void ProcessMEM(struct Pipeline *Pipeline, int width);
void ProcessEX(struct Pipeline *Pipeline, int width);
void ProcessID(struct Pipeline *Pipeline, int width);
void ProcessIF(struct Pipeline *Pipeline, int width);
void Simulate_Cycle(struct Pipeline *Pipeline, int width);
bool isBranchin_IF_ID_EX(struct Pipeline *Pipeline);
void FreePipeline(struct Pipeline *Pipeline);
void PrintStatistics(struct Pipeline *Pipeline, int width, int starting_instruction, int simulating_instruction, string filename);

#endif