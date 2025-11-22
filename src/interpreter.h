#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "config.h"
#include "types.h"

struct vm_state {
    int bytecode[PROGRAM_SIZE];
    struct label address_table[PROGRAM_SIZE];
    int ip;
    int stack[STACK_SIZE];
    int sp;
    int local_mem[MEM_SIZE];
    int global_mem[MEM_SIZE];
};

void interpret_bytecode(struct vm_state* state);

void op_add(struct vm_state* state);
void op_sub(struct vm_state* state);
void op_psh(struct vm_state* state);
void op_pop(struct vm_state* state);
void op_dup(struct vm_state* state);
void op_prt(struct vm_state* state);
void op_str(struct vm_state* state);
void op_lod(struct vm_state* state);
void op_jmp(struct vm_state* state);

#endif // INTERPRETER_H
