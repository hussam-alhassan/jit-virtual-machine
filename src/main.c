#include "config.h"
#include "opcodes.h"
#include "loader.h"
#include "assembler.h"
#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {

    // Define state

    char program[PROGRAM_SIZE];
    //int bytecode[PROGRAM_SIZE];
    //struct label address_table[PROGRAM_SIZE];

    struct vm_state state;

    memset(&state, 0, sizeof(state));
    memset(state.address_table, -1, sizeof(state.address_table));
    memset(state.stack, STACK_NULL, sizeof(state.stack));

    state.sp = -1;

    // Load program
    load_program(program);

    // Assemble to bytecode
    printf("\nAssembling Bytecode...\n");
    assemble_bytecode(program, &state);
    // print_bytecode(bytecode);
    // print_address_table(address_table);

    // Set up initial frame
    frame main_frame;
    main_frame.label = "MAIN";

    state.fp = 0;
    state.frame_stack[state.fp] = main_frame;

    // Run program
    printf("\nRunning Bytecode...\n");
    interpret_bytecode(&state);

       
    return 0;
}
