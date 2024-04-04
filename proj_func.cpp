#include "proj.hpp"

struct Pipeline *InitalizePipeline(){

    struct Pipeline *pipeline = new struct Pipeline;

    pipeline->stall_queue = new struct InstructionQueue;
    pipeline->stall_queue->head = NULL;
    pipeline->stall_queue->tail = NULL;
    pipeline->stall_queue->count = 0;

    pipeline->IF_queue = new struct InstructionQueue;
    pipeline->IF_queue->head = NULL;
    pipeline->IF_queue->tail = NULL;
    pipeline->IF_queue->count = 0;

    pipeline->ID_queue = new struct InstructionQueue;
    pipeline->ID_queue->head = NULL;
    pipeline->ID_queue->tail = NULL;
    pipeline->ID_queue->count = 0;

    pipeline->EX_queue = new struct InstructionQueue;
    pipeline->EX_queue->head = NULL;
    pipeline->EX_queue->tail = NULL;
    pipeline->EX_queue->count = 0;

    pipeline->MEM_queue = new struct InstructionQueue;
    pipeline->MEM_queue->head = NULL;
    pipeline->MEM_queue->tail = NULL;
    pipeline->MEM_queue->count = 0;

    pipeline->WB_queue = new struct InstructionQueue;
    pipeline->WB_queue->head = NULL;
    pipeline->WB_queue->tail = NULL;
    pipeline->WB_queue->count = 0;

    pipeline->latest_instruction_address_finished = 0x0;
    pipeline->finish_count = 0;

    return pipeline;
}

struct Instruction *NewInstruction(unsigned long address, int type,
 unsigned long dependency1, unsigned long dependency2) {

    struct Instruction *newInstruction = new struct Instruction;
    newInstruction->instruction_address = address;
    newInstruction->instructionType = type;
    newInstruction->instruction_dependency[0] = dependency1;
    newInstruction->instruction_dependency[1] = dependency2;
    newInstruction->next = NULL;
    newInstruction->prev = NULL;

    return newInstruction;

 }

void Insert_Queue(struct InstructionQueue *InstructionQueue, struct Instruction *Instruction) {

    if (InstructionQueue->head == NULL) {

        InstructionQueue->head = Instruction;
        InstructionQueue->tail = Instruction;

    } else {
       
        InstructionQueue->tail->next = Instruction;
        Instruction->prev = InstructionQueue->tail;
        InstructionQueue->tail = Instruction;
    }

    InstructionQueue->count++;
}

unsigned long Delete_Instruction(struct InstructionQueue *InstructionQueue) {

    struct Instruction *toDelete = InstructionQueue->head;
    InstructionQueue->head = InstructionQueue->head->next;

    if (InstructionQueue->head != NULL) {

        InstructionQueue->head->prev = NULL;
    } else {

        InstructionQueue->tail = NULL;
    }

    unsigned long address = toDelete->instruction_address;

    delete toDelete;
    InstructionQueue->count--;

    return address;
}

void ProcessWB(struct Pipeline *Pipeline, int width){

    while (Pipeline->WB_queue->count != 0) {

        Pipeline->latest_instruction_address_finished = Delete_Instruction(Pipeline->WB_queue);
        Pipeline->finish_count++;
    }
}

void ProcessMEM(struct Pipeline *Pipeline, int width){


}

void ProcessEX(struct Pipeline *Pipeline, int width){


}

void ProcessID(struct Pipeline *Pipeline, int width){


}

void ProcessIF(struct Pipeline *Pipeline, int width){


}
