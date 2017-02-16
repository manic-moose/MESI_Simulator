#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define LOAD_CMD  1
#define STORE_CMD 2
#define NOP_CMD   3

struct Instruction {
    unsigned int OPCODE;
    unsigned int ADDRESS;
};

#endif //INSTRUCTION_H

