#include "interpreter.h"

#include "config.h"
#include "opcodes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void print_address_table(vm_state* state) {
    printf("\nAddress Table:\n");
    for (int i = 0; i < PROGRAM_SIZE; i++) {
        if (state->address_table[i].address != -1) {
            printf("Label %s at address %d\n", state->address_table[i].name, state->address_table[i].address);
        }
    }
}

void print_state(vm_state* state) {

    // Print address table
    print_address_table(state);
    printf("\n");

    // Print bytecode
    printf("Bytecode: ");
    for (int i = 0; i < 30; i++) {
        printf("%02X ", (unsigned char)state->bytecode[i]);
    }
    printf("\n");

    // Print stack
    printf("Stack:    X ");
    for (int i = 0; i < 30; i++) {
        int entry = state->stack[i];
        printf("%d ", entry);
    }
    printf("\n%*s\n", 13+(state->sp)*2, "^");

    printf("Pointers: Instruction: %d, Stack %d\n", state->ip, state->sp);

    printf("----END OP\n\n");
}

void interpret_bytecode(vm_state* state) {

    // Load all opcodes

    void (*opcodes[256])(vm_state* state);

    opcodes[OP_ADD] = op_add;
    opcodes[OP_SUB] = op_sub;
    opcodes[OP_PSH] = op_psh;
    opcodes[OP_POP] = op_pop;
    opcodes[OP_DUP] = op_dup;
    opcodes[OP_PRT] = op_prt;
    opcodes[OP_STR] = op_str;
    opcodes[OP_LOD] = op_lod;
    opcodes[OP_JMP] = op_jmp;
    opcodes[OP_CAL] = op_cal;
    opcodes[OP_RET] = op_ret;

    while (state->ip < PROGRAM_SIZE) {

        if (DEBUG) {
            printf("----START OP @ip %d in frame no. %d\n", state->ip, state->fp);
        }

        int op = state->bytecode[state->ip];


        if (op == OP_HLT) {
            printf("Halting...\n");
            break;
        }

        opcodes[op](state); // op call

        state->ip++;

        // Check we haven't exited the last frame
        if (state->fp < 0) {
            printf("Exited last frame, halting...\n");
            break;
        }

        // getchar();
        usleep(20000); // 20ms

        if (DEBUG) {
            print_state(state);
        }
    }
}

void dbg(char* message) {
    if (DEBUG) {
        printf("%s", message);
    }
}

/* ==----- Helper Functions -----== */

void push_val(vm_state* state, int val) {
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

int pop_val(vm_state* state) {
    int value = state->stack[(state->sp)];

    state->stack[(state->sp)--] = STACK_NULL;
    return value;
}

int get_operand(vm_state* state) {
    return state->bytecode[++(state->ip)];
}

void store_local(vm_state* state, int value) {

}

int load_local(vm_state* state) {

}

/* ==----- Opcodes -----== */

void op_add(vm_state* state) {

    int operand_1 = pop_val(state);
    int operand_2 = pop_val(state);
    int result = operand_1 + operand_2;
    push_val(state, result);
    dbg("Executed ADD\n");
}

void op_sub(vm_state* state) {
    dbg("Executed SUB\n");
}

void op_psh(vm_state* state) {

    int value = get_operand(state);
    push_val(state, value);
    dbg("Executed PSH\n");
}

void op_pop(vm_state* state) {
    pop_val(state);
    dbg("Executed POP\n");
}

void op_dup(vm_state* state) {
    int val = pop_val(state);
    push_val(state, val);
    push_val(state, val);
    dbg("Executed DUP\n");
}

void op_prt(vm_state* state) {
    int val = pop_val(state);
    printf("PRINT: %d\n", val);
    push_val(state, val);
    dbg("Executed PRT\n");
}

void op_str(vm_state* state) {
    int mem_address = get_operand(state);
    int val = pop_val(state);
    state->frame_stack[state->fp]->local_mem[mem_address] = val;
    dbg("Executed STR\n");
}

void op_lod(vm_state* state) {
    int mem_address = get_operand(state);
    int val = state->frame_stack[state->fp]->local_mem[mem_address];
    push_val(state, val);
    dbg("Executed LOD\n");
}

void op_jmp(vm_state* state) {
    int target_address = get_operand(state);
    state->ip = target_address - 1; // -1 to compensate for ip++
    dbg("Executed JMP\n");
}

// Set up a stack frame and jump to it
void op_cal(vm_state* state) {
    int return_address = state->ip + 1;
    frame* new_frame = malloc(sizeof(frame));
    new_frame->return_address = return_address;
    (state->fp)++;
    state->frame_stack[state->fp] = new_frame;
    op_jmp(state);
    dbg("Executed CAL\n");
}

void op_ret(vm_state* state) {
    dbg("Executing RET...\n");
    int return_address = state->frame_stack[state->fp]->return_address;
    free(state->frame_stack[state->fp]);
    (state->fp)--;
    state->ip = return_address;
    dbg("Executed RET\n");
}