#include <stdio.h>

#include "chunk.h"
#include "debug.h"
#include "object.h"
#include "value.h"

void disassembleChunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

static int simpleInstruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int byteInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

static int bit24Instruction(const char *name, Chunk *chunk, int offset) {
    uint8_t slot1 = chunk->code[offset + 1];
    uint8_t slot2 = chunk->code[offset + 2];
    uint8_t slot3 = chunk->code[offset + 3];

    uint32_t slot = slot1 | (slot2 << 8) | (slot3 << 16);

    printf("%-16s %4d\n", name, slot);
    return offset + 4;
}

static int jumpInstruction(const char *name, int sign, Chunk *chunk,
                           int offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jump);
    return offset + 3;
}

static int constantInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant =
        chunk->code[offset + 1]; // Gets the constant index in the constants
                                 // array right after the OPcode
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");

    return offset + 2;
}

static int constantInstructionLong(const char *name, Chunk *chunk, int offset) {
    uint8_t constant1 = chunk->code[offset + 1];
    uint8_t constant2 = chunk->code[offset + 2];
    uint8_t constant3 = chunk->code[offset + 3];

    uint32_t constant = constant1 | (constant2 << 8) | (constant3 << 16);

    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");

    return offset + 4;
}

static int invokeInstruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    uint8_t argCount = chunk->code[offset + 2];
    printf("%-16s (%d args) %4d '", name, argCount, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 3;
}

static int invokeInstructionLong(const char *name, Chunk *chunk, int offset) {
    uint8_t constant1 = chunk->code[offset + 1];
    uint8_t constant2 = chunk->code[offset + 2];
    uint8_t constant3 = chunk->code[offset + 3];

    uint32_t constant = constant1 | (constant2 << 8) | (constant3 << 16);
    uint8_t argCount = chunk->code[offset + 4];
    printf("%-16s (%d args) %4d '", name, argCount, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 5;
}

int disassembleInstruction(Chunk *chunk, int offset) {
    printf("%04d ", offset);

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf(" | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
        return constantInstructionLong("OP_CONSTANT_LONG", chunk, offset);
    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);
    case OP_POP:
        return simpleInstruction("OP_POP", offset);
    case OP_GET_LOCAL:
        return byteInstruction("OP_GET_LOCAL", chunk, offset);
    case OP_GET_LOCAL_LONG:
        return bit24Instruction("OP_GET_LOCAL_LONG", chunk, offset);
    case OP_SET_LOCAL:
        return byteInstruction("OP_SET_LOCAL", chunk, offset);
    case OP_SET_LOCAL_LONG:
        return bit24Instruction("OP_SET_LOCAL_LONG", chunk, offset);
    case OP_GET_GLOBAL:
        return constantInstruction("OP_GET_GLOBAL", chunk, offset);
    case OP_GET_GLOBAL_LONG:
        return constantInstructionLong("OP_GET_GLOBAL_LONG", chunk, offset);
    case OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
    case OP_DEFINE_GLOBAL_LONG:
        return constantInstructionLong("OP_DEFINE_GLOBAL_LONG", chunk, offset);
    case OP_SET_GLOBAL:
        return constantInstruction("OP_SET_GLOBAL", chunk, offset);
    case OP_SET_GLOBAL_LONG:
        return constantInstructionLong("OP_SET_GLOBAL_LONG", chunk, offset);
    case OP_GET_UPVALUE:
        return byteInstruction("OP_GET_UPVALUE", chunk, offset);
    case OP_GET_UPVALUE_LONG:
        return bit24Instruction("OP_GET_UPVALUE_LONG", chunk, offset);
    case OP_SET_UPVALUE:
        return byteInstruction("OP_SET_UPVALUE", chunk, offset);
    case OP_SET_UPVALUE_LONG:
        return bit24Instruction("OP_SET_UPVALUE_LONG", chunk, offset);
    case OP_GET_PROPERTY:
        return constantInstruction("OP_GET_PROPERTY", chunk, offset);
    case OP_GET_PROPERTY_LONG:
        return constantInstructionLong("OP_GET_PROPERTY_LONG", chunk, offset);
    case OP_SET_PROPERTY:
        return constantInstruction("OP_SET_PROPERTY", chunk, offset);
    case OP_SET_PROPERTY_LONG:
        return constantInstructionLong("OP_SET_PROPERTY_LONG", chunk, offset);
    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);
    case OP_JUMP:
        return jumpInstruction("OP_JUMP", 1, chunk, offset);
    case OP_JUMP_IF_FALSE:
        return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
    case OP_LOOP:
        return jumpInstruction("OP_LOOP", -1, chunk, offset);
    case OP_CALL:
        return byteInstruction("OP_CALL", chunk, offset);
    case OP_INVOKE:
        return invokeInstruction("OP_INVOKE", chunk, offset);
    case OP_INVOKE_LONG:
        return invokeInstructionLong("OP_INVOKE_LONG", chunk, offset);
    case OP_CLOSE_UPVALUE:
        return simpleInstruction("OP_CLOSE_UPVALUE", offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    case OP_CLASS:
        return constantInstruction("OP_CLASS", chunk, offset);
    case OP_CLASS_LONG:
        return constantInstructionLong("OP_CLASS_LONG", chunk, offset);
    case OP_METHOD:
        return constantInstruction("OP_METHOD", chunk, offset);
    case OP_METHOD_LONG:
        return constantInstructionLong("OP_METHOD_LONG", chunk, offset);
    case OP_CLOSURE: {
        offset++;
        uint8_t constant = chunk->code[offset++];
        printf("%-16s %4d ", "OP_CLOSURE", constant);
        printValue(chunk->constants.values[constant]);
        printf("\n");

        ObjFunction *function = AS_FUNCTION(chunk->constants.values[constant]);
        for (int j = 0; j < function->upvalueCount; j++) {
            int isLocal = chunk->code[offset++];
            int index = chunk->code[offset++];
            printf("%04d | %s %d\n", offset - 2, isLocal ? "local" : "upvalue",
                   index);
        }

        return offset;
    }
    case OP_CLOSURE_LONG: {
        offset++;
        uint8_t constant1 = chunk->code[offset];
        uint8_t constant2 = chunk->code[offset + 1];
        uint8_t constant3 = chunk->code[offset + 2];

        uint32_t constant = constant1 | (constant2 << 8) | (constant3 << 16);

        offset += 3;
        printf("%-16s %4d ", "OP_CLOSURE_LONG", constant);
        printValue(chunk->constants.values[constant]);
        printf("\n");

        ObjFunction *function = AS_FUNCTION(chunk->constants.values[constant]);
        for (int j = 0; j < function->upvalueCount; j++) {
            int isLocal = chunk->code[offset++];

            int index = chunk->code[offset] | (chunk->code[offset + 1] << 8) |
                        (chunk->code[offset + 2] << 16);
            offset += 3;

            printf("%04d | %s %d\n", offset - 4, isLocal ? "local" : "upvalue",
                   index);
        }

        return offset;
    }
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}
