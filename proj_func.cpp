#include "proj.hpp"
#include <algorithm>

// ------------Global variables----------------------------------------------------------------------
// Feel free to add or remove. 
// You could make these local variables for cleaner code, but you need to change function definitions 
int int_ALU = 0; // integer alu vancany. 0 is not being used else it is 1 if it is.
int ftp_ALU = 0; // floating point alu vancancy. 0 is empty, 1 is using.
int branch_ex = 0; // branch execution if being used is 1 else is 0
int storing_port = 0; // write port 0 if not being used 1 if being used.
int loading_port = 0; // read port 0 if not being used 1 if being used.

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
        Insert_Queue(pipeline->ID_queue, NewInstruction(0x0, 6, -1, 0x0, 0x0));
    }

    pipeline->EX_queue = new struct InstructionQueue;
    pipeline->EX_queue->head = NULL;
    pipeline->EX_queue->tail = NULL;
    pipeline->EX_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->EX_queue, NewInstruction(0x0, 6, -1, 0x0, 0x0));
    }

    pipeline->MEM_queue = new struct InstructionQueue;
    pipeline->MEM_queue->head = NULL;
    pipeline->MEM_queue->tail = NULL;
    pipeline->MEM_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->MEM_queue, NewInstruction(0x0, 6, -1, 0x0, 0x0));
    }

    pipeline->WB_queue = new struct InstructionQueue;
    pipeline->WB_queue->head = NULL;
    pipeline->WB_queue->tail = NULL;
    pipeline->WB_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->WB_queue, NewInstruction(0x0, 6, -1, 0x0, 0x0));
    }

    pipeline->cycle_count = 0;
    pipeline->latest_instruction_address_finished = 0x0;
    pipeline->finish_count = 0;
    pipeline->instructions_count = 0;

    pipeline->integer_count = 0;
    pipeline->floating_count = 0;
    pipeline->branch_count = 0;
    pipeline->load_count = 0;
    pipeline->store_count = 0;

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

        if (Pipeline->WB_queue->head->instructionType != 6) {

            if (Pipeline->WB_queue->head->instructionType == 1) {

                Pipeline->integer_count++;

            } else  if (Pipeline->WB_queue->head->instructionType == 2) {
                
                Pipeline->floating_count++;

            } else  if (Pipeline->WB_queue->head->instructionType == 3) {
                
                Pipeline->branch_count++;

            } else  if (Pipeline->WB_queue->head->instructionType == 4) {
                
                Pipeline->load_count++;

            } else {

                Pipeline->store_count++;
            }

            Pipeline->latest_instruction_address_finished = Delete_WB_Instruction(Pipeline->WB_queue);
            Pipeline->finish_count++;
            
        } else {

            Delete_Instruction(Pipeline->WB_queue);
        }
    }
}

void ProcessMEM(struct Pipeline *Pipeline, int width)
{

    while ((Pipeline->MEM_queue->count != 0 ) && (Pipeline->WB_queue->count != width)) 
    {
        if (Pipeline->MEM_queue->head->instructionType == 4) {

            storing_port = 0;

        } else if (Pipeline->MEM_queue->head->instructionType == 5) {

            loading_port = 0;
        }
        
        Insert_Queue(Pipeline->WB_queue, NewInstruction(Pipeline->MEM_queue->head->instruction_address, 
        Pipeline->MEM_queue->head->cycle_inserted, Pipeline->MEM_queue->head->instructionType,
         Pipeline->MEM_queue->head->instruction_dependency[0], Pipeline->MEM_queue->head->instruction_dependency[1]));
        
        Delete_Instruction(Pipeline->MEM_queue);
    }
}

void ProcessEX(struct Pipeline *Pipeline, int width)
{
     while ((Pipeline->EX_queue->count != 0) && Pipeline->MEM_queue->count != width)
    {   
        if (((Pipeline->EX_queue->head->instructionType == 1)) ||
         ((Pipeline->EX_queue->head->instructionType == 2)) || 
         ((Pipeline->EX_queue->head->instructionType == 3))
         || ((Pipeline->EX_queue->head->instructionType == 4) && (storing_port == 0)) 
         || ((Pipeline->EX_queue->head->instructionType == 5) && (loading_port == 0)) 
         || (Pipeline->EX_queue->head->instructionType == 6)) {

            if (Pipeline->EX_queue->head->instructionType == 1) {

                int_ALU = 0;

            } else if (Pipeline->EX_queue->head->instructionType == 2) {

                ftp_ALU = 0;

            } else if (Pipeline->EX_queue->head->instructionType == 4) {

                storing_port = 1;

            } else if (Pipeline->EX_queue->head->instructionType == 5) {

                loading_port = 1;
            }

            Insert_Queue(Pipeline->MEM_queue, NewInstruction(Pipeline->EX_queue->head->instruction_address, 
            Pipeline->EX_queue->head->cycle_inserted, Pipeline->EX_queue->head->instructionType,
            Pipeline->EX_queue->head->instruction_dependency[0], Pipeline->EX_queue->head->instruction_dependency[1]));

            Delete_Instruction(Pipeline->EX_queue);

        } else {

            break;
        }
    }

}

void ProcessID(struct Pipeline *Pipeline, int width)
{
     while ((Pipeline->ID_queue->count != 0) && Pipeline->EX_queue->count != width)
    {   
        // Assuming because it is an In-Order Pipeline...
        if (((Pipeline->ID_queue->head->instructionType == 1) && (int_ALU == 0)) ||
         ((Pipeline->ID_queue->head->instructionType == 2) && (ftp_ALU == 0)) || 
         ((Pipeline->ID_queue->head->instructionType == 3) && (branch_ex == 0))
         || (Pipeline->ID_queue->head->instructionType == 4) || (Pipeline->ID_queue->head->instructionType == 5) ||
         (Pipeline->ID_queue->head->instructionType == 6)) {

            // make it busy.
            if (Pipeline->ID_queue->head->instructionType == 1) {

                int_ALU = 1;

            } else if (Pipeline->ID_queue->head->instructionType == 2) {

                ftp_ALU = 1;

            } else if (Pipeline->ID_queue->head->instructionType == 3) {

                branch_ex = 1;
            }

            Insert_Queue(Pipeline->EX_queue, NewInstruction(Pipeline->ID_queue->head->instruction_address, 
            Pipeline->ID_queue->head->cycle_inserted, Pipeline->ID_queue->head->instructionType,
            Pipeline->ID_queue->head->instruction_dependency[0], Pipeline->ID_queue->head->instruction_dependency[1]));
    
            Delete_Instruction(Pipeline->ID_queue);

        } else { // stall

            break;
        }
    }

}


void ProcessIF(struct Pipeline *Pipeline, int width)
{

    while ((Pipeline->IF_queue->count != 0))
    {
        if ((std::max(Pipeline->IF_queue->head->instruction_dependency[0], Pipeline->IF_queue->head->instruction_dependency[1]) == 0)||(std::max(Pipeline->IF_queue->head->instruction_dependency[0], Pipeline->IF_queue->head->instruction_dependency[1]) <= (Pipeline->latest_instruction_address_finished)))//no depndencies or denpendencies are finished
        {   
            struct Instruction* newInstruction= NewInstruction(Pipeline->IF_queue->head->instruction_address, 
            Pipeline->IF_queue->head->cycle_inserted, Pipeline->IF_queue->head->instructionType, 
            Pipeline->IF_queue->head->instruction_dependency[0], Pipeline->IF_queue->head->instruction_dependency[1]);

            Insert_Queue(Pipeline->ID_queue,newInstruction);
            Delete_Instruction(Pipeline->IF_queue);
        } else {
            break;
        }

    }
}



void Simulate_Cycle(struct Pipeline *Pipeline, int width){

    ProcessWB(Pipeline, width);
    ProcessMEM(Pipeline, width);
    ProcessEX(Pipeline, width);
    ProcessID(Pipeline, width);
    ProcessIF(Pipeline, width);

    Pipeline->cycle_count++;
    
}
bool isBranchin_IF_ID_EX(struct Pipeline *Pipeline)
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