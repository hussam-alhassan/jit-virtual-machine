# Stack-Based VM w/ JIT

A stack-based bytecode VM with multi-arch JIT compilation. The example Fibonacci sequence program works:

<img width="500" height="310" alt="image" src="https://github.com/user-attachments/assets/8e2d0e12-b371-46fd-94bb-5ffba8670469" />


W.I.P features:
- Tagged Objects
- Multi arch JIT targeting ARM64 & x86_64

Instruction set:

- ADD
- SUB
- PSH
- POP
- STR ; Store
- LOD ; Load
- DUP 
- PRT ; Print value
- ASC ; Print in ASCII mode
- INC
- ; for comments
- @ for addresses
