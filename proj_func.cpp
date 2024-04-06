#include "proj.hpp"

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

struct Pipeline *InitalizePipeline(int width){

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
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->ID_queue, NewInstruction(0x0, 6, 0x0, 0x0));
    }

    pipeline->EX_queue = new struct InstructionQueue;
    pipeline->EX_queue->head = NULL;
    pipeline->EX_queue->tail = NULL;
    pipeline->EX_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->EX_queue, NewInstruction(0x0, 6, 0x0, 0x0));
    }

    pipeline->MEM_queue = new struct InstructionQueue;
    pipeline->MEM_queue->head = NULL;
    pipeline->MEM_queue->tail = NULL;
    pipeline->MEM_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->MEM_queue, NewInstruction(0x0, 6, 0x0, 0x0));
    }

    pipeline->WB_queue = new struct InstructionQueue;
    pipeline->WB_queue->head = NULL;
    pipeline->WB_queue->tail = NULL;
    pipeline->WB_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->WB_queue, NewInstruction(0x0, 6, 0x0, 0x0));
    }

    pipeline->latest_instruction_address_finished = 0x0;
    pipeline->finish_count = 0;
    pipeline->instructions_count = 0;

    return pipeline;
}

struct Instruction *NewInstruction(unsigned long address, int cycle_count, int type,
 unsigned long dependency1, unsigned long dependency2) {

    struct Instruction *newInstruction = new struct Instruction;
    newInstruction->instruction_address = address;
    newInstruction->cycle_inserted = cycle_count;
    newInstruction->instructionType = type;
    newInstruction->instruction_dependency[0] = dependency1;
    newInstruction->instruction_dependency[1] = dependency2;
    newInstruction->next = NULL;
    newInstruction->prev = NULL;

    return newInstruction;

 }

unsigned long Delete_WB_Instruction(struct InstructionQueue *InstructionQueue) {

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

void Delete_Instruction(struct InstructionQueue *InstructionQueue){

    struct Instruction *toDelete = InstructionQueue->head;
    InstructionQueue->head = InstructionQueue->head->next;

    if (InstructionQueue->head != NULL) {

        InstructionQueue->head->prev = NULL;
    } else {

        InstructionQueue->tail = NULL;
    }

    delete toDelete;
    InstructionQueue->count--;
}

void ProcessWB(struct Pipeline *Pipeline, int width){

    while (Pipeline->WB_queue->count != 0) {

        Pipeline->latest_instruction_address_finished = Delete_WB_Instruction(Pipeline->WB_queue);
        Pipeline->finish_count++;
    }
}

void ProcessMEM(struct Pipeline *Pipeline, int width)
{

    while ((Pipeline->MEM_queue->count != 0 ) && (Pipeline->WB_queue->count != width)) 
    {

        Insert_Queue(Pipeline->WB_queue, NewInstruction(Pipeline->MEM_queue->head->instruction_address, 
        Pipeline->MEM_queue->head->cycle_inserted, Pipeline->MEM_queue->head->instructionType,
         Pipeline->MEM_queue->head->instruction_dependency[0], Pipeline->MEM_queue->head->instruction_dependency[1]));
        
        Delete_Instruction(Pipeline->MEM_queue);
    }
}

void ProcessEX(struct Pipeline *Pipeline, int width)
{
     while ((Pipeline->EX_queue->count != 0))
    {
        Insert_Queue(Pipeline->MEM_queue, NewInstruction(Pipeline->EX_queue->head->instruction_address, 
        Pipeline->EX_queue->head->cycle_inserted, Pipeline->EX_queue->head->instructionType,
         Pipeline->EX_queue->head->instruction_dependency[0], Pipeline->EX_queue->head->instruction_dependency[1]));
    
        Delete_Instruction(Pipeline->EX_queue);
    }

}

void ProcessID(struct Pipeline *Pipeline, int width)
{
     while ((Pipeline->ID_queue->count != 0))
    {
        Insert_Queue(Pipeline->EX_queue, NewInstruction(Pipeline->ID_queue->head->instruction_address, 
        Pipeline->ID_queue->head->cycle_inserted, Pipeline->ID_queue->head->instructionType,
         Pipeline->ID_queue->head->instruction_dependency[0], Pipeline->ID_queue->head->instruction_dependency[1]));
    
        Delete_Instruction(Pipeline->ID_queue);
     }

}

void ProcessIF(struct Pipeline *Pipeline, int width)
{

    while ((Pipeline->IF_queue->count != 0))
    {
        Insert_Queue(Pipeline->ID_queue, NewInstruction(Pipeline->IF_queue->head->instruction_address, 
        Pipeline->IF_queue->head->cycle_inserted, Pipeline->IF_queue->head->instructionType,
         Pipeline->IF_queue->head->instruction_dependency[0], Pipeline->IF_queue->head->instruction_dependency[1]));
    
        Delete_Instruction(Pipeline->IF_queue);
    }
}



int Simulate_Cycle(struct Pipeline *Pipeline, int cycle_count, int width){

    ProcessWB(Pipeline, width);
    ProcessMEM(Pipeline, width);
    ProcessEX(Pipeline, width);
    ProcessID(Pipeline, width);
    ProcessIF(Pipeline, width);

    return cycle_count++;
    
}
bool isBranchin_IF_ID_EX(struct Pipeline *Pipeline, int width)
{
    struct Instruction *current = Pipeline->IF_queue->head;
    while (current != NULL) 
    {
        if (current->instructionType == 3) 
        {
            return true;
        }
        current = current->next;
    }

    current = Pipeline->ID_queue->head;
    while (current != NULL) 
    {
        if (current->instructionType == 3) 
        {
            return true;
        }
        current = current->next;
    }

    current = Pipeline->EX_queue->head;
    while (current != NULL) 
    {
        if (current->instructionType == 3) 
        {
            return true;
        }
        current = current->next;
    }
    
    return false;
}