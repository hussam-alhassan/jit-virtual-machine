#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "config.h"
#include "types.h"

typedef struct vm_state vm_state;

int assemble_bytecode(char* program, vm_state* state);

void get_label_address(struct label* address_table, int address_counter, char* label_name, int* address);

void print_opcode(int line_number, int opcode, char* name);
void print_opcode_with_arg(int line_number, int opcode, char* name, int arg);
void print_label(int line_number, char* label);

#endif // ASSEMBLER_H
