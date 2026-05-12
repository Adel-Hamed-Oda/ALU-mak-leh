# CSEN601 — Package 4: Double McHarvard with Cheese Circular Shifts
## Complete Implementation Reference for Claude Code

---

## Table of Contents
1. [Project Overview](#1-project-overview)
2. [Architecture Specification](#2-architecture-specification)
3. [Instruction Set Architecture](#3-instruction-set-architecture)
4. [Pipeline / Data Path](#4-pipeline--data-path)
5. [Branch and Jump Behavior](#5-branch-and-jump-behavior)
6. [Data Hazard Handling](#6-data-hazard-handling)
7. [SREG Flag Computation](#7-sreg-flag-computation)
8. [Parsing Rules](#8-parsing-rules)
9. [Output and Printing Requirements](#9-output-and-printing-requirements)
10. [File Structure and Responsibilities](#10-file-structure-and-responsibilities)
11. [Data Types](#11-data-types)
12. [Critical Edge Cases and Important Notes](#12-critical-edge-cases-and-important-notes)

---

## 1. Project Overview

Implement a **Harvard-architecture pipelined processor simulator** in C.

- Read an assembly program from a text file.
- Parse and encode each instruction into its 16-bit binary representation.
- Store encoded instructions in Instruction Memory.
- Simulate a **3-stage pipeline** (IF → ID → EX) cycle by cycle.
- Handle **data hazards** (forwarding/stalling) and **control hazards** (pipeline flush).
- Print detailed per-cycle state and a final dump of all registers and memories.

The project language is **C**. The entry point is `main.c`. The assembly text file path is provided as a command-line argument (or hardcoded path to `programs/test0.txt`).

---

## 2. Architecture Specification

### 2.1 Memory Architecture — Harvard

Harvard architecture uses **completely separate** storage and buses for instructions and data. There is no shared memory.

### 2.2 Instruction Memory

| Property          | Value                          |
|-------------------|--------------------------------|
| Total size        | 1024 rows × 16 bits per row    |
| Addressable unit  | 1 word = 16 bits (2 bytes)     |
| Addressing mode   | Word-addressable               |
| Address range     | 0 to 1023 (0 to 2^10 − 1)     |
| Content           | Encoded 16-bit instructions    |
| C type per cell   | `int16_t` (signed 16-bit)      |

- Instructions are stored starting at address 0.
- Only addresses 0..(num_instructions − 1) are populated; the rest are 0.
- The PC indexes directly into this array (no byte-to-word conversion needed).

### 2.3 Data Memory

| Property          | Value                          |
|-------------------|--------------------------------|
| Total size        | 2048 rows × 8 bits per row     |
| Addressable unit  | 1 word = 1 byte = 8 bits       |
| Addressing mode   | Word/byte-addressable          |
| Address range     | 0 to 2047 (0 to 2^11 − 1)     |
| Content           | Data bytes                     |
| C type per cell   | `int8_t` (signed 8-bit)        |

- All cells initialize to 0.
- LB reads one byte from data memory into a register.
- SB writes one byte from a register into data memory.

### 2.4 Registers

**General-Purpose Registers (GPRs):**

| Property   | Value                    |
|------------|--------------------------|
| Count      | 64                       |
| Names      | R0 through R63           |
| Size       | 8 bits each              |
| C type     | `int8_t`                 |
| Init value | 0                        |

**Status Register (SREG):**

| Property   | Value                                    |
|------------|------------------------------------------|
| Name       | SREG                                     |
| Size       | 8 bits                                   |
| C type     | `uint8_t`                                |
| Init value | 0x00                                     |

SREG bit layout (bit 7 = MSB, bit 0 = LSB):

```
Bit:  7   6   5   4   3   2   1   0
      0   0   0   C   V   N   S   Z
```

- **Bit 4 — C (Carry Flag):** Set when an arithmetic carry/borrow is generated out of the MSB (bit 7) of an unsigned 8-bit operation.
- **Bit 3 — V (Overflow Flag):** Set when the result of a signed operation cannot be represented in 8 bits (sign bit overflow).
- **Bit 2 — N (Negative Flag):** Set when the result is negative (MSB of result = 1).
- **Bit 1 — S (Sign Flag):** Expected sign of result. `S = N XOR V`.
- **Bit 0 — Z (Zero Flag):** Set when the result is exactly zero.
- **Bits 7:5** must **always remain 0**. Never set them.

**Program Counter (PC):**

| Property   | Value                                            |
|------------|--------------------------------------------------|
| Name       | PC                                               |
| Size       | 16 bits                                          |
| C type     | `uint16_t`                                       |
| Init value | 0                                                |
| Purpose    | Holds address of the instruction being fetched   |

- PC is incremented by 1 (one word) during the Fetch stage of each instruction.
- PC is a 16-bit register even though instruction memory only has 1024 rows.

---

## 3. Instruction Set Architecture

### 3.1 Instruction Formats

All instructions are exactly **16 bits** wide. There are two formats:

**R-Format:**
```
| OPCODE (4 bits) | R1 (6 bits) | R2 (6 bits) |
  bits [15:12]      bits [11:6]   bits [5:0]
```

**I-Format:**
```
| OPCODE (4 bits) | R1 (6 bits) | IMMEDIATE (6 bits) |
  bits [15:12]      bits [11:6]   bits [5:0]
```

- OPCODE is always 4 bits, encoding values 0–11.
- R1 and R2 are 6-bit register indices (0–63, selecting R0–R63).
- IMMEDIATE is a 6-bit signed value (two's complement), range −32 to +31.
  - **Exception:** For SAL and SAR, IMMEDIATE is always non-negative (0–31).

### 3.2 Complete Instruction Table

Opcodes are assigned in the order listed (0 through 11):

| # | Opcode (dec) | Opcode (4-bit bin) | Name  | Mnemonic | Type | Assembly Format    | Operation                                   |
|---|-------------|---------------------|-------|----------|------|--------------------|---------------------------------------------|
| 0 | 0           | `0000`              | Add   | ADD      | R    | `ADD R1 R2`        | `R1 = R1 + R2`                              |
| 1 | 1           | `0001`              | Subtract | SUB   | R    | `SUB R1 R2`        | `R1 = R1 - R2`                              |
| 2 | 2           | `0010`              | Multiply | MUL   | R    | `MUL R1 R2`        | `R1 = R1 * R2` (lower 8 bits)              |
| 3 | 3           | `0011`              | Load Immediate | LDI | I  | `LDI R1 IMM`       | `R1 = IMM` (sign-extended to 8 bits)       |
| 4 | 4           | `0100`              | Branch if Equal Zero | BEQZ | I | `BEQZ R1 IMM` | `if (R1 == 0) { PC = PC+1+IMM }` (see §5) |
| 5 | 5           | `0101`              | And   | AND      | R    | `AND R1 R2`        | `R1 = R1 & R2`                              |
| 6 | 6           | `0110`              | Or    | OR       | R    | `OR R1 R2`         | `R1 = R1 \| R2`                             |
| 7 | 7           | `0111`              | Jump Register | JR | R  | `JR R1 R2`         | `PC = R1 \|\| R2` (concatenation, see §5)  |
| 8 | 8           | `1000`              | Shift Arithmetic Left | SAL | I | `SAL R1 IMM` | `R1 = R1 << IMM`                       |
| 9 | 9           | `1001`              | Shift Arithmetic Right | SAR | I | `SAR R1 IMM` | `R1 = R1 >> IMM`                     |
| 10| 10          | `1010`              | Load Byte | LB  | I    | `LB R1 ADDRESS`    | `R1 = DataMem[ADDRESS]`                    |
| 11| 11          | `1011`              | Store Byte | SB | I    | `SB R1 ADDRESS`    | `DataMem[ADDRESS] = R1`                    |

**Notes on specific instructions:**

- **MUL:** Full 8-bit × 8-bit multiplication may produce up to 16 bits. Store only the **lower 8 bits** into R1. N and Z flags are computed from the 8-bit result stored.
- **LDI:** The 6-bit immediate is sign-extended to 8 bits before storing in R1.
- **SAL:** Arithmetic left shift. Shift R1 left by IMM positions. Vacated bits fill with 0. IMM is always ≥ 0.
- **SAR:** Arithmetic right shift. Shift R1 right by IMM positions. Vacated bits fill with the sign bit (MSB). IMM is always ≥ 0.
- **LB / SB:** The 6-bit IMMEDIATE field encodes the data memory address. Treat this field as **unsigned** (0–63) for address computation since it is a memory address.
- **JR:** `PC = R1 || R2`. R1 provides the **most significant byte** (bits 15:8) and R2 provides the **least significant byte** (bits 7:0) of the new 16-bit PC value.

### 3.3 SREG Flag Update Rules

A flag is updated **only** by the instructions listed for it. If an instruction is not listed for a flag, that flag is **not touched** (keeps its previous value).

| Flag | Updated By                                     |
|------|------------------------------------------------|
| C    | ADD                                            |
| V    | ADD, SUB                                       |
| N    | ADD, SUB, MUL, AND, OR, SAL, SAR              |
| S    | ADD, SUB  (`S = N XOR V` after update)        |
| Z    | ADD, SUB, MUL, AND, OR, SAL, SAR              |

---

## 4. Pipeline / Data Path

### 4.1 The Three Stages

All instructions — regardless of type — must pass through **all three stages**:

**Stage 1 — Instruction Fetch (IF):**
- Read the instruction at `InstructionMemory[PC]`.
- Store the current PC value alongside the instruction for later use (especially for BEQZ).
- Increment PC by 1: `PC = PC + 1`.
- Pass the fetched 16-bit instruction to the IF/ID pipeline register.

**Stage 2 — Instruction Decode (ID):**
- Decode the 16-bit instruction into **both** R-format and I-format fields simultaneously:
  - `opcode  = (instruction >> 12) & 0xF`   (bits 15:12)
  - `r1      = (instruction >> 6) & 0x3F`   (bits 11:6)
  - `r2      = instruction & 0x3F`           (bits 5:0, R-format)
  - `imm_raw = instruction & 0x3F`           (bits 5:0, I-format, raw 6 bits)
  - `imm_signed = sign_extend_6_to_8(imm_raw)` (sign-extend for non-shift I-format)
- Read the values of R1 (and R2 for R-format) from the register file.
- Pass all decoded fields and read register values to the ID/EX pipeline register.

**Stage 3 — Execute (EX):**
- Perform the ALU operation or memory access.
- For ALU instructions (ADD, SUB, MUL, AND, OR, SAL, SAR): compute the result and update SREG flags accordingly.
- For LDI: write the sign-extended immediate to R1.
- For LB: read `DataMemory[ADDRESS]` and write to R1.
- For SB: write R1 value to `DataMemory[ADDRESS]`.
- For BEQZ: check condition and update PC if taken (see §5).
- For JR: compute new PC and update (see §5).
- Write the result back to the destination register (R1) in the same EX stage (no separate WB stage).

### 4.2 Pipeline Timing

With 3 stages and no separate Memory/WB stages, the pipeline issues one instruction per cycle (after fill):

```
Clock Cycle Formula:  Total cycles = 3 + ((n − 1) × 1)
                      where n = number of instructions that enter the pipeline
```

Example with 7 instructions → `3 + 6 = 9` clock cycles.

### 4.3 Pipeline Diagram (normal flow, no hazards)

```
Cycle:  1       2       3       4       5       6       7       8       9
I1:     IF      ID      EX
I2:             IF      ID      EX
I3:                     IF      ID      EX
I4:                             IF      ID      EX
I5:                                     IF      ID      EX
I6:                                             IF      ID      EX
I7:                                                     IF      ID      EX
```

### 4.4 Pipeline Registers (inter-stage buffers)

Implement two pipeline registers as structs:

**IF/ID Register:**
```c
typedef struct {
    int16_t  instruction;   // raw 16-bit encoded instruction
    uint16_t pc;            // PC value at the time of fetch (before increment)
    int      valid;         // 1 = holds real instruction, 0 = bubble/flush
} IF_ID_Register;
```

**ID/EX Register:**
```c
typedef struct {
    int      opcode;
    int      r1_index;      // destination / source register index
    int      r2_index;      // source register index (R-format)
    int8_t   r1_val;        // value read from R1
    int8_t   r2_val;        // value read from R2 (R-format)
    int8_t   imm_signed;    // sign-extended 6-bit immediate (I-format)
    uint8_t  imm_unsigned;  // raw unsigned 6-bit immediate (for shifts / addresses)
    uint16_t pc;            // PC at fetch time (for BEQZ: PC+1+IMM calculation)
    int      valid;         // 1 = holds real instruction, 0 = bubble/flush
} ID_EX_Register;
```

---

## 5. Branch and Jump Behavior

### 5.1 BEQZ — Branch if Equal Zero

- **Format:** `BEQZ R1 IMM` (I-format, opcode 4)
- **Condition evaluated in EX stage.**
- If `R1 == 0`:
  - `PC = (fetch_PC_of_BEQZ) + 1 + sign_extended_IMM`
  - `fetch_PC_of_BEQZ` = the PC value stored when BEQZ was fetched (before the +1 increment that happened in IF). So `fetch_PC_of_BEQZ + 1` is the address of the instruction immediately after BEQZ.
  - Branch is **taken**: flush the pipeline (see §5.3).
- If `R1 != 0`:
  - Branch is **not taken**: continue normally, no flush.

**Example:**
```
Address 0: BEQZ R1 5    → if(R1==0): PC = 0 + 1 + 5 = 6
Address 1: ADD R2 R3    → (in pipeline during BEQZ EX, will be flushed if taken)
Address 2: ADD R4 R5    → (in pipeline during BEQZ EX, will be flushed if taken)
```
If taken, next fetch is from address 6.

### 5.2 JR — Jump Register

- **Format:** `JR R1 R2` (R-format, opcode 7)
- **Always taken (unconditional).**
- In EX stage: `PC = (R1_value << 8) | (R2_value & 0xFF)`
  - R1 value forms the **upper byte** (bits 15:8) of the new PC.
  - R2 value forms the **lower byte** (bits 7:0) of the new PC.
- Flush the pipeline (see §5.3).

**Example:**
```
Address 0: JR R0 R1     → PC = (R0_val << 8) | (R1_val & 0xFF)
                           Since R0=0, R1=0 initially: PC = 0x0000 = 0
```

### 5.3 Control Hazard — Pipeline Flush (Packages 3 & 4 Rule)

When a branch is **taken** (BEQZ condition true) or a jump executes (JR):

1. In the **EX stage**, compute the target PC and update `PC`.
2. **Immediately flush** (invalidate / set `valid=0`) the instructions currently in:
   - The **IF stage** (in the IF/ID pipeline register)
   - The **ID stage** (in the ID/EX pipeline register)
3. In the **very next clock cycle** after EX, fetch the target instruction (the pipeline register for ID is now a bubble, and the pipeline register for IF is now a bubble from the flush; fetch begins at the new PC).

**Timeline for Packages 3 & 4 (3-stage pipeline):**
```
Cycle X:    BEQZ/JR in EX  |  Inst+1 in ID  |  Inst+2 in IF
            → branch resolved, flush ID and IF
Cycle X+1:  [bubble] in ID  |  Target in IF  |  (EX is empty)
Cycle X+2:  Target in ID    |  Target+1 in IF
Cycle X+3:  Target in EX    | ...
```

Specifically: after the branch/jump EX cycle, the **next** cycle fetches the branch target. The two instructions that were in the pipeline (in IF and ID at the time of EX) are **dropped** — they do not execute, do not modify registers, and do not modify memory.

---

## 6. Data Hazard Handling

### 6.1 The Hazard

In the 3-stage pipeline (IF → ID → EX), a data hazard occurs when:

- Instruction N is in **EX** (computing its result, writing to R_dest).
- Instruction N+1 is in **ID** (reading R_dest before N has written back).

Since write-back happens at the end of EX, the result is not yet in the register file when N+1 reads it in ID.

### 6.2 Required Solution — Stalling or Forwarding

With the IF→ID→EX simulation order, when `decode(old_if_id)` runs, `execute(old_id_ex)` has not yet run this cycle. So if the instruction in `old_id_ex` writes a register that the instruction in `old_if_id` reads, the register file does not yet have the correct value.

**Stalling approach:**

In `decode()`, before reading registers, check whether `old_id_ex` will write to a register that the current instruction reads. If so, insert a bubble into `id_ex`, undo the PC increment, and restore `if_id` so the same instruction is decoded again next cycle.

```c
if (old_id_ex.valid && instruction_writes_register(old_id_ex.opcode)) {
    int dest = old_id_ex.r1_index;
    if ((instruction_reads_r1(decoded_opcode) && decoded_r1 == dest) ||
        (is_r_format(decoded_opcode) && decoded_r2 == dest)) {
        pipeline_flush_id_ex();
        if_id = old_if_id;   // keep same instruction in IF/ID next cycle
        pc--;                // undo this cycle's PC increment
        return;
    }
}
```

**Forwarding approach (alternative):**

Run `execute(old_id_ex)` first within the cycle to produce the result, then pass that result into `decode()` for use instead of the register file value. This requires either a global `last_ex_result`/`last_ex_dest` pair or passing the result as a parameter.

### 6.3 No Hazard for Non-Modifying Instructions

SB does not write to any register. BEQZ does not write to any register (unless branch not taken and R1 happens to equal R1 — but BEQZ never writes a result to a GPR). JR does not write to any GPR.

---

## 7. SREG Flag Computation

### 7.1 Carry Flag (C) — ADD only

The carry flag checks whether the 9th bit (bit 8) of the **unsigned** addition result is set.

```c
// R1 and R2 are int8_t (signed 8-bit); treat them as unsigned for carry
int temp1 = (uint8_t)r1_val;         // zero-extend to int
int temp2 = (uint8_t)r2_val;         // zero-extend to int
int result_unsigned = temp1 + temp2;

// Check bit 8 (the 9th bit)
#define CARRY_MASK 0x100
int carry = ((result_unsigned & CARRY_MASK) == CARRY_MASK) ? 1 : 0;
```

The 8-bit result stored in R1 is `(int8_t)(result_unsigned & 0xFF)`.

### 7.2 Overflow Flag (V) — ADD and SUB

Overflow occurs when the mathematical result cannot be represented in 8 signed bits.

**Method: XOR of the last two carry bits**

For an 8-bit operation, XOR the carry-into-bit-7 and the carry-out-of-bit-7:
- `carry_into_7` = carry from bit 6 to bit 7 during the addition/subtraction
- `carry_out_of_7` = carry out of bit 7 (the overall carry bit = C above, for ADD)
- `V = carry_into_7 XOR carry_out_of_7`

**Practical C implementation:**

```c
// For ADD: a + b
int8_t a = r1_val, b = r2_val;
int8_t result = a + b;
// Overflow: both operands same sign but result has opposite sign
int V = 0;
if (((a >= 0) && (b >= 0) && (result < 0)) ||
    ((a < 0)  && (b < 0)  && (result >= 0))) {
    V = 1;
}

// For SUB: a - b  (equivalent to a + (-b))
int8_t result_sub = a - b;
// Overflow: different signs of operands, result has same sign as subtrahend (b)
int V_sub = 0;
if (((a >= 0) && (b < 0) && (result_sub < 0)) ||
    ((a < 0)  && (b >= 0) && (result_sub >= 0))) {
    V_sub = 1;
}
```

Alternatively, use the XOR-of-last-two-carries approach by computing with wider integer types and extracting the relevant bits.

### 7.3 Negative Flag (N)

```c
int N = (result_8bit < 0) ? 1 : 0;
// Or equivalently: N = ((result_8bit >> 7) & 1);
```

N is updated by: ADD, SUB, MUL, AND, OR, SAL, SAR.

### 7.4 Sign Flag (S)

```c
int S = N ^ V;
```

S is updated by: ADD, SUB (always after N and V are computed).

### 7.5 Zero Flag (Z)

```c
int Z = (result_8bit == 0) ? 1 : 0;
```

Z is updated by: ADD, SUB, MUL, AND, OR, SAL, SAR.

### 7.6 Updating SREG

When updating SREG, use bit masking to set/clear individual flags while keeping bits 7:5 = 0:

```c
// Set individual bits
void sreg_set_bit(uint8_t *sreg, int bit_position, int value) {
    if (value)
        *sreg |= (1 << bit_position);
    else
        *sreg &= ~(1 << bit_position);
}
// Bit positions: C=4, V=3, N=2, S=1, Z=0
// Always ensure bits 7:5 remain 0 after any update
*sreg &= 0x1F;   // mask to keep only bits 4:0
```

---

## 8. Parsing Rules

### 8.1 Input File Format

- Plain text file, one instruction per line.
- Tokens on each line are separated by whitespace.
- Register tokens: `R0`, `R1`, ..., `R63` (case-insensitive acceptable, standardize to uppercase).
- Immediate tokens: decimal integers (positive or negative).
- First token on each line is the mnemonic.

**Example `programs/test0.txt`:**
```
LDI R1 5
LDI R2 10
ADD R1 R2
SB R1 0
LB R3 0
```

### 8.2 Parsing to Binary — Critical Rule

**You are NOT allowed to keep the fields as separate variables across stages.** The parsed instruction must be:

1. Encoded into a single **16-bit integer** (`int16_t`) immediately after parsing.
2. Stored as that 16-bit integer in instruction memory.
3. During the **ID stage**, re-decoded from the 16-bit integer into both R-format and I-format fields simultaneously, without relying on any helper data stored during parsing.

**Encoding process (assembler):**

```c
int16_t encode_instruction(int opcode, int field1, int field2) {
    // For R-format: field1=r1_index, field2=r2_index
    // For I-format: field1=r1_index, field2=immediate (6-bit signed)
    int16_t instr = 0;
    instr |= (opcode & 0xF) << 12;    // bits 15:12
    instr |= (field1 & 0x3F) << 6;    // bits 11:6
    instr |= (field2 & 0x3F);          // bits 5:0
    return instr;
}
```

For immediate encoding, the 6-bit two's complement of the signed value is stored:
```c
// imm is an int (e.g., -3, +5)
int imm_6bit = imm & 0x3F;  // take lower 6 bits (handles negative via two's complement)
```

**Decoding process (ID stage):**

```c
int16_t raw = if_id_reg.instruction;
int opcode    = (raw >> 12) & 0xF;
int r1_index  = (raw >> 6) & 0x3F;
int r2_index  = raw & 0x3F;           // R-format interpretation
int imm_raw   = raw & 0x3F;           // I-format interpretation (unsigned 6-bit)
// Sign-extend 6-bit to 8-bit:
int8_t imm_signed = (imm_raw & 0x20) ? (int8_t)(imm_raw - 64) : (int8_t)(imm_raw);
// For shifts: use imm_raw directly (unsigned, always positive)
```

### 8.3 Validator

Before encoding, validate each instruction:
- Mnemonic must be one of the 12 known mnemonics.
- Register indices must be 0–63.
- Immediate values must fit in a 6-bit signed range (−32 to +31) for non-shift instructions.
- Shift immediates must be 0–31.
- LB/SB address must be 0–63 (6-bit unsigned).
- Report errors clearly; do not proceed with invalid programs.

### 8.4 Assembly Parsing Flow

```
Text file → validator → assembler (encodes to int16_t) → InstructionMemory[]
                                                               ↓
                                                       Simulation begins
```

---

## 9. Output and Printing Requirements

Print to **stdout** after every clock cycle, and a final dump after all cycles complete.

### 9.1 Per-Cycle Output

For each clock cycle, print:

```
Clock Cycle #<N>:
```

Then for each active pipeline stage (IF, ID, EX), print:

```
  Stage IF:
    Instruction: <mnemonic and operands or "No instruction / Bubble">
    Input: PC = <address>
    Output: Fetched instruction = <16-bit binary or hex>

  Stage ID:
    Instruction: <mnemonic and operands>
    Input: Instruction bits = <binary>, R<n> = <value>, R<m> = <value>
    Output: opcode=<val>, R1_index=<val>, R1_val=<val>, R2_index=<val>, R2_val=<val>, IMM=<val>

  Stage EX:
    Instruction: <mnemonic and operands>
    Input: <all relevant values entering EX>
    Output: Result = <value>  [or: Branch taken/not taken, PC updated to <addr>]
```

If a stage contains a bubble (flushed or empty), print `"No instruction (bubble)"` for that stage.

### 9.2 Register Change Notifications

Whenever a register value is modified (in the EX stage), print **immediately** after the EX stage output:

```
  [EX] Register R<n> updated: <old_value> → <new_value>
  [EX] SREG updated: 0x<old_hex> → 0x<new_hex>  (C=<c>, V=<v>, N=<n>, S=<s>, Z=<z>)
```

### 9.3 Memory Change Notifications

Whenever data memory is written (SB instruction in EX):

```
  [EX] Data Memory[<address>] updated: <old_value> → <new_value>
```

### 9.4 Final State Dump (after last cycle)

Print all of the following:

```
=== Final Register State ===
R0  = <value>
R1  = <value>
...
R63 = <value>
PC   = <value>
SREG = 0x<hex> (C=<c>, V=<v>, N=<n>, S=<s>, Z=<z>)

=== Final Instruction Memory ===
[  0]: <16-bit binary or hex>  ; <decoded mnemonic>
[  1]: <16-bit binary or hex>  ; <decoded mnemonic>
...
[1023]: 0000000000000000
(Print all 1024 entries, or at minimum all non-zero entries plus a note about the rest being 0)

=== Final Data Memory ===
[   0]: <value>
[   1]: <value>
...
[2047]: <value>
(Print all 2048 entries, or at minimum all non-zero entries plus a note)
```

### 9.5 Stopping Condition

The simulation stops when **there are no more instructions to fetch** — that is, when the PC points beyond the last loaded instruction address **and** the pipeline is empty (all pipeline registers are invalid/bubbles).

Do **not** pre-calculate the total number of cycles and use it as the stopping condition. The loop runs cycle by cycle, checking whether any stage has a valid instruction remaining.

---

## 10. File Structure and Responsibilities

```
.
├── CLAUDE.md                          ← This file
├── Makefile                           ← Build system
├── main.c                             ← Entry point
├── constants.h                        ← Global constants
├── errors.h                           ← Error codes and messages
├── includes.h                         ← Common includes (stdint.h, stdio.h, etc.)
├── programs/
│   └── test0.txt                      ← Assembly test program
├── src/
│   ├── memory/
│   │   ├── instruction_memory.h/.c    ← Instruction memory (1024×16-bit)
│   │   ├── data_memory.h/.c           ← Data memory (2048×8-bit)
│   ├── registers/
│   │   └── register.h/.c             ← GPR file (R0–R63), SREG, PC
│   ├── instructions/
│   │   ├── parser.h/.c               ← Decode 16-bit instruction into fields
│   │   └── instruction_manager.h/.c  ← Opcode definitions, mnemonic↔opcode mapping
│   ├── programs/
│   │   ├── assembler.h/.c            ← Read text file, encode to int16_t, load to InstrMem
│   │   └── validator.h/.c            ← Validate assembly syntax and operand ranges
│   └── general_utils/
│       └── utils.h/.c                ← Shared helpers (sign extension, binary string print, etc.)
```

### 10.1 `constants.h`

```c
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define INSTR_MEM_SIZE      1024
#define DATA_MEM_SIZE       2048
#define NUM_REGISTERS       64
#define INSTR_BITS          16
#define REG_BITS            8
#define PC_BITS             16

// Instruction format bit widths
#define OPCODE_BITS         4
#define REG_FIELD_BITS      6
#define IMM_FIELD_BITS      6

// SREG bit positions
#define SREG_C_BIT          4
#define SREG_V_BIT          3
#define SREG_N_BIT          2
#define SREG_S_BIT          1
#define SREG_Z_BIT          0
#define SREG_MASK           0x1F   // bits 4:0 only

// Opcodes
#define OP_ADD   0
#define OP_SUB   1
#define OP_MUL   2
#define OP_LDI   3
#define OP_BEQZ  4
#define OP_AND   5
#define OP_OR    6
#define OP_JR    7
#define OP_SAL   8
#define OP_SAR   9
#define OP_LB    10
#define OP_SB    11

#define NUM_OPCODES         12

#endif
```

### 10.2 `src/memory/instruction_memory.h`

```c
#ifndef INSTRUCTION_MEMORY_H
#define INSTRUCTION_MEMORY_H

#include <stdint.h>
#include "../../constants.h"

extern int16_t instruction_memory[INSTR_MEM_SIZE];

void     instr_mem_init(void);
void     instr_mem_write(int address, int16_t instruction);
int16_t  instr_mem_read(int address);
void     instr_mem_dump(void);

#endif
```

### 10.3 `src/memory/data_memory.h`

```c
#ifndef DATA_MEMORY_H
#define DATA_MEMORY_H

#include <stdint.h>
#include "../../constants.h"

extern int8_t data_memory[DATA_MEM_SIZE];

void    data_mem_init(void);
void    data_mem_write(int address, int8_t value);
int8_t  data_mem_read(int address);
void    data_mem_dump(void);

#endif
```

### 10.4 `src/registers/register.h`

```c
#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>
#include "../../constants.h"

extern int8_t   gpr[NUM_REGISTERS];   // R0–R63
extern uint8_t  sreg;                 // Status register (bits 4:0 = C,V,N,S,Z)
extern uint16_t pc;                   // Program counter (16-bit)

void    reg_init(void);
int8_t  reg_read(int index);
void    reg_write(int index, int8_t value);  // no-op if index out of range

// SREG helpers
void    sreg_set_flag(int bit_pos, int value);
int     sreg_get_flag(int bit_pos);
void    sreg_dump(void);
void    reg_dump_all(void);

#endif
```

### 10.5 `src/instructions/instruction_manager.h`

```c
#ifndef INSTRUCTION_MANAGER_H
#define INSTRUCTION_MANAGER_H

#include <stdint.h>

// Decoded instruction representation (all possible fields)
typedef struct {
    int      opcode;
    int      r1_index;
    int      r2_index;
    int8_t   r1_val;
    int8_t   r2_val;
    int8_t   imm_signed;     // sign-extended 6-bit immediate
    uint8_t  imm_unsigned;   // raw 6-bit immediate (for shifts/addresses)
    uint16_t fetch_pc;       // PC at time of fetch (before increment)
    int      valid;          // 1 = real instruction, 0 = bubble
} DecodedInstruction;

// Pipeline registers
extern DecodedInstruction if_id;    // Between IF and ID
extern DecodedInstruction id_ex;    // Between ID and EX

// Mnemonics table
extern const char *MNEMONIC[12];

// Format identification (R or I)
int is_r_format(int opcode);
int is_i_format(int opcode);
int instruction_writes_register(int opcode);   // does this instruction write to R1?
int get_dest_register(DecodedInstruction *d);  // returns r1_index if it writes, else -1

// Pipeline register operations
void pipeline_init(void);
void pipeline_flush_if_id(void);
void pipeline_flush_id_ex(void);

#endif
```

### 10.6 `src/instructions/parser.h`

```c
#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

// Decode a raw 16-bit instruction into all fields (both formats simultaneously)
// Returns a DecodedInstruction with all fields populated
#include "instruction_manager.h"

DecodedInstruction decode_instruction(int16_t raw, uint16_t fetch_pc);

// Sign-extend a 6-bit value to 8 bits
int8_t sign_extend_6to8(int raw_6bit);

// Encode fields into a 16-bit instruction
int16_t encode_instruction(int opcode, int field1, int field2);

#endif
```

### 10.7 `src/programs/assembler.h`

```c
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

// Read assembly file, validate, encode, and load into instruction memory.
// Returns number of instructions loaded, or -1 on error.
int assemble_program(const char *filepath);

// Look up opcode by mnemonic string (case-insensitive)
// Returns -1 if not found
int mnemonic_to_opcode(const char *mnemonic);

// Parse a register token "R<n>" and return the index n
// Returns -1 on parse failure
int parse_register(const char *token);

// Parse an integer immediate from string
// Returns 1 on success, 0 on failure; writes to *out
int parse_immediate(const char *token, int *out);

#endif
```

### 10.8 `src/programs/validator.h`

```c
#ifndef VALIDATOR_H
#define VALIDATOR_H

// Validate a single line of assembly.
// Returns 1 if valid, 0 if invalid (and prints an error message).
int validate_line(const char *line, int line_number);

// Validate that a register index is in range [0, 63]
int validate_register_index(int index);

// Validate a signed immediate fits in 6 bits [-32, 31]
int validate_imm_signed(int imm);

// Validate a shift immediate fits in [0, 31]
int validate_imm_shift(int imm);

// Validate an address immediate fits in [0, 63] (unsigned 6-bit)
int validate_imm_address(int addr);

#endif
```

### 10.9 `src/general_utils/utils.h`

```c
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// Convert a 16-bit value to a binary string (null-terminated, 17 chars)
void int16_to_binary_str(int16_t val, char *buf);

// Convert an 8-bit value to a binary string (null-terminated, 9 chars)
void int8_to_binary_str(int8_t val, char *buf);

// String to uppercase in-place
void str_to_upper(char *s);

// Trim leading and trailing whitespace from s (in-place)
void str_trim(char *s);

// Returns 1 if string is blank/empty, 0 otherwise
int str_is_blank(const char *s);

#endif
```

### 10.10 `main.c` Responsibilities

```
1. Accept the assembly file path (argv[1] or default "programs/test0.txt").
2. Call reg_init(), instr_mem_init(), data_mem_init(), pipeline_init().
3. Call assemble_program(filepath) → loads instructions into InstructionMemory.
4. Run the main simulation loop:
      while (pipeline has valid instruction OR PC < num_instructions):
          cycle()     // one clock tick
          cycle_count++
5. Print final state: reg_dump_all(), sreg_dump(), instr_mem_dump(), data_mem_dump().
```

### 10.11 Simulation Loop Structure

```c
void cycle(void) {
    // Snapshot BEFORE any stage modifies the registers
    IF_ID_Register  old_if_id = if_id;
    ID_EX_Register  old_id_ex = id_ex;

    fetch();           // writes fresh if_id
    decode(old_if_id); // reads last cycle's if_id, writes fresh id_ex
    execute(old_id_ex);// reads last cycle's id_ex, writes results
}
```

**Why EX before ID before IF:** In a real pipeline, stages are parallel, but in simulation you want EX to produce its result first so that the forwarding unit in ID can use it. Fetch is last because it doesn't depend on the others.

---

## 11. Data Types

Strict data type requirements from the project specification:

| Thing                        | C Type         | Reason                          |
|------------------------------|----------------|---------------------------------|
| Instruction (16-bit)         | `int16_t`      | Must be exactly 16-bit signed   |
| Register value (8-bit)       | `int8_t`       | Must be exactly 8-bit signed    |
| Data memory cell (8-bit)     | `int8_t`       | Must be exactly 8-bit signed    |
| SREG                         | `uint8_t`      | 8-bit unsigned (flags)          |
| PC                           | `uint16_t`     | 16-bit unsigned address         |
| Instruction memory address   | `int` or `uint16_t` | 0–1023                    |
| Data memory address          | `int` or `uint16_t` | 0–2047                    |
| Register index               | `int`          | 0–63                            |
| Opcode                       | `int`          | 0–11                            |
| Intermediate arithmetic      | `int`          | Wider than 8-bit for carry/overflow detection |

Include `<stdint.h>` everywhere. The `includes.h` file should contain:

```c
#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#endif
```

---

## 12. Critical Edge Cases and Important Notes

### 12.1 PC Storage During Fetch

When an instruction is fetched from `InstructionMemory[PC]`, **store the current PC value** (before incrementing) in the IF/ID pipeline register. This is needed so that BEQZ can compute `stored_PC + 1 + IMM`.

### 12.2 Decode Into All Possible Formats

During ID, always extract both:
- R-format fields: `r1_index`, `r2_index`
- I-format fields: `r1_index`, `imm_signed`, `imm_unsigned`

The opcode tells you which fields to use for execution, but you must decode all fields at decode time. Do not carry any annotation from the parsing stage.

### 12.3 Immediate Sign Extension for LDI

The 6-bit immediate in LDI is sign-extended to 8 bits before writing to R1:
```c
int8_t value = sign_extend_6to8(imm_raw);
reg_write(r1_index, value);
```

### 12.4 MUL Result Truncation

`R1 = R1 * R2` may produce values beyond 8-bit range. Store only the lower 8 bits:
```c
int product = (int)r1_val * (int)r2_val;
int8_t result = (int8_t)(product & 0xFF);
reg_write(r1_index, result);
```
N and Z flags use `result` (the 8-bit truncated value).

### 12.5 SAR Sign Behavior

Arithmetic right shift must propagate the sign bit. In C, right-shifting a signed `int8_t` is implementation-defined, but in practice (and for this project) it is arithmetic. To be safe:
```c
int8_t val = id_ex.r1_val;
int8_t result = (int8_t)((int8_t)val >> imm);
// If IMM >= 8, result is either 0x00 or 0xFF depending on sign
```

### 12.6 Shift Amount Out of Range

If shift amount ≥ 8 for SAL: result is 0 (all bits shifted out).
If shift amount ≥ 8 for SAR: result is 0x00 (if positive) or 0xFF (if negative).

### 12.7 BEQZ Uses fetch_pc, Not Current PC

At the time BEQZ reaches EX, the PC has already been incremented twice (once for BEQZ itself and once for the next fetch). Always use the `fetch_pc` value stored in the pipeline register, not the current value of the `pc` global variable.

```c
// In EX for BEQZ:
uint16_t branch_target = (uint16_t)((int)(id_ex.fetch_pc) + 1 + (int)id_ex.imm_signed);
if (id_ex.r1_val == 0) {
    pc = branch_target;
    pipeline_flush_if_id();
    pipeline_flush_id_ex();
}
```

### 12.8 JR is Unconditional — Always Flush

JR always jumps. Always flush the pipeline after JR reaches EX.

### 12.9 Flushed Instructions Must Not Write Back

When an instruction is flushed (valid = 0), the EX stage must check `id_ex.valid` before performing any write-back to registers or data memory. A bubble must have zero side effects.

```c
void execute(void) {
    if (!id_ex.valid) {
        // Print "Stage EX: No instruction (bubble)"
        return;
    }
    // ... normal execution
}
```

### 12.10 Stopping Condition

```c
int simulation_done(int num_instructions_loaded) {
    // Stop when:
    // - PC is past the last instruction, AND
    // - Both pipeline registers are invalid (no instructions in flight)
    return (pc >= num_instructions_loaded)
        && (!if_id.valid)
        && (!id_ex.valid);
}
```

Loop: `while (!simulation_done(n)) { cycle(); }`

### 12.11 Data Hazard Detection Example (Stalling)

```c
void decode(IF_ID_Register old_if_id) {
    if (!old_if_id.valid) {
        pipeline_flush_id_ex();
        return;
    }

    // Decode fields from old_if_id
    int opcode   = (old_if_id.instruction >> 12) & 0xF;
    int r1_index = (old_if_id.instruction >> 6) & 0x3F;
    int r2_index =  old_if_id.instruction & 0x3F;

    // Stall check: if old_id_ex will write a register this instruction reads
    if (old_id_ex.valid && instruction_writes_register(old_id_ex.opcode)) {
        int dest = old_id_ex.r1_index;
        int stall = 0;
        if (instruction_reads_r1(opcode) && r1_index == dest) stall = 1;
        if (is_r_format(opcode) && r2_index == dest)          stall = 1;

        if (stall) {
            pipeline_flush_id_ex();  // insert bubble into EX next cycle
            if_id = old_if_id;       // re-present same instruction to ID next cycle
            pc--;                    // undo PC increment from this cycle's fetch
            return;
        }
    }

    // No hazard — read registers and fill id_ex normally
    // ...
}
```

### 12.12 Instructions That Read vs. Write R1

| Instruction | Reads R1? | Reads R2? | Writes R1? |
|-------------|-----------|-----------|------------|
| ADD         | Yes       | Yes       | Yes        |
| SUB         | Yes       | Yes       | Yes        |
| MUL         | Yes       | Yes       | Yes        |
| LDI         | No        | No        | Yes        |
| BEQZ        | Yes       | No        | No         |
| AND         | Yes       | Yes       | Yes        |
| OR          | Yes       | Yes       | Yes        |
| JR          | Yes (PC)  | Yes (PC)  | No         |
| SAL         | Yes       | No        | Yes        |
| SAR         | Yes       | No        | Yes        |
| LB          | No        | No        | Yes        |
| SB          | Yes (data)| No        | No         |

### 12.13 LB/SB Address Field

For LB and SB, the 6-bit immediate field holds the data memory address. Treat it as **unsigned** (0–63). Do not sign-extend it for address computation.

```c
int address = (int)(id_ex.imm_unsigned);  // 0–63
```

### 12.14 Instruction Memory is Word-Addressable

`InstructionMemory[0]` = first instruction, `InstructionMemory[1]` = second instruction, etc. PC increments by 1 per instruction, not by 2 (no byte-addressing conversion needed for instruction memory).

### 12.15 Output Format for Binary Instructions

When printing an instruction in binary (for IF output), use a 16-character binary string. For example, the instruction `LDI R1 5` encodes as opcode=3, r1=1, imm=5:
```
0011 000001 000101  →  "0011000001000101"
```

---

## 13. Makefile

The project should build with a simple `make` command:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = processor

SRCS = main.c \
       src/memory/instruction_memory.c \
       src/memory/data_memory.c \
       src/registers/register.c \
       src/instructions/instruction_manager.c \
       src/instructions/parser.c \
       src/programs/assembler.c \
       src/programs/validator.c \
       src/general_utils/utils.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) programs/test0.txt
```

---

## 14. Sample Assembly Program and Expected Behavior

File: `programs/test0.txt`

```
LDI R1 5
LDI R2 3
ADD R1 R2
SB R1 0
LB R3 0
```

Expected behavior trace (with stall handling):

- Cycle 1: IF fetches `LDI R1 5`; ID: bubble; EX: bubble
- Cycle 2: IF fetches `LDI R2 3`; ID decodes `LDI R1 5`; EX: bubble
- Cycle 3: IF fetches `ADD R1 R2`; ID decodes `LDI R2 3`; EX executes `LDI R1 5` → R1=5
- Cycle 4: IF fetches `SB R1 0`; ID decodes `ADD R1 R2` — **STALL**: ADD reads R2, but LDI R2 3 is in EX right now and hasn't written back yet. Insert bubble, freeze if_id, undo PC.
- Cycle 5: IF re-fetches `SB R1 0` (PC restored); ID decodes `ADD R1 R2` again (no hazard now, R2=3 is in regfile); EX executes `LDI R2 3` → R2=3
- Cycle 6: IF fetches `LB R3 0`; ID decodes `SB R1 0` — **STALL**: SB reads R1, but ADD is in EX right now. Insert bubble, freeze if_id, undo PC.
- Cycle 7: IF re-fetches `LB R3 0`; ID decodes `SB R1 0` again (no hazard, R1=8 in regfile); EX executes `ADD R1 R2` → R1=8
- Cycle 8: IF: no more instructions; ID decodes `LB R3 0`; EX executes `SB R1 0` → DataMem[0]=8
- Cycle 9: IF: bubble; ID: bubble; EX executes `LB R3 0` → R3=8

Total cycles: 9 (7 base + 2 stall cycles).

---

*End of CLAUDE.md — Package 4: Double McHarvard with Cheese Circular Shifts*
