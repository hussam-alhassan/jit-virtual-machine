#include "assembler.h"

#include "opcodes.h"
#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int op_matches(char* line, char* opcode) {
    return strncmp(line, opcode, 3) == 0;
}

int register_jump_address(vm_state* state, char* line, int address_counter) {
    char label_name[LABEL_MAX_LENGTH];
    sscanf(line + 3, "%s", label_name);
    int label_address = -1;
    get_label_address(state->address_table, address_counter, label_name, &label_address);
    if (label_address == -1) {
        printf("Error: Undefined label %s\n", label_name);
        printf("Aborting.\n");
        return -1;
    }
    return label_address;
}

int assemble_bytecode(char* program, vm_state* state) {
    char* line = strtok(program, "\n");
    int line_number = 1;
    int address_counter = 0;
    int bytecode_length = 0;

    for (int i = 0; line != NULL; i++) {
        if (op_matches(line, "PSH")) {
            state->bytecode[i] = OP_PSH;
            int value = atoi(line + 4);
            state->bytecode[++i] = (char)value;
            print_opcode_with_arg(line_number, OP_PSH, "PSH", value);
        } else if (op_matches(line, "DUP")) {
            state->bytecode[i] = OP_DUP;
            print_opcode(line_number, OP_DUP, "DUP");
        } else if (op_matches(line, "PRT")) {
            state->bytecode[i] = OP_PRT;
            print_opcode(line_number, OP_PRT, "PRT");
        } else if (op_matches(line, "STR")) {
            state->bytecode[i] = OP_STR;
            int addr = atoi(line + 4);
            state->bytecode[++i] = (char)addr;
            print_opcode_with_arg(line_number, OP_STR, "STR", addr);
        } else if (op_matches(line, "LOD")) {
            state->bytecode[i] = OP_LOD;
            int addr = atoi(line + 4);
            state->bytecode[++i] = (char)addr;
            print_opcode_with_arg(line_number, OP_LOD, "LOD", addr);
        } else if (op_matches(line, "ADD")) {
            state->bytecode[i] = OP_ADD;
            print_opcode(line_number, OP_ADD, "ADD");
        } else if (op_matches(line, "SUB")) {
            state->bytecode[i] = OP_SUB;
            print_opcode(line_number, OP_SUB, "SUB");
        } else if (op_matches(line, "POP")) {
            state->bytecode[i] = OP_POP;
            print_opcode(line_number, OP_POP, "POP");
        } else if (op_matches(line, "JMP")) {
            state->bytecode[i] = OP_JMP;
            int label_address = register_jump_address(state, line, address_counter);
            if (label_address < 0) {
                break;
            }
            state->bytecode[++i] = (char)label_address;
            print_opcode_with_arg(line_number, OP_JMP, "JMP", label_address);
        } else if (op_matches(line, "CAL")) {
            state->bytecode[i] = OP_CAL;
            int label_address = register_jump_address(state, line, address_counter);
            if (label_address < 0) {
                break;
            }
            state->bytecode[++i] = (char)label_address;
            print_opcode_with_arg(line_number, OP_CAL, "CAL", label_address);
        } else if (op_matches(line, "RET")) {
            state->bytecode[i] = OP_RET;
            print_opcode(line_number, OP_RET, "RET");

        /*==----- Symbols and Labels -----==*/

        } else if (strncmp(line, "@", 1) == 0) {
            print_label(line_number, line);
            state->address_table[address_counter].address = i;
            strcpy(state->address_table[address_counter].name, line);
            address_counter++;
            i--; // Labels do not consume bytecode space

        } else if (strncmp(line, ".", 1 == 0)) {


        } else {
            printf("Unknown instruction: %s\n", line);
            printf("Aborting.\n");
            // exit(1);
            break;
        }
        line = strtok(NULL, "\n");
        line_number++;
        bytecode_length = i + 1;
    }

    return 0;
}

void get_label_address(struct label* address_table, int address_counter, char* label_name, int* address) {
    for (int i = 0; i < address_counter; i++) {
        if (strcmp(address_table[i].name, label_name) == 0) {
            if (DEBUG) {
                printf("Returning label address %d", address_table[i].address);
            }
            *address = address_table[i].address;
            return;
        }
    }
    *address = -1; // Label not found
}

void print_opcode(int line_number, int opcode, char* name) {
    if (DEBUG) {
        char fmt_line_number[12];
        sprintf(fmt_line_number, "%03d", line_number);
        printf("LINE %s: Opcode %d (%s)\n", fmt_line_number, opcode, name);
    }
}

void print_opcode_with_arg(int line_number, int opcode, char* name, int arg) {
    if (DEBUG) {
        char fmt_line_number[12];
        sprintf(fmt_line_number, "%03d", line_number);
        printf("LINE %s: Opcode %d (%s %d)\n", fmt_line_number, opcode, name, arg);
    }
}

void print_label(int line_number, char* label) {
    if (DEBUG) {
        char fmt_line_number[12];
        sprintf(fmt_line_number, "%03d", line_number);
        printf("LINE %s: Label %s\n", fmt_line_number, label);
    }
}

