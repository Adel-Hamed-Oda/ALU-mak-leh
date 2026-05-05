# Assembler

This document describes how the assembler currently works in the project.

## Purpose

The assembler converts one line of assembly text into a single 16-bit instruction word, then loads a whole program from a text file into instruction memory.

## Supported Instructions

The assembler recognizes these mnemonics:

- `ADD`
- `SUB`
- `MUL`
- `LDI`
- `BEQZ`
- `AND`
- `OR`
- `JR`
- `SAL`
- `SAR`
- `LB`
- `SB`

Each instruction is encoded using a 4-bit opcode plus two 6-bit fields.

## Input Format

Assembly lines are split by spaces. The expected format is:

- Most instructions: `OPCODE R1 R2`
- Immediate instructions: `OPCODE R1 IMM`
- Memory instructions: `LB R1 VAR` and `SB R1 VAR`

Registers must be written as `R<number>`, where the number must be between `0` and `63`.

## Encoding

The final instruction word is built like this:

- bits 12-15: opcode
- bits 6-11: first register
- bits 0-5: second register, immediate, or variable index

Fields are masked before packing so extra bits do not corrupt the result.

## Variable Handling

`LB` and `SB` treat the third token as a variable name instead of a register or immediate.

- Variable names are stored in an internal array.
- If a variable is seen for the first time, it is assigned the next free data-memory slot.
- The variable table is reset every time a program is loaded.
- If the variable table runs out of space, assembly fails with an out-of-bounds error.

## Program Loading

When a program file is loaded:

1. Instruction and data memory are cleared.
2. The file is read line by line.
3. Empty lines are skipped.
4. Each non-empty line is assembled into one instruction.
5. The assembled instructions are written into instruction memory.

If the program contains more instructions than instruction memory can hold, loading fails.

## Error Cases

The assembler can fail when:

- the file cannot be opened
- the instruction mnemonic is unknown
- a register name is invalid
- an immediate value is malformed
- a variable slot is unavailable
- instruction memory limits are exceeded

## Notes

- The assembler currently uses a simple space-based tokenizer.
- It expects valid tokens in the correct order and does not try to recover from malformed lines.
- `load_program_from_file` resets state before each load, so previous variables and memory contents do not persist.
