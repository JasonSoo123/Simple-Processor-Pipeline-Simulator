#include "proj.hpp"

int instruction_count = 0;
unsigned long last_instruction_address = 0x0;

int main(int argc, char* argv[]) 
{
    if (argc >= 5) 
    {
        string filename = argv[1];
        ifstream infile(filename);

        if (!infile.is_open()) 
        {
            cout << "Failed to open file: " << filename << endl;
            return 1;
        }
        
        string line;
        int starting_instruction = stoi(argv[2]);
        int simulating_instruction = stoi(argv[3]);
        int width = stoi(argv[4]);

        if ((width>4)||(width<1)||(starting_instruction<0)||(simulating_instruction<0))
        {
            cout << "Input Error. Terminating Simulation...\n" << endl;

        } else {

            struct Pipeline * pipeline = InitalizePipeline(width);

            unsigned long token_array[4] = {0x0, 0x0, 0x0, 0x0};
            int token_instruction_type = 0;

            int j = 1;


            while (getline(infile, line)) 
            {
                if (j >= starting_instruction && j < starting_instruction + simulating_instruction) {
                
                    istringstream iss(line);
                    string token;
                    
                    int i = 0;
                    while (getline(iss, token, ',')) 
                    {

                        if (i != 1) 
                        {   
                            if (i == 0) {

                                token_array[i] = stoul(token, nullptr, 16);
                            } else {

                                token_array[i-1] = stoul(token, nullptr, 16);
                            }

                        } 
                        else 
                        {
                            token_instruction_type = stoi(token); 
                        }
                        i++;
                    }
                
                    // if it is the first instruction throw out dependencies.
                    if (instruction_count == 0) {
                        last_instruction_address = token_array[0];
                        token_array[1] = 0x0;
                        token_array[2] = 0x0;
                        token_array[3] = 0x0;
                    }

                    // if it is already done then no need to check for dependency
                    if (isAddressTree(pipeline->finish_address_tree, token_array[1])) {

                        token_array[1] = 0x0;
                    }

                    if (isAddressTree(pipeline->finish_address_tree, token_array[2])) {

                        token_array[2] = 0x0;
                    }

                    if (isAddressTree(pipeline->finish_address_tree, token_array[3])) {

                        token_array[3] = 0x0;
                    }

                    struct Instruction *newInstruction = NewInstruction(token_array[0],
                    pipeline->cycle_count, token_instruction_type, token_array[1], token_array[2], token_array[3]);

                    instruction_count++;

                    // if there is something in the stall queue and no branch
                    if ((pipeline->stall_queue->count > 0) && (!isBranchin_IF_ID_EX(pipeline)))
                    {   
                        // while there is something in stall and IF is not full
                        while((pipeline->stall_queue->count > 0) && (pipeline->IF_queue->count != width))
                        {   
                    
                            // if IF is empty
                            if (pipeline->IF_queue->head == NULL) {

                                struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                                pipeline->cycle_count, pipeline->stall_queue->head->instructionType,
                                pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1],
                                pipeline->stall_queue->head->instruction_dependency[2]);

                                // if it is already done then no need to check for dependency
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[0])) {

                                newInstruction->instruction_dependency[0] = 0x0;
                            }

                            // if it is already done then no need to check for dependency
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[1])) {

                                newInstruction->instruction_dependency[1] = 0x0;
                            }

                            // if it is already done then no need to check for dependency
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[1])) {

                                newInstruction->instruction_dependency[2] = 0x0;
                            }

                                // if the instruction address is being processed again, erase the address from the finish queu
                                if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_address)) {
                                   
                                    Delete_Address(pipeline->finish_address_tree, newInstruction->instruction_address);
                                }

                                Insert_Queue(pipeline->IF_queue, newInstruction);
                                Delete_Instruction(pipeline->stall_queue);

                            } else {
                                
                                // if it is not a branch or not a dummy fill procced 
                                if ((pipeline->IF_queue->tail->instructionType != 3) && 
                                    (pipeline->IF_queue->tail->instructionType != 6)) {

                                    struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                                    pipeline->cycle_count, pipeline->stall_queue->head->instructionType,
                                    pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1],
                                    pipeline->stall_queue->head->instruction_dependency[2]);

                                    // if it is already done then no need to check for dependency
                                    if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[0])) {

                                        newInstruction->instruction_dependency[0] = 0x0;
                                    }

                                    // if it is already done then no need to check for dependency
                                    if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[1])) {

                                        newInstruction->instruction_dependency[1] = 0x0;
                                    }

                                    // if it is already done then no need to check for dependency
                                    if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[1])) {

                                        newInstruction->instruction_dependency[2] = 0x0;
                                    }
                                    // if the instruction address is being processed again, erase the address from the finish queue
                                    if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_address)) {
                                
                                        Delete_Address(pipeline->finish_address_tree, newInstruction->instruction_address);
                                    }
                
                                    Insert_Queue(pipeline->IF_queue, newInstruction);
                                    Delete_Instruction(pipeline->stall_queue);

                                } else {

                                    Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0)); // place a dummy node
                                }
                            }
                        }
                    }
                
                    if (isBranchin_IF_ID_EX(pipeline)) {
                        int x = pipeline->IF_queue->count;
                        for (int i = x; i < width; i ++) {

                            Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0)); // place a dummy node
                        }
                    }
                    if ((pipeline->IF_queue->count != width)) 
                    {   
                        if (pipeline->IF_queue->head == NULL) {
                            
                            // if the instruction address is being processed again, erase the address from the finish queu
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_address)) {
                        
                                Delete_Address(pipeline->finish_address_tree, newInstruction->instruction_address);
                            }
                            Insert_Queue(pipeline->IF_queue, newInstruction);
            

                        } else {
                            
                            if ((pipeline->IF_queue->tail->instructionType != 3) && 
                            (pipeline->IF_queue->tail->instructionType != 6)) {
                                
                                // if the instruction address is being processed again, erase the address from the finish queu
                                if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_address)) {
                        
                                    Delete_Address(pipeline->finish_address_tree, newInstruction->instruction_address);
                                }
                                Insert_Queue(pipeline->IF_queue, newInstruction);
                                

                            }  else {

                                Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0)); // place a dummy node
                                Insert_Queue(pipeline->stall_queue, newInstruction);   
                            }
                        }
                    }
                    else 
                    {   
                        Insert_Queue(pipeline->stall_queue, newInstruction);
                    }
                    
                    // reset the token array
                    token_array[0] = 0x0;
                    token_array[1] = 0x0;
                    token_array[2] = 0x0;
                    token_array[3] = 0x0;
                    token_instruction_type = 0;
                
                    if ((pipeline->IF_queue->count == width) && (pipeline->ID_queue->count == width) &&
                    (pipeline->EX_queue->count == width) && (pipeline->MEM_queue->count == width) &&
                    (pipeline->WB_queue->count == width)) 
                    {
                        
                        Simulate_Cycle(pipeline, width, simulating_instruction);

                    }
                }
                
                j++;

                if (j >= starting_instruction + simulating_instruction){
                    break;
                }
            }
            infile.close(); 

         
            while (pipeline->finish_count < simulating_instruction) {
                
                // if there is a branch that hasnt excuted yet in the pipeline
                if (isBranchin_IF_ID_EX(pipeline)) {

                    int x = pipeline->IF_queue->count;
                    for (int i = x; i < width; i ++) {

                        Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0)); // place a dummy node
                    }
                }
                
                while((pipeline->stall_queue->count > 0) && (pipeline->IF_queue->count != width))
                {   
                    
                    // if IF is 0
                    if (pipeline->IF_queue->head == NULL) {

                        struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                        pipeline->cycle_count, pipeline->stall_queue->head->instructionType,
                        pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1],
                        pipeline->stall_queue->head->instruction_dependency[2]);


                        // if it is already done then no need to check for dependency
                        if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[0])) {

                            newInstruction->instruction_dependency[0] = 0x0;
                        }

                        // if it is already done then no need to check for dependency
                        if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[1])) {

                            newInstruction->instruction_dependency[1] = 0x0;
                        }

                        // if it is already done then no need to check for dependency
                        if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[2])) {

                            newInstruction->instruction_dependency[2] = 0x0;
                        }

                        // if the instruction address is being processed again, erase the address from the finish queu
                        if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_address)) {
                           
                            Delete_Address(pipeline->finish_address_tree, newInstruction->instruction_address);
                        }

                        Insert_Queue(pipeline->IF_queue, newInstruction);
                        Delete_Instruction(pipeline->stall_queue);

                    } else {

                        if ((pipeline->IF_queue->tail->instructionType != 3) && 
                            (pipeline->IF_queue->tail->instructionType != 6)) {

                            struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                            pipeline->cycle_count, pipeline->stall_queue->head->instructionType,
                            pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1],
                            pipeline->stall_queue->head->instruction_dependency[2]);

                            // if it is already done then no need to check for dependency
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[0])) {

                                newInstruction->instruction_dependency[0] = 0x0;
                            }

                            // if it is already done then no need to check for dependency
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[1])) {

                                newInstruction->instruction_dependency[1] = 0x0;
                            }

                            // if it is already done then no need to check for dependency
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_dependency[2])) {

                                newInstruction->instruction_dependency[2] = 0x0;
                            }

                            // if the instruction address is being processed again, erase the address from the finish queu
                            if (isAddressTree(pipeline->finish_address_tree, newInstruction->instruction_address)) {
                        
                                Delete_Address(pipeline->finish_address_tree, newInstruction->instruction_address);
                            }
                            Insert_Queue(pipeline->IF_queue, newInstruction);
                            Delete_Instruction(pipeline->stall_queue);

                        } else {

                            Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0)); // place a dummy node
                        }
                    }
                }
                Simulate_Cycle(pipeline, width, simulating_instruction);
            }
            PrintStatistics(pipeline, width, starting_instruction, simulating_instruction, filename);
            FreePipeline(pipeline);
        }
    } 
    else 
    {
        cout << "Insufficient command line arguments provided." << endl;
    }

    return 0;
}
