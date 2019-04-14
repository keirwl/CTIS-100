#include <stdio.h>

#include "debugging.h"

const char *opcodes[] = {
    "OP_MOV", "OP_SWP", "OP_SAV",
    "OP_ADD", "OP_SUB", "OP_NEG", 
    "OP_JMP", "OP_JEZ", "OP_JNZ",
    "OP_JGZ", "OP_JLZ", "OP_JRO"
};

void print_token(Token token)
{
    if (token.type == TOK_LABEL) {
        printf("label: %s\n", token.text);
    } else if (token.type == TOK_LITERAL) {
        printf("literal: %d\n", token.num);
    } else if (token.type == TOK_END) {
        printf("------------------\n");
    } else {
        printf("%s\n", keywords[token.type]);
    }
}

static void print_value(Value value)
{
    switch (value.type) {
        case VAL_LITERAL:
            printf("%d", value.num);
            break;
        case VAL_ACC:
            printf("ACC");
            break;
        case VAL_ANY:
            printf("ANY");
            break;
        case VAL_LAST:
            printf("LAST");
            break;
        case VAL_PORT:
            printf("%p", value.port);
            break;
    }
}

void print_instr(Instr *instr)
{
    printf("%s: ", opcodes[instr->type]);
    if (instr->type == OP_MOV) {
        InstrMov *mov = (InstrMov*)instr;
        print_value(mov->src);
        printf(", ");
        print_value(mov->dst);
    } else if (instr->type == OP_ADD 
            || instr->type == OP_SUB
            || instr->type == OP_JRO) {
        InstrUnary *unary = (InstrUnary*)instr;
        print_value(unary->val);
    } else if (is_jump(instr->type)) {
        InstrJump *jump = (InstrJump*)instr;
        printf("%s", jump->name);
    }
    printf("\n");
}
