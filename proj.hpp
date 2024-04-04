#ifndef proj_H
#define proj_H

typedef struct instruction{

    enum class instructionType {IF, ID, EX, MEM, WB};
    int instruction_dependency; // 0 if none, else the instruction number it is depended on.
    Instruction *next;
    Instruction *prev;

} Instruction;

typedef struct instructionQueue{

    Instruction *head;
    Instruction *tail;

} InstructionQueue;

typedef struct pipeline{

    InstructionQueue *IF_queue;
    InstructionQueue *ID_queue;
    InstructionQueue *EX_queue;
    InstructionQueue *MEM_queue;
    InstructionQueue *WB_queue;

    int finish_count;
    
} Pipeline;


#endif