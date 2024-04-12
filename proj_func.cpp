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

// NOTE: all AddressNode functions for the tree is referenced from Geeks for Geeks.
// https://www.geeksforgeeks.org/insertion-in-an-avl-tree/

// inserting instruction to a queue
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

// Helper function to get the height of a node
int height(struct AddressNode *node) {

    if (node == NULL) {

        return 0;
    }

    return node->height;
}

// make a new Address node
struct AddressNode *newAddressNode(unsigned long address) {

    struct AddressNode* node = new struct AddressNode;
    node->address = address;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// Helper function to right rotate subtree rooted with y
struct AddressNode *rightRotate(struct AddressNode *y) {

    struct AddressNode *x = y->left;
    struct AddressNode *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// Helper function to left rotate subtree rooted with x
struct AddressNode *leftRotate(struct AddressNode *x) {
    struct AddressNode *y = x->right;
    struct AddressNode *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Get Balance factor of node
int getBalance(struct AddressNode *node) {

    if (node == NULL) {
        return 0;
    }

    return height(node->left) - height(node->right);
}

// Recursive function to insert a node with given address in AVL tree
struct AddressNode* insertAddress(struct AddressNode *node, unsigned long address) {

    // Perform normal BST insertion
    if (node == NULL) {

        return newAddressNode(address);
    }

    if (address < node->address) {

        node->left = insertAddress(node->left, address);

    } else if (address > node->address) {

        node->right = insertAddress(node->right, address);

    } else { // Duplicate addresses are not allowed

        return node;
    }

    // Update height of this ancestor node
    node->height = 1 + max(height(node->left), height(node->right));

    // check if it became unbalanced
    int balance = getBalance(node);

    // If this node becomes unbalanced, four cases

    // Left Left Case
    if (balance > 1 && address < node->left->address) {

        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && address > node->right->address) {

        return leftRotate(node);
    }

    // Left Right Case
    if ((balance > 1) && (address > node->left->address)) {

        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if ((balance < -1) && (address < node->right->address)) {

        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Insert address into AVL tree
void Insert_Address(struct AddressTree *address_tree, unsigned long instruction_address){

    address_tree->root = insertAddress(address_tree->root, instruction_address);
    address_tree->count++;
}

// return the node with the min address value found in that sub tree
struct AddressNode *minValueNode(struct AddressNode *node) {

    struct AddressNode *current = node;

    while (current->left != NULL) {

        current = current->left;
    }

    return current;
}

// Recursive function to delete a node with given address from AVL tree and adjust the address queue
struct AddressNode* deleteAddress(struct AddressNode *root, unsigned long address) {
   
    if (root == NULL) {

        return root;
    }

    // Search for the node to be deleted
    if (address < root->address) {

        root->left = deleteAddress(root->left, address);

    } else if (address > root->address) {

        root->right = deleteAddress(root->right, address);

    } else {
        // Node with the address to be deleted found

        // Node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL)) {
            struct AddressNode *temp;
            if (root->left != NULL) {

                temp = root->left;

            } else {

                temp = root->right;
            }

            // No child case
            if (temp == NULL) {

                temp = root;
                root = NULL;

            } else { // One child case

                *root = *temp; // Copy the contents of the non-empty child
            }
            delete temp;

        } else {  // Node with two children: Get the inorder successor (smallest in the right subtree)

            struct AddressNode* temp = minValueNode(root->right);

            // Copy the inorder successor's data to this node
            root->address = temp->address;

            // Delete the inorder successor
            root->right = deleteAddress(root->right, temp->address);
        }
    }

    // If the tree had only one node then return
    if (root == NULL) {

        return root;
    }

    // Update the height of the current node
    root->height = 1 + max(height(root->left), height(root->right));

    // Check if the node is unbalanced
    int balance = getBalance(root);

    // Left Left Case
    if ((balance > 1) && (getBalance(root->left) >= 0)) {

        return rightRotate(root);
    }

    // Left Right Case
    if ((balance > 1) && (getBalance(root->left) < 0)) {

        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if ((balance < -1) && (getBalance(root->right) <= 0)) {

        return leftRotate(root);
    }

    // Right Left Case
    if ((balance < -1) && (getBalance(root->right) > 0)) {

        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Delete address in AVL tree
void Delete_Address(struct AddressTree *address_tree, unsigned long instruction_address){
    // Check if the tree is empty
    if (address_tree == NULL || address_tree->root == NULL) {

        return;
    }

    // Delete the address from the AVL tree
    address_tree->root = deleteAddress(address_tree->root, instruction_address);
    address_tree->count--;
}

// initalize the Pipeline
struct Pipeline *InitalizePipeline(int width){

    struct Pipeline *pipeline = new struct Pipeline;

    pipeline->stall_queue = new struct InstructionQueue;
    pipeline->stall_queue->head = NULL;
    pipeline->stall_queue->tail = NULL;
    pipeline->stall_queue->count = 0;

    pipeline->finish_address_tree = new struct AddressTree;
    pipeline->finish_address_tree->root = NULL;
    pipeline->finish_address_tree->count = 0;
    Insert_Address(pipeline->finish_address_tree, 0x0); // dummy address where dummies can work


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

// make a new Instruction node
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

// delete the head instruction of a queue
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

// proccess the write back stage
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

            Delete_Instruction(Pipeline->WB_queue);
            Pipeline->finish_count++;
            
        } else {

            Delete_Instruction(Pipeline->WB_queue);
        }
    }
}

// proccess the memory stage
void ProcessMEM(struct Pipeline *Pipeline, int width)
{

    while ((Pipeline->MEM_queue->count != 0 ) && (Pipeline->WB_queue->count != width)) 
    {
        if (Pipeline->MEM_queue->head->instructionType == 4) {
            Insert_Address(Pipeline->finish_address_tree, Pipeline->MEM_queue->head->instruction_address);
            storing_port = 0;

        } else if (Pipeline->MEM_queue->head->instructionType == 5) {
            Insert_Address(Pipeline->finish_address_tree, Pipeline->MEM_queue->head->instruction_address);
            loading_port = 0;
        }
        
        Insert_Queue(Pipeline->WB_queue, NewInstruction(Pipeline->MEM_queue->head->instruction_address, 
        Pipeline->MEM_queue->head->cycle_inserted, Pipeline->MEM_queue->head->instructionType,
         Pipeline->MEM_queue->head->instruction_dependency[0], Pipeline->MEM_queue->head->instruction_dependency[1],
         Pipeline->MEM_queue->head->instruction_dependency[2]));
        
        Delete_Instruction(Pipeline->MEM_queue);
    }
}

// process the execute stage
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

                Insert_Address(Pipeline->finish_address_tree, Pipeline->EX_queue->head->instruction_address);
                int_ALU = 0;

            } else if (Pipeline->EX_queue->head->instructionType == 2) {

                Insert_Address(Pipeline->finish_address_tree, Pipeline->EX_queue->head->instruction_address);
                ftp_ALU = 0;

            } else if (Pipeline->EX_queue->head->instructionType == 3) {

                Insert_Address(Pipeline->finish_address_tree, Pipeline->EX_queue->head->instruction_address);
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

// process the instruction decode stage
void ProcessID(struct Pipeline *Pipeline, int width)
{   
     while ((Pipeline->ID_queue->count != 0) && Pipeline->EX_queue->count != width)
    {   
        // Assuming because it is an In-Order Pipeline...
        // check if it passes structual hazards
        if (((Pipeline->ID_queue->head->instructionType == 1) && (int_ALU == 0)) ||
         ((Pipeline->ID_queue->head->instructionType == 2) && (ftp_ALU == 0)) || 
         ((Pipeline->ID_queue->head->instructionType == 3) && (branch_ex == 0)) ||
         (Pipeline->ID_queue->head->instructionType == 4) || 
         (Pipeline->ID_queue->head->instructionType == 5) ||
         (Pipeline->ID_queue->head->instructionType == 6)) {

            // check if it passes all dependency tests.
            if (isAddressTree(Pipeline->finish_address_tree, Pipeline->ID_queue->head->instruction_dependency[0])
            && isAddressTree(Pipeline->finish_address_tree, Pipeline->ID_queue->head->instruction_dependency[1])
            && isAddressTree(Pipeline->finish_address_tree, Pipeline->ID_queue->head->instruction_dependency[2])) {

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


        } else { // stall

            break;
        }
    }

}

// process the instruction fetch stage
void ProcessIF(struct Pipeline *Pipeline, int width)
{
 
    while ((Pipeline->IF_queue->count != 0) && (Pipeline->ID_queue->count != width))
    {    
        // if you cannot find the the dependency in the pipeline or in the finished address tree
        if (!isAddressinPipeline(Pipeline, Pipeline->IF_queue->head->instruction_dependency[0])
         && !isAddressTree(Pipeline->finish_address_tree, Pipeline->IF_queue->head->instruction_dependency[0])) {

            Pipeline->IF_queue->head->instruction_dependency[0] = 0x0; // reset the dependency
        }

        // if you cannot find the the dependency in the pipeline or in the finished address tree
        if (!isAddressinPipeline(Pipeline, Pipeline->IF_queue->head->instruction_dependency[1])
         && !isAddressTree(Pipeline->finish_address_tree, Pipeline->IF_queue->head->instruction_dependency[1])) {

            Pipeline->IF_queue->head->instruction_dependency[1] = 0x0; // reset the dependency
        }

        // if you cannot find the the dependency in the pipeline or in the finished address tree
        if (!isAddressinPipeline(Pipeline, Pipeline->IF_queue->head->instruction_dependency[2])
         && !isAddressTree(Pipeline->finish_address_tree, Pipeline->IF_queue->head->instruction_dependency[2])) {

            Pipeline->IF_queue->head->instruction_dependency[2] = 0x0; // reset the dependency
        }

        struct Instruction* newInstruction= NewInstruction(Pipeline->IF_queue->head->instruction_address, 
        Pipeline->IF_queue->head->cycle_inserted, Pipeline->IF_queue->head->instructionType, 
        Pipeline->IF_queue->head->instruction_dependency[0], Pipeline->IF_queue->head->instruction_dependency[1],
        Pipeline->IF_queue->head->instruction_dependency[2]);

        Insert_Queue(Pipeline->ID_queue,newInstruction);
        Delete_Instruction(Pipeline->IF_queue);
    }
}

// simulate a cycle
void Simulate_Cycle(struct Pipeline *Pipeline, int width, int simulating_instruction){
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

    if (Pipeline->finish_count != simulating_instruction) {

        Pipeline->cycle_count++;
    }
}

// check if there is a branch instruction in the first 3 stages
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

// check if that address is in the pipeline
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

// Recursive function that finds the address in AVL tree
bool is_Address_Tree(struct AddressNode *root, unsigned long address) {

    if (root == NULL) {
        
        return false;   
    }
    if (root->address == address) {
       
        return true;
    }
    if (address < root->address) {

        return is_Address_Tree(root->left, address);
    }
    else {

        return is_Address_Tree(root->right, address);
    }
}

// find if the address is finished executing
bool isAddressTree(struct AddressTree *tree, unsigned long address){
    
    return is_Address_Tree(tree->root, address);
}

// Recursive function that frees the tree
void freeAddressTree(struct AddressNode *root) {

    if (root == NULL){

        return;
    }

    freeAddressTree(root->left);
    freeAddressTree(root->right);
    free(root);

}

// free the tree
void Free_Address_Tree(struct AddressTree *address_tree) {

    freeAddressTree(address_tree->root);
    address_tree->root = NULL; 
}

// free everything in pipeline
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

    Free_Address_Tree(Pipeline->finish_address_tree);
}

// print stats
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