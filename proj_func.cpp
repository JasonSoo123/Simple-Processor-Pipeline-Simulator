#include "proj.hpp"
using namespace std;

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

void Insert_Address(struct AddressQueue *address_queue, unsigned long instruction_address) {

    struct Address *newAddress = new struct Address;
    newAddress->address = instruction_address;
    newAddress->next = NULL;

    if (address_queue->head == NULL) {

        address_queue->head = newAddress;
        address_queue->tail = newAddress;

    } else {
        // Find the correct position to insert the new address
        struct Address *current = address_queue->head;
        struct Address *prev = NULL;

        while (current != NULL && current->address < instruction_address) {
            prev = current;
            current = current->next;
        }

        if (prev == NULL) {

            newAddress->next = address_queue->head;
            address_queue->head = newAddress;

        } else {

            prev->next = newAddress;
            newAddress->next = current;
        }
        if (current == NULL) {
            address_queue->tail = newAddress;
        }
    }
}

void Delete_Address(struct AddressQueue *address_queue, unsigned long instruction_address) {

    if (address_queue->head == NULL) {

        return;
    }

    if (address_queue->head->address == instruction_address) {

        struct Address *temp = address_queue->head;
        address_queue->head = address_queue->head->next;
        delete temp;

        if (address_queue->head == NULL) {

            address_queue->tail = NULL;
        }

        return;
    }

    struct Address *current = address_queue->head;
    struct Address *prev = NULL;
    while (current != NULL && current->address != instruction_address) {

        prev = current;
        current = current->next;
    }

    if (current == NULL) {

        return;
    }

    prev->next = current->next;
    delete current;

    if (prev->next == NULL) {

        address_queue->tail = prev;
    }
}

struct Pipeline *InitalizePipeline(int width){

    struct Pipeline *pipeline = new struct Pipeline;

    pipeline->stall_queue = new struct InstructionQueue;
    pipeline->stall_queue->head = NULL;
    pipeline->stall_queue->tail = NULL;
    pipeline->stall_queue->count = 0;

    pipeline->finsh_address_queue = new struct AddressQueue;
    pipeline->finsh_address_queue->head = NULL;
    pipeline->finsh_address_queue->tail = NULL;
    Insert_Address(pipeline->finsh_address_queue, 0x0); // dummy address where dummies can work

    pipeline->IF_queue = new struct InstructionQueue;
    pipeline->IF_queue->head = NULL;
    pipeline->IF_queue->tail = NULL;
    pipeline->IF_queue->count = 0;

    pipeline->ID_queue = new struct InstructionQueue;
    pipeline->ID_queue->head = NULL;
    pipeline->ID_queue->tail = NULL;
    pipeline->ID_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->ID_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0));
    }

    pipeline->EX_queue = new struct InstructionQueue;
    pipeline->EX_queue->head = NULL;
    pipeline->EX_queue->tail = NULL;
    pipeline->EX_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->EX_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0));
    }

    pipeline->MEM_queue = new struct InstructionQueue;
    pipeline->MEM_queue->head = NULL;
    pipeline->MEM_queue->tail = NULL;
    pipeline->MEM_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->MEM_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0));
    }

    pipeline->WB_queue = new struct InstructionQueue;
    pipeline->WB_queue->head = NULL;
    pipeline->WB_queue->tail = NULL;
    pipeline->WB_queue->count = 0;
    for (int i = 0; i < width; i++) {
        Insert_Queue(pipeline->WB_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0));
    }

    pipeline->cycle_count = 0;
    pipeline->latest_instruction_address_finished = 0x0;
    pipeline->finish_count = 0;

    pipeline->integer_count = 0;
    pipeline->floating_count = 0;
    pipeline->branch_count = 0;
    pipeline->load_count = 0;
    pipeline->store_count = 0;

    return pipeline;
}

struct Instruction *NewInstruction(unsigned long address, int cycle_count, int type,
 unsigned long dependency1, unsigned long dependency2, unsigned long dependency3) {

    struct Instruction *newInstruction = new struct Instruction;
    newInstruction->instruction_address = address;
    newInstruction->cycle_inserted = cycle_count;
    newInstruction->instructionType = type;
    newInstruction->instruction_dependency[0] = dependency1;
    newInstruction->instruction_dependency[1] = dependency2;
    newInstruction->instruction_dependency[2] = dependency3;
    newInstruction->next = NULL;
    newInstruction->prev = NULL;

    return newInstruction;

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
            Insert_Address(Pipeline->finsh_address_queue, Pipeline->WB_queue->head->instruction_address);
            Delete_Instruction(Pipeline->WB_queue);
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
         Pipeline->MEM_queue->head->instruction_dependency[0], Pipeline->MEM_queue->head->instruction_dependency[1],
         Pipeline->MEM_queue->head->instruction_dependency[2]));
        
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

            } else if (Pipeline->EX_queue->head->instructionType == 3) {

                branch_ex = 0;

            } else if (Pipeline->EX_queue->head->instructionType == 4) {

                storing_port = 1;

            } else if (Pipeline->EX_queue->head->instructionType == 5) {

                loading_port = 1;
            }

            Insert_Queue(Pipeline->MEM_queue, NewInstruction(Pipeline->EX_queue->head->instruction_address, 
            Pipeline->EX_queue->head->cycle_inserted, Pipeline->EX_queue->head->instructionType,
            Pipeline->EX_queue->head->instruction_dependency[0], Pipeline->EX_queue->head->instruction_dependency[1],
            Pipeline->EX_queue->head->instruction_dependency[2]));

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
            Pipeline->ID_queue->head->instruction_dependency[0], Pipeline->ID_queue->head->instruction_dependency[1],
            Pipeline->ID_queue->head->instruction_dependency[2]));
    
            Delete_Instruction(Pipeline->ID_queue);

        } else { // stall

            break;
        }
    }

}


void ProcessIF(struct Pipeline *Pipeline, int width)
{
 
    while ((Pipeline->IF_queue->count != 0) && (Pipeline->ID_queue->count != width))
    {   
        //no dependencies or dependencies are finished (Note: at initalization 0x0 is put into the finish address queue)
        if (isAddressFinished(Pipeline->finsh_address_queue, Pipeline->IF_queue->head->instruction_dependency[0])
        && isAddressFinished(Pipeline->finsh_address_queue, Pipeline->IF_queue->head->instruction_dependency[1])
        && isAddressFinished(Pipeline->finsh_address_queue, Pipeline->IF_queue->head->instruction_dependency[2]))
        {   
            struct Instruction* newInstruction= NewInstruction(Pipeline->IF_queue->head->instruction_address, 
            Pipeline->IF_queue->head->cycle_inserted, Pipeline->IF_queue->head->instructionType, 
            Pipeline->IF_queue->head->instruction_dependency[0], Pipeline->IF_queue->head->instruction_dependency[1],
            Pipeline->IF_queue->head->instruction_dependency[2]);

            Insert_Queue(Pipeline->ID_queue,newInstruction);
            Delete_Instruction(Pipeline->IF_queue);

        } else { 

            break;
        }
    }
}



void Simulate_Cycle(struct Pipeline *Pipeline, int width){
    // cout << "Cycle: " << Pipeline->cycle_count << endl; 
    // struct Instruction *temp = Pipeline->WB_queue->head;
    // cout << "WB: " << endl;
    // while (temp != NULL) {
    //     cout << temp->instruction_address << endl;
    //     cout << temp->instructionType << endl;
    //     cout << endl;
    //     temp = temp->next;
    // }
    ProcessWB(Pipeline, width);

    // temp = Pipeline->MEM_queue->head;
    // cout << "MEM: " << endl;
    // while (temp != NULL) {
    //     cout << temp->instruction_address << endl;
    //     cout << temp->instructionType << endl;
    //     cout << endl;
    //     temp = temp->next;
    // }
    ProcessMEM(Pipeline, width);

    // temp = Pipeline->EX_queue->head;
    // cout << "EX: " << endl;
    // while (temp != NULL) {
    //     cout << temp->instruction_address << endl;
    //     cout << temp->instructionType << endl;
    //     cout << endl;
    //     temp = temp->next;
    // }
    ProcessEX(Pipeline, width);

    // temp = Pipeline->ID_queue->head;
    // cout << "ID: " << endl;
    // while (temp != NULL) {
    //     cout << temp->instruction_address << endl;
    //     cout << temp->instructionType << endl;
    //     cout << endl;
    //     temp = temp->next;
    // }
    ProcessID(Pipeline, width);

    // temp = Pipeline->IF_queue->head;
    // cout << "IF: " << endl;
    // while (temp != NULL) {
    //     cout << temp->instruction_address << endl;
    //     cout << temp->instructionType << endl;
    //     cout << endl;
    //     temp = temp->next;
    // }
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

bool isAddressinPipeline(struct Pipeline *Pipeline, unsigned long address) {

    struct Instruction *current = Pipeline->IF_queue->head;
    while (current != NULL) {

        if (current->instruction_address == address) 
        {
            return true;
        }
        current = current->next;
    }

    current = Pipeline->ID_queue->head;
    while (current != NULL) {

        if (current->instruction_address == address) 
        {
            return true;
        }
        current = current->next;
    }

    current = Pipeline->EX_queue->head;
    while (current != NULL) {

        if (current->instruction_address == address) 
        {
            return true;
        }
        current = current->next;
    }

    current = Pipeline->MEM_queue->head;
    while (current != NULL) {

        if (current->instruction_address == address) 
        {
            return true;
        }
        current = current->next;
    }

    current = Pipeline->WB_queue->head;
    while (current != NULL) {

        if (current->instruction_address == address) 
        {
            return true;
        }
        current = current->next;
    }

    return false;
}
bool isAddressFinished(struct AddressQueue *finish_address_queue, unsigned long address) {

    struct Address *current = finish_address_queue->head;
    // (inOrder queue so if current goes past the address it is not in here)
    while ((current != NULL) && (current->address <= address)) {
        cout << "test" << endl;
        if (current->address == address) {
            return true;
        }
        current = current->next;
    }

    return false;
}

void FreePipeline(struct Pipeline *Pipeline) {

    struct Instruction *temp = Pipeline->IF_queue->head;
    while (temp != NULL) {
        struct Instruction *toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }

    temp = Pipeline->ID_queue->head;
    while (temp != NULL) {
        struct Instruction *toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }
    temp = Pipeline->EX_queue->head;
    while (temp != NULL) {
        struct Instruction *toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }

    temp = Pipeline->MEM_queue->head;
    while (temp != NULL) {
        struct Instruction *toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }

    temp = Pipeline->WB_queue->head;
    while (temp != NULL) {
        struct Instruction *toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }

    temp = Pipeline->stall_queue->head;
    while (temp != NULL) {
        struct Instruction *toDelete = temp;
        temp = temp->next;
        delete toDelete;
    }

    struct Address *tempA = Pipeline->finsh_address_queue->head;
    while (tempA != NULL) {
        struct Address *toDelete = tempA;
        tempA = tempA->next;
        delete toDelete;
    }
}

void PrintStatistics(struct Pipeline *Pipeline, int width, int starting_instruction, int simulating_instruction, string filename) {

    cout << "Running the file: '" << filename << "' with a pipeline with W-wide superscalar: " << width <<
       ". Starting Simulation at Instruction " <<  starting_instruction << 
       " and Simulating " << simulating_instruction << " instructions..." << endl;

    cout << endl;
    cout << "Total execution time (Cycles): " << Pipeline->cycle_count << endl;
    cout << endl;
    cout << "Output Histogram:" << endl;
    cout << endl;
    cout << "Integer Instructions(Completed): "<< ((double)Pipeline->integer_count/Pipeline->finish_count*100) << "%" << endl;
    cout << "Floating Instructions(Completed): "<< ((double)Pipeline->floating_count/Pipeline->finish_count*100) << "%" << endl;
    cout << "Branch Instructions(Completed): "<< ((double)Pipeline->branch_count/Pipeline->finish_count*100) << "%" << endl;
    cout << "Load Instructions(Completed): "<< ((double)Pipeline->load_count/Pipeline->finish_count*100) << "%" << endl;
    cout << "Store Instructions(Completed): "<< ((double)Pipeline->store_count/Pipeline->finish_count*100) << "%" << endl;
    cout << endl;
}