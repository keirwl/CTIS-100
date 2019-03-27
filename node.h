#ifndef ctis_node_h
#define ctis_node_h

#include <stdbool.h>
#include <stdlib.h>

#define NODE_TEXT_WIDTH 18
#define NODE_TEXT_LINES 15
#define NODE_PROGRAM_LIMIT 64

#define REGISTER_MIN -999
#define REGISTER_MAX 999

typedef struct {
    short value;
    bool blocked;
} Port;

typedef struct {
    Port *i_left, *i_right, *i_up, *i_down,
        *o_left, *o_right, *o_up, *o_down;
    uint8_t program[NODE_PROGRAM_LIMIT];
    uint8_t *pc;
    size_t prog_length;
    short acc;
    short bak;
} Node;

void free_node(Node *node);

bool write_byte(Node *node, uint8_t byte);

#endif
