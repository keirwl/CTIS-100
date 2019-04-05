#ifndef ctis_instruction_h
#define ctis_instruction_h

#include <stdbool.h>

typedef enum {
    OP_MOV, OP_SWP, OP_SAV,
    OP_ADD, OP_SUB, OP_NEG, 
    OP_JMP, OP_JEZ, OP_JNZ,
    OP_JGZ, OP_JLZ, OP_JRO
} OpCode;

typedef struct {
    short value;
    bool blocked;
} Port;

typedef struct {
    enum {VAL_ACC, VAL_PORT, 
        VAL_ANY, VAL_LAST, 
        VAL_LITERAL} type;
    union {
        Port *port;
        short num;
    };
} Value;

typedef struct Instr {
    OpCode type;
    struct Instr *next;
} Instr;

typedef struct {
    OpCode type;
    struct Instr *next;
    Value src;
    Value dst;
} InstrMov;

typedef struct {
    OpCode type;
    struct Instr *next;
    Value val;
} InstrUnary;

typedef struct {
    OpCode type;
    struct Instr *next;
    struct Instr *to;
    char *name;
} InstrJump;

typedef struct Label {
    char *name;
    struct Label *next;
    struct Instr *to;
} Label;

bool is_jump(OpCode type)
{
    return type == OP_JMP
        || type == OP_JEZ
        || type == OP_JNZ
        || type == OP_JGZ
        || type == OP_JLZ;
}

#endif
