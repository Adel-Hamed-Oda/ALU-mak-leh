# Registers

This document describes the structure and operations of the registers implemented in `register.c`.

## Purpose

The register file manages the internal processor state, including General Purpose Registers (GPRs), the Program Counter (PC), and the Status Register (SREG). It provides interfaces for reading, writing, and debugging these registers.

## Supported Registers

The system models two types of registers based on their bit-width:
- `register_8`: 8-bit registers used for GPRs and the Status Register.
- `register_16`: 16-bit register used solely for the Program Counter (PC).

There are three main components:
- **PC (Program Counter):** A 16-bit register tracks the current instruction address.
- **SREG (Status Register):** An 8-bit register holding condition flags.
- **GPRS (General Purpose Registers):** An array of 8-bit registers representing the processor's main working data set.

## Register Operations

### General Purpose Registers (GPRs)
- **set_register(index, value):** Stores an 8-bit value (masked with `0xFF`) in the GPR at the given `index`. Returns an error if out-of-bounds.
- **get_register(index, out_value):** Retrieves the 8-bit value at the specified GPR `index`.
- **clear_register(index):** Resets the specific GPR to 0. Returns an error if the index is invalid.

### Program Counter (PC)
- **get_pc():** Returns the 16-bit value of the program counter.
- **set_pc(new_pc):** Sets the program counter to a new value (masked with `0xFFFF`).
- **increment_pc():** Increments the PC by 1 and masks it to keep it within 16-bit bounds.

### Status Register (SREG)
- Evaluates operation outcomes using status flags:
  - `C_FLAG (0b00010000)`: Carry
  - `V_FLAG (0b00001000)`: Overflow
  - `N_FLAG (0b00000100)`: Negative
  - `S_FLAG (0b00000010)`: Sign Flag (N XOR V)
  - `Z_FLAG (0b00000001)`: Zero
- **get_flag(flag):** Returns 1 if a specific flag is set, 0 otherwise.
- **update_status_register():** Currently a placeholder intended to be integrated with the instruction parser.

## Debugging

The module includes utilities to dump register states:
- **print_registers():** Prints the contents of all registers (PC, SREG, and all GPRs).
- **print_register(index):** Prints a specific register based on an arbitrary lookup (PC for index 0, SREG for index 1, and GPRs otherwise).

## Error Cases

Interacting with the registers can yield the `OUT_OF_BOUNDS` error if operations target a user-specified register index that is invalid (e.g., beyond `NUMBER_OF_GP_REGISTERS`).
