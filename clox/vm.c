#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"


VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {
}

void push(Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}


static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_FROM(index) (vm.chunk->constants.values[index])
#define READ_24BIT() ( \
    vm.ip += 3, \
    (uint32_t)(vm.ip[-3] | (vm.ip[-2] << 8) | (vm.ip[-1] << 16)) \
)
    // Modifying the ninary result in place to avoid a pop and push
#define BINARY_OP(op) \
do { \
    double b = pop(); \
    *(vm.stackTop - 1) = *(vm.stackTop - 1) op b; \
} while (false)

for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf(" ");
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        printf("[ ");
        printValue(*slot);
        printf(" ]");
    }
    printf("\n");

    disassembleInstruction(vm.chunk,
        (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
    }
    case OP_CONSTANT_LONG: {
        uint32_t constantIdx = READ_24BIT();
        Value constant = READ_CONSTANT_FROM(constantIdx);
        push(constant);
        break;
    }
    case OP_ADD:      BINARY_OP(+); break;
    case OP_SUBTRACT: BINARY_OP(-); break;
    case OP_MULTIPLY: BINARY_OP(*); break;
    case OP_DIVIDE:   BINARY_OP(/); break;
    case OP_NEGATE:
        *(vm.stackTop - 1) = -*(vm.stackTop - 1);
        break;
    case OP_RETURN: {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
    }
    }
}

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_FROM
#undef READ_24BIT
#undef BINARY_OP
}

InterpretResult interpret(const char* source) {
  compile(source);
  return INTERPRET_OK;
}
