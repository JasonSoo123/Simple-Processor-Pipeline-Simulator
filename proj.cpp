#include "proj.hpp"
using namespace std;


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
        }

        
        struct Pipeline * pipeline = InitalizePipeline(width);

        cout << "Pipeline Initalized" << endl;

        unsigned long token_array[3] = {0x0, 0x0, 0x0};
        int token_instruction_type = 0;

        int j = 0;


        while (getline(infile, line)) 
        {
            if (j >= starting_instruction) {

                istringstream iss(line);
                string token;
                
                int i = 0;
                while (getline(iss, token, ',')) 
                {

                    if (i != 1) 
                    {   
                        if (i == 0) {

                            token_array[i] = stoul(token);
                        } else {

                            token_array[i-1] = stoul(token);
                        }

                    } 
                    else 
                    {
                        token_instruction_type = stoi(token); 
                    }
                    i++;
                }
                
                // proccess the instruction with tokens
                struct Instruction *newInstruction = NewInstruction(token_array[0],
                pipeline->cycle_count, token_instruction_type, token_array[1], token_array[2]);

                cout << "Instruction " << j << endl;
                cout << "address: " << newInstruction->instruction_address << endl;
                cout << "type: " << newInstruction->instructionType << endl;
                cout << "cycle inserted: " << newInstruction->cycle_inserted << endl;
                cout << "dependency 1: " << newInstruction->instruction_dependency[0] << endl;
                cout << "dependency 2: " << newInstruction->instruction_dependency[1] << endl;
                cout << endl;
                if ((pipeline->stall_queue->count > 0) && (!isBranchin_IF_ID_EX(pipeline)))
                {
                    while((pipeline->stall_queue->count > 0) && (pipeline->IF_queue->count != width))
                    {
                        struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                        pipeline->stall_queue->head->instructionType, pipeline->stall_queue->head->cycle_inserted, 
                        pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1]);
                        Insert_Queue(pipeline->IF_queue, newInstruction);
                        Delete_Instruction(pipeline->stall_queue);
                    }
                }
                if ((pipeline->IF_queue->count != width)) 
                {   
                    if (pipeline->IF_queue->head == NULL) {

                        Insert_Queue(pipeline->IF_queue, newInstruction);
                        pipeline->instructions_count++;

                    } else {
                        
                        if ((pipeline->IF_queue->tail->instructionType != 3) && 
                        (pipeline->IF_queue->tail->instructionType != 6)) {

                            Insert_Queue(pipeline->IF_queue, newInstruction);
                            pipeline->instructions_count++;

                        }  else {

                             Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, 6, -1, 0x0, 0x0)); // place a dummy node
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
                token_instruction_type = 0;

                if ((pipeline->IF_queue->count == width) && (pipeline->ID_queue->count == width) &&
                (pipeline->EX_queue->count == width) && (pipeline->MEM_queue->count == width) &&
                (pipeline->WB_queue->count == width)) 
                {

                    Simulate_Cycle(pipeline, width);

                }
            }
            
            j++;
        }
        
        infile.close(); 
        
    } 
    else 
    {
        cout << "Insufficient command line arguments provided." << endl;
    }

    return 0;
}
