#include "node.h"

void init_node(Node *node)
{
    node->i_left = NULL;
    node->i_right = NULL;
    node->i_up = NULL;
    node->i_down = NULL;
    node->o_left = NULL;
    node->o_right = NULL;
    node->o_up = NULL;
    node->o_down = NULL;
    node->program_start = NULL;
    node->pc = NULL;
    node->labels = NULL;
    node->acc = 0;
    node->bak = 0;
}


void free_node(Node *node)
{
    Instr *next_instr = node->program_start;
    while (next_instr != NULL) {
        Instr *curr = next_instr;
        next_instr = curr->next;
        if (is_jump(curr->type)) {
            free(((InstrJump*)curr)->name);
        }
        free(curr);
    }
    Label *next_label = node->labels;
    while (next_label != NULL) {
        Label *curr = next_label;
        next_label = curr->next;
        free(curr->name);
        free(curr);
    }
    init_node(node);
}
