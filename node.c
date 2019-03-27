#include "node.h"

void init_node(Node *node)
{
    node->pc = node->program;
    node->prog_length = 0;
    node->acc = 0;
    node->bak = 0;
}

bool write_byte(Node *node, uint8_t byte)
{
    if (node->prog_length >= NODE_PROGRAM_LIMIT) return false;
    
    node->program[node->prog_length++] = byte;
    return true;
}
