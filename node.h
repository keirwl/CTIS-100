#ifndef ctis_node_h
#define ctis_node_h

#include "instruction.h"

#include <stdbool.h>
#include <stdlib.h>

#define NODE_TEXT_WIDTH 18
#define NODE_TEXT_LINES 15
#define NODE_PROGRAM_LIMIT 64

#define REGISTER_MIN -999
#define REGISTER_MAX 999

typedef struct {
    Port *i_left, *i_right, *i_up, *i_down,
        *o_left, *o_right, *o_up, *o_down;
    Instr *program_start;
    Instr *pc;
    Label *labels;
    short acc;
    short bak;
} Node;

void init_node(Node *node);
void free_node(Node *node);

bool write_byte(Node *node, uint8_t byte);

#endif
