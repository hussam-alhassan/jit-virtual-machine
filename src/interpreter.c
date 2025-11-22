#include "interpreter.h"

#include "config.h"
#include "opcodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void print_address_table(struct vm_state* state) {
    printf("\nAddress Table:\n");
    for (int i = 0; i < PROGRAM_SIZE; i++) {
        if (state->address_table[i].address != -1) {
            printf("Label %s at address %d\n", state->address_table[i].name, state->address_table[i].address);
        }
    }
}

void print_state(struct vm_state* state) {
    if (!DEBUG) {
        return;
    }

    // Print bytecode
    printf("Bytecode: ");
    for (int i = 0; i < 30; i++) {
        printf("%02X ", (unsigned char)state->bytecode[i]);
    }
    printf("\n");

    // Print address table
    print_address_table(state);


    // Print stack
    printf("Stack:    X ");
    for (int i = 0; i < 30; i++) {
        int entry = state->stack[i];
        printf("%d ", entry);
    }
    printf("\n%*s\n", 13+(state->sp)*2, "^");

    printf("Pointers: Instruction: %d, Stack %d\n", state->ip, state->sp);

    printf("\n");
}

void interpret_bytecode(struct vm_state* state) {

    // Load all opcodes

    void (*opcodes[256])(struct vm_state* state);

    opcodes[OP_ADD] = op_add;
    opcodes[OP_SUB] = op_sub;
    opcodes[OP_PSH] = op_psh;
    opcodes[OP_POP] = op_pop;
    opcodes[OP_DUP] = op_dup;
    opcodes[OP_PRT] = op_prt;
    opcodes[OP_STR] = op_str;
    opcodes[OP_LOD] = op_lod;
    opcodes[OP_JMP] = op_jmp;

    while (state->ip < PROGRAM_SIZE) {

        int op = state->bytecode[state->ip];


        if (op == OP_HLT) {
            printf("Halting...\n");
            break;
        }

        opcodes[op](state);

        state->ip++;

        // getchar();
        usleep(20000); // 20ms
        print_state(state);
    }
}

void dbg(char* message) {
    if (DEBUG) {
        printf("%s", message);
    }
}

void push_val(struct vm_state* state, int val) {
    (state->sp)++;
	
    if (state->sp >= STACK_SIZE - 1) {
        printf("STACKOVERFLOW: Attempted to access stack address (%d) which exceeds the max (%d)", state->sp, STACK_SIZE);
    	exit(1);
    }

    if (state->sp < -1) {
        printf("STACKUNDERFLOW: Attempted to access stack address (%d) which is below 0", state->sp);
	    exit(1);
    }


    state->stack[state->sp] = val;

}

int pop_val(struct vm_state* state) {
    int value = state->stack[(state->sp)];

    state->stack[(state->sp)--] = STACK_NULL;
    return value;
}

int get_operand(struct vm_state* state) {
    return state->bytecode[++(state->ip)];
}

void op_add(struct vm_state* state) {

    int operand_1 = pop_val(state);
    int operand_2 = pop_val(state);
    int result = operand_1 + operand_2;
    push_val(state, result);
    dbg("Executed ADD\n");
}

void op_sub(struct vm_state* state) {
    dbg("Executed SUB\n");
}

void op_psh(struct vm_state* state) {

    int value = get_operand(state);
    push_val(state, value);
    dbg("Executed PSH\n");
}

void op_pop(struct vm_state* state) {
    pop_val(state);
    dbg("Executed POP\n");
}

void op_dup(struct vm_state* state) {
    int val = pop_val(state);
    push_val(state, val);
    push_val(state, val);
    dbg("Executed DUP\n");
}

void op_prt(struct vm_state* state) {
    int val = pop_val(state);
    printf("PRINT: %d\n", val);
    dbg("Executed PRT\n");
}

void op_str(struct vm_state* state) {
    int mem_address = get_operand(state);
    int val = pop_val(state);
    state->local_mem[mem_address] = val;
    dbg("Executed STR\n");
}

void op_lod(struct vm_state* state) {
    int mem_address = get_operand(state);
    int val = state->local_mem[mem_address];
    push_val(state, val);
    dbg("Executed LOD\n");
}

void op_jmp(struct vm_state* state) {
    int bytecode_address = get_operand(state);
    state->ip = bytecode_address - 1; // -1 to compensate for ip++
    dbg("Executed JMP\n");
}
