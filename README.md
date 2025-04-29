# CPU Simulator (Pseudo-Assembly Interpreter)

## Description

A simplified CPU simulator and pseudo-assembly interpreter written in C, designed for educational use. It models key CPU components—registers, ALU, memory segments—and executes programs written in a custom pseudo-assembly language.

## Features

- **Register File**: Implements general-purpose registers (AX, BX, CX, DX) and special registers (IP, ZF, SF, SP, BP, ES) using a generic hash map.
- **Memory Management**: Simulates RAM with dynamic segments, free-list management, and allocation strategies (First Fit, Best Fit, Worst Fit).
- **Pseudo-Assembly Parser**: Reads `.DATA` and `.CODE` sections from text files, builds instruction arrays, and resolves labels and variables. 
- **Instruction Set**: Supports MOV, ADD, CMP, JMP, JZ, JNZ, HALT, PUSH, POP, ALLOC, FREE with various addressing modes. 
- **Execution Modes**:
  - **Step-by-Step GUI**: Interactive execution with state preview (`maincpu`).
  - **Batch Executor**: Direct run to completion with final state output (`executor`).

## Requirements

- **Platforms**: macOS, Linux (uses `<regex.h>`, not supported on Windows). 
- **Compiler**: GCC 14.2.0 or compatible C compiler.
- **Build System**: GNU Make

## Build & Run

All targets are managed via the provided `Makefile`. No additional dependencies are required.

```sh
# Build all test programs, GUI, and executor
make all

# Run intermediate tests:
./main1   # HashMap module
./main2   # Memory segments
./main3   # Pseudo-assembler parser
./main4   # CPU module
./main5   # MOV instruction demo
./main6   # Full execution with steps

# Launch interactive GUI:
make maincpu && ./maincpu

# Execute pseudo-assembly program:
make executor && ./executor <filename.txt>
```

## Repository Structure

```
├── C-files              # C source files
├── tests/               # Sample `.txt` pseudo-assembly programs
│   ├── test.txt
│   ├── test2.txt
│   ├── setup_env_test.txt
│   └── test_pseudo_asm.txt
├── Makefile             # Build definitions
└── README.md            # Project documentation
```

## Core Data Structures

- **HashMap**: Open-addressing hash table for registers, labels, and allocated segments. 
- **Segment**: Linked-list node representing a memory segment with `start`, `size`, and `next` pointer. 
- **MemoryHandler**: Manages overall memory array, free-list, and allocated segments map. 
- **Instruction**: Represents one pseudo-assembly instruction with `mnemonic`, `operand1`, `operand2`. 
- **ParserResult**: Holds parsed `.DATA` and `.CODE` arrays, counts, and hash maps for labels and memory locations. 
- **CPU**: The `CPU` struct encapsulates the entire simulated processor state

## Pseudo-Assembly Language

### .DATA Section

Defines variables and constants. Syntax:

```
.DATA
<name> <type> <value1>[,<value2>,...]
```

- `DW`: 32-bit integers
- `DB`: 8-bit integers

Example:

```
.DATA
x DW 42
arr DB 20,21,22,23
```

### .CODE Section

Sequence of instructions. Each on a new line. Labels end with `:`. Operands separated by comma without spaces.

Example:

```
.CODE
MOV AX,5
MOV BX,[x]
loop: ADD CX,BX
CMP AX, CX
JNZ loop
HALT
```

## Instruction Set Overview

| Mnemonic     | Description                                              |
| ------------ | -------------------------------------------------------- |
| MOV dest,src | Copy value from `src` to `dest`                          |
| ADD dest,src | `dest += src`                                            |
| CMP dest,src | Compare, set ZF/SF flags                                 |
| JMP addr     | Unconditional jump                                       |
| JZ addr      | Jump if zero flag set                                    |
| JNZ addr     | Jump if zero flag not set                                |
| PUSH src     | Push value onto stack                                    |
| POP dest     | Pop value from stack                                     |
| ALLOC        | Allocate extra segment (ES) using AX(size), BX(strategy) |
| FREE         | Free the ES segment                                      |
| HALT         | Stop execution                                           |

Refer to source for addressing modes: immediate, register, direct, indirect, segment override. 

## Contributing

Contributions are welcome. Please fork the repository and submit pull requests.

## License

TBD

## Authors

- Vadim Prokhorov
- Haya Termos

