#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "config.h"
#include "types.h"

typedef struct {
    int local_mem[MEM_SIZE];                  // Local per function memory
    int return_address;
    char* label;                              // Points to label in address table
} frame;

typedef struct {
    int bytecode[PROGRAM_SIZE];               // Read-only program memory
    struct label address_table[PROGRAM_SIZE]; // Symbols
    int ip;                                   // Instruction pointer
    int sp;                                   // Stack pointer
    int stack[STACK_SIZE];                    // Stack
    int global_mem[MEM_SIZE];                 // Global memory
    frame frame_stack[STACK_SIZE];            // Frame stack
    int fp;                                   // Frame Pointer
    // Some object store?
} vm_state;

void interpret_bytecode(vm_state* state);

void op_add(vm_state* state);
void op_sub(vm_state* state);
void op_psh(vm_state* state);
void op_pop(vm_state* state);
void op_dup(vm_state* state);
void op_prt(vm_state* state);
void op_str(vm_state* state);
void op_lod(vm_state* state);
void op_jmp(vm_state* state);


#endif // INTERPRETER_H
