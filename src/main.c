#include "config.h"
#include "opcodes.h"
#include "loader.h"
#include "assembler.h"
#include "interpreter.h"
#include "jit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int setup_state(vm_state* state) {
    memset(state, 0, sizeof(state));
    memset(state->native_funcs, -1, sizeof(state->native_funcs));
    memset(state->stack, STACK_NULL, sizeof(state->stack));

    // Set jump table addresses to -1 (meaning not set)
    for (int i = 0; i < FUNC_TABLE_SIZE; i++) {
        state->jump_table[i].address = -1;
    }

    state->sp = -1; // TODO: Should start from 0
}

int setup_initial_frame(vm_state* state) {
    // Set up initial frame
    frame* main_frame = malloc(sizeof(frame));
    main_frame->label = "MAIN";

    state->fp = 0;
    state->frame_stack[state->fp] = main_frame;

    // Check if MAIN entry point exists
    for (int i = 0; i < FUNC_TABLE_SIZE; i++) {
        if (strncmp(state->jump_table[i].name, "@MAIN", 4) == 0) {
            state->ip = state->jump_table[i].address;
            break;
        }
    }
}

int main() {

    // Define state
    vm_state state;
    setup_state(&state);
    printf("MAIN: State setup complete.\n");

    // Load program
    char program[PROGRAM_SIZE];
    char* file_name = "programs/test_native_str.bc";
    load_program(program, file_name);

    // Assemble to bytecode
    assemble_bytecode(program, &state);
    printf("MAIN: Bytecode assembled.\n");
    //print_bytecode(bytecode);
    //print_jump_table(jump_table);

    setup_initial_frame(&state);
	printf("MAIN: Initial frame set.\n");

    // TEST emission
    //native_test(&state);

    //return 0;

    // Run program
    printf("\nRunning Bytecode...\n");
    interpret_bytecode(&state);

    printf("\n----------------------------------------------------\n");
    printf("\nProgram halted.\n");
    print_vm_state(&state);
       
    return 0;
}
