#include "interpreter.h"
#include "jit.h"
#include "opcodes.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int get_jit_operand(jitc* jc) {
    return jc->bytecode[++(jc->bytecode_index)];
}

#define __aarch64__

#ifdef __aarch64__

void emit_psh(jitc* jc) {
    int val = get_jit_operand(jc);
    
    if (DEBUG) {
        printf("JIT emit PSH with operand %d\n", val);
    }

    // TODO: Add support for negative numbers and those larger than 16 bits
    // For now, only support PSHing unsigned vals up to 16 bits

    uint32_t movz_base = 0x52800005;  // movz w5, #0
    uint32_t movz = movz_base | ((uint32_t)val << 5);

    // Reverse bytes (little-endian)
    unsigned char byte_1 = movz & 0xFF;
    unsigned char byte_2 = (movz >> 8) & 0xFF;
    unsigned char byte_3 = (movz >> 16) & 0xFF;
    unsigned char byte_4 = (movz >> 24) & 0xFF;

    unsigned char psh[] = {
        byte_1, byte_2, byte_3, byte_4, // mov w5, #val
        0x05, 0x4c, 0x00, 0xb8,         // str w5, [x0, #4]!
    };

    memcpy(jc->native_code + jc->native_size, psh, sizeof(psh));
    jc->native_size += sizeof(psh);

    return;
}

void emit_add(jitc* jc) {
    if (DEBUG) {
        printf("JIT emit ADD\n");
    }

    unsigned char add[] = {
        0x01, 0xc4, 0x5f, 0xb8, // ldr w1, [x0], #-4 // Basically POP
        0x02, 0xc4, 0x5f, 0xb8, // ldr w2, [x0], #-4 // Basically POP
        0x23, 0x00, 0x02, 0x0b, // add w3, w1, w2 
        0x03, 0x4c, 0x00, 0xb8, // str w3, [x0, #4]! // Basically PSH w3
    };
    
    memcpy(jc->native_code + jc->native_size, add, sizeof(add));
    jc->native_size += sizeof(add);

    return;
}

void emit_prt(jitc* jc) {
    if (DEBUG) {
        printf("JIT emit PRT\n");
    }

    unsigned char prt[] = {
        0x02, 0xc4, 0x5f, 0xb8,
        0x23, 0x00, 0x40, 0xf9,
        0xe0, 0x07, 0xbf, 0xa9,
        0xe0, 0x03, 0x02, 0x2a,
        0x60, 0x00, 0x3f, 0xd6,
        0xe0, 0x07, 0xc1, 0xa8
    };

    memcpy(jc->native_code + jc->native_size, prt, sizeof(prt));
    jc->native_size += sizeof(prt);

    return;
}

void emit_dup(jitc* jc) {
    if (DEBUG) {
        printf("JIT emit DUP\n");
    }

    unsigned char dup[] = {
        0x01, 0x00, 0x40, 0xb9, // ldr w1, [x0]      // Read
        0x01, 0x4c, 0x00, 0xb8  // str w1, [x0, #4]! // Push
    };

    memcpy(jc->native_code + jc->native_size, dup, sizeof(dup));
    jc->native_size += sizeof(dup);

    return;
}

void emit_ret(jitc* jc) {
    if (DEBUG) {
        printf("JIT emit RET\n");
    }

    unsigned char ret[] = {
        0xc0, 0x03, 0x5f, 0xd6  // ret
    };
    
    memcpy(jc->native_code + jc->native_size, ret, sizeof(ret));
    jc->native_size += sizeof(ret);

    return;
}

void emit_str(jitc* jc) {
    int index = get_jit_operand(jc);
    
    if (DEBUG) {
        printf("JIT emit STR with index %d\n", index);
    }

    uint32_t arm64_str_base = 0xb9000025;  // str w5, [x1, #0] 
    uint32_t arm64_str = arm64_str_base | ((uint32_t)index << 10); // 10 because Unsigned offset class 

    // Reverse bytes (little-endian)
    unsigned char byte_1 = arm64_str & 0xFF;
    unsigned char byte_2 = (arm64_str >> 8) & 0xFF;
    unsigned char byte_3 = (arm64_str >> 16) & 0xFF;
    unsigned char byte_4 = (arm64_str >> 24) & 0xFF;

    unsigned char str[] = {
        0x05, 0xc4, 0x5f, 0xb8,         // ldr w5, [x0], #-4     // Pop first operand from stack
        byte_1, byte_2, byte_3, byte_4  // str w5, [x1, #index]  // Store it to an index in function-local memory
    };

    memcpy(jc->native_code + jc->native_size, str, sizeof(str));
    jc->native_size += sizeof(str);

    return;
}

void emit_lod(jitc* jc) {
    int index = get_jit_operand(jc);
    
    if (DEBUG) {
        printf("JIT emit LOD with index %d\n", index);
    }

    uint32_t arm64_lod_base = 0xb9400025; // ldr w5, [x1, #0]
    uint32_t arm64_lod = arm64_lod_base | ((uint32_t)index << 10); // 10 because Unsigned offset class 
    // Reverse bytes (little-endian)
    unsigned char byte_1 = arm64_lod & 0xFF;
    unsigned char byte_2 = (arm64_lod >> 8) & 0xFF;
    unsigned char byte_3 = (arm64_lod >> 16) & 0xFF;
    unsigned char byte_4 = (arm64_lod >> 24) & 0xFF;

    unsigned char lod[] = {     
        byte_1, byte_2, byte_3, byte_4, // ldr w5, [x1, #index]  // Load value from function-local memory // immediate is set by vm per operand
        0x05, 0x4c, 0x00, 0xb8,         // str w5, [x0, #4]!     // Push value onto stack
    };

    memcpy(jc->native_code + jc->native_size, lod, sizeof(lod));
    jc->native_size += sizeof(lod);

    return;
}


#elif defined(__x86_64__)

void emit_psh(unsigned char* native_code, size_t* snippet_size, int val)  {
    return;
}

void emit_add(vm_state* state) {
    // Machine code for add_two (from Section 4):
    unsigned char machine_code[] = {
        0x8B, 0xC7,             // mov eax, edi  (first arg)
        0x03, 0xC6,             // add eax, esi  (second arg)
        0xC3                    // ret
    };
    size_t code_size = sizeof(machine_code);

    write_native_code(state, machine_code, code_size);

    return;
}

#endif

// Native code is emitted on a function/routine basis, ending in a return.
void emit_native_routine(jitc* jc) { // jit compiler
    // Setup jit emission instructions
    emit_function emit_native[256];

    emit_native[OP_PSH] = emit_psh;
    emit_native[OP_ADD] = emit_add;
    emit_native[OP_PRT] = emit_prt;
    emit_native[OP_DUP] = emit_dup;
    emit_native[OP_STR] = emit_str;
    emit_native[OP_LOD] = emit_lod;

    for (; jc->bytecode[jc->bytecode_index] != 0; jc->bytecode_index++) {
        int bytecode_instr = jc->bytecode[jc->bytecode_index];
        emit_native[bytecode_instr](jc);
    }
    emit_ret(jc);
    printf("Emitted main instructions + ret...\n");
}