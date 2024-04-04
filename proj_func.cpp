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

    pipeline->finish_count = 0;

    return pipeline;
}

struct Instruction *NewInstruction(){


}