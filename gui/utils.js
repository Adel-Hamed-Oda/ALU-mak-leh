const OPCODE_NAMES = [
    "ADD",
    "SUB",
    "MUL",
    "LDI",
    "BEQZ",
    "AND",
    "OR",
    "JR",
    "SAL",
    "SAR",
    "LB",
    "SB"
];

const REGISTER_OPERAND_OPCODES = new Set([
    "ADD",
    "SUB",
    "MUL",
    "AND",
    "OR",
    "JR"
]);

const SIGNED_IMMEDIATE_OPCODES = new Set([
    "LDI",
    "BEQZ",
    "LB",
    "SB"
]);

const END_OF_PROGRAM_INSTRUCTION = 0xffff;

function signExtend6(value) {
    return (value & 0x20) ? value - 0x40 : value;
}

function decodeInstruction(encodedInstruction) {
    const instruction = Number(encodedInstruction) & 0xffff;

    if (instruction === END_OF_PROGRAM_INSTRUCTION) {
        return "return 0";
    }

    const opcode = (instruction >>> 12) & 0x0f;
    const r1 = (instruction >>> 6) & 0x3f;
    const r2Imm = instruction & 0x3f;
    const opcodeName = OPCODE_NAMES[opcode];

    if (!opcodeName) {
        return "UNKNOWN";
    }

    const operand = REGISTER_OPERAND_OPCODES.has(opcodeName)
        ? `R${r2Imm}`
        : formatImmediate(opcodeName, r2Imm);

    return `${opcodeName} R${r1} ${operand}`;
}

function formatImmediate(opcodeName, value) {
    if (SIGNED_IMMEDIATE_OPCODES.has(opcodeName)) {
        return signExtend6(value);
    }

    return value;
}
