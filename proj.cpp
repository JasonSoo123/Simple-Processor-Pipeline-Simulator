#include "proj.hpp"

int instruction_count = 0;
unsigned long first_instruction_address = 0x0;

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

        unsigned long token_array[4] = {0x0, 0x0, 0x0, 0x0};
        int token_instruction_type = 0;

        int j = 0;


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
                    first_instruction_address = token_array[0];
                    token_array[1] = 0x0;
                    token_array[2] = 0x0;
                    token_array[3] = 0x0;
                }

                // if the dependencies you are checking is less then your first instruction address then remove it.
                // check also if there is a case where the dependency is somehow larger then the original address reset it.
                if ((token_array[1] < first_instruction_address) || (token_array[1] >= token_array[0])) {

                    token_array[1] = 0x0;
                }
                if (token_array[2] < first_instruction_address || (token_array[2] >= token_array[0])) {

                    token_array[2] = 0x0;
                }
                if (token_array[3] < first_instruction_address || (token_array[3] >= token_array[0])) {

                    token_array[3] = 0x0;
                }

                struct Instruction *newInstruction = NewInstruction(token_array[0],
                pipeline->cycle_count, token_instruction_type, token_array[1], token_array[2], token_array[3]);

                instruction_count++;

                if ((pipeline->stall_queue->count > 0) && (!isBranchin_IF_ID_EX(pipeline)))
                {
                    while((pipeline->stall_queue->count > 0) && (pipeline->IF_queue->count != width))
                    {
                        struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                        pipeline->cycle_count, pipeline->stall_queue->head->instructionType, 
                        pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1],
                        pipeline->stall_queue->head->instruction_dependency[2]);
                        Insert_Queue(pipeline->IF_queue, newInstruction);
                        Delete_Instruction(pipeline->stall_queue);
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

                        Insert_Queue(pipeline->IF_queue, newInstruction);
          

                    } else {
                        
                        if ((pipeline->IF_queue->tail->instructionType != 3) && 
                        (pipeline->IF_queue->tail->instructionType != 6)) {

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
                    
                    Simulate_Cycle(pipeline, width);

                }
            }
            
            j++;
            
            if (j >= starting_instruction + simulating_instruction){
                break;
            }
        }
        infile.close(); 
        
      while (pipeline->finish_count < simulating_instruction) {

            if (isBranchin_IF_ID_EX(pipeline)) {
                int x = pipeline->IF_queue->count;
                for (int i = x; i < width; i ++) {

                    Insert_Queue(pipeline->IF_queue, NewInstruction(0x0, -1, 6, 0x0, 0x0, 0x0)); // place a dummy node
                }
            }
            while((pipeline->stall_queue->count > 0) && (pipeline->IF_queue->count != width))
            {
                struct Instruction *newInstruction = NewInstruction(pipeline->stall_queue->head->instruction_address,
                pipeline->cycle_count, pipeline->stall_queue->head->instructionType,
                pipeline->stall_queue->head->instruction_dependency[0], pipeline->stall_queue->head->instruction_dependency[1],
                pipeline->stall_queue->head->instruction_dependency[2]);
                Insert_Queue(pipeline->IF_queue, newInstruction);
                Delete_Instruction(pipeline->stall_queue);
            }
            Simulate_Cycle(pipeline, width);
        }
        PrintStatistics(pipeline, width, starting_instruction, simulating_instruction, filename);
        FreePipeline(pipeline);
        
    } 
    else 
    {
        cout << "Insufficient command line arguments provided." << endl;
    }

    return 0;
}
