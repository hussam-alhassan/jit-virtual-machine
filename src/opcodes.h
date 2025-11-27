#ifndef OPCODES_H
#define OPCODES_H

#define OP_HLT 0x00
#define OP_ADD 0x01 
#define OP_SUB 0x02
#define OP_PSH 0x03 // Push
#define OP_POP 0x04 // Pop
#define OP_DUP 0x05 // Duplicate
#define OP_PRT 0x06 // Print
#define OP_STR 0x07 // Store
#define OP_LOD 0x08 // Load
#define OP_JMP 0x09 // Jump
#define OP_CAL 0x10 // Call
#define OP_RET 0x11 // Return

#endif // OPCODES_H