#include <stdio.h>
#include <string.h>

#include "assembler.h"

#define VALUE(t) ((Value){.type = (t)})
#define LITERAL_VALUE(n) ((Value){.type = VAL_LITERAL, .num = (n)})
#define PORT_VALUE(p) ((Value){.type = VAL_PORT, .port = (p)})

#define SET_NEXT_INSTR(i) if (node->program_start == NULL) { \
        node->program_start = (Instr*)(i); \
        node->pc = node->program_start; \
    } else { \
        node->pc->next = (Instr*)(i); \
        node->pc = node->pc->next; \
    }

bool assembler_error = false; // assemblerror?

// To avoid having to pass these to every function:
Token *token;
Node *node;

static void assemble_error(const char *message)
{
    fprintf(stderr, "%s\n", message);
    assembler_error = true;
}

static bool is_value(TokenType token_type) {
    return 
        token_type == TOK_LITERAL
        || token_type == TOK_NIL
        || token_type == TOK_ACC
        || token_type == TOK_UP
        || token_type == TOK_DOWN
        || token_type == TOK_LEFT
        || token_type == TOK_RIGHT
        || token_type == TOK_ANY
        || token_type == TOK_LAST;
}

static Value make_src_value(Token *token)
{
    switch (token->type) {
        case TOK_LITERAL:
            return LITERAL_VALUE(token->num);
            break;
        case TOK_NIL:
            return LITERAL_VALUE(0);
            break;
        case TOK_ACC:
            return VALUE(VAL_ACC);
            break;
        case TOK_ANY:
            return VALUE(VAL_ANY);
            break;
        case TOK_LAST:
            return VALUE(VAL_LAST);
            break;
        case TOK_UP:
            return PORT_VALUE(node->i_up);
            break;
        case TOK_DOWN:
            return PORT_VALUE(node->i_down);
            break;
        case TOK_LEFT:
            return PORT_VALUE(node->i_left);
            break;
        case TOK_RIGHT:
            return PORT_VALUE(node->i_right);
            break;
        default:
            assemble_error("Impossible non-value source token reached.");
            return LITERAL_VALUE(0);
            break;
    }
}

static Value make_dst_value(Token *token)
{
    switch (token->type) {
        case TOK_LITERAL:
            return LITERAL_VALUE(token->num);
            break;
        case TOK_NIL:
            return LITERAL_VALUE(0);
            break;
        case TOK_ACC:
            return VALUE(VAL_ACC);
            break;
        case TOK_ANY:
            return VALUE(VAL_ANY);
            break;
        case TOK_LAST:
            return VALUE(VAL_LAST);
            break;
        case TOK_UP:
            return PORT_VALUE(node->o_up);
            break;
        case TOK_DOWN:
            return PORT_VALUE(node->o_down);
            break;
        case TOK_LEFT:
            return PORT_VALUE(node->o_left);
            break;
        case TOK_RIGHT:
            return PORT_VALUE(node->o_right);
            break;
        default:
            assemble_error("Impossible non-value destination token reached.");
            return LITERAL_VALUE(0);
            break;
    }
}

static int add_mov()
{
    InstrMov *instr = malloc(sizeof (InstrMov));
    if (!instr) {
        assemble_error("Memory error in add_mov()");
        return -1;
    }

    Token *src_token = token + 1;
    Token *dst_token = token + 2;
    if (!is_value(src_token->type)) {
        free(instr);
        assemble_error("Invalid value for MOV source.");
        return -1;
    }
    if (!is_value(dst_token->type)) {
        free(instr);
        assemble_error("Invalid value for MOV destination.");
        return -1;
    }

    Value src = make_src_value(src_token);
    Value dst = make_dst_value(dst_token);

    instr->type = OP_MOV;
    instr->src = src;
    instr->dst = dst;
    instr->next = NULL;
    SET_NEXT_INSTR(instr);
    return 3;
}

static int add_nullary()
{
    OpCode type;
    if (token->type == TOK_SWP) {
        type = OP_SWP;
    } else if (token->type == TOK_SAV) {
        type = OP_SAV;
    } else if (token->type == TOK_NEG) {
        type = OP_NEG;
    } else {
        assemble_error("Trying to make nullary instruction that is not SWP, SAV or NEG.");
        return -1;
    }

    Instr *instr = malloc(sizeof (Instr));
    if (!instr) {
        assemble_error("Memory error in add_nullary()");
        return -1;
    }
    instr->type = type;
    instr->next = NULL;
    SET_NEXT_INSTR(instr);
    return 1;
}

static InstrUnary *make_unary_instr(TokenType token_type, Value val)
{
    OpCode type;
    if (token_type == TOK_ADD) {
        type = OP_ADD;
    } else if (token_type == TOK_SUB) {
        type = OP_SUB;
    } else if (token->type == TOK_JRO) {
        type = OP_JRO;
    } else {
        assemble_error("Trying to make unary instruction that is not ADD, SUB or JRO.");
        return NULL;
    }

    InstrUnary *instr = malloc(sizeof (InstrUnary));
    if (!instr) return NULL;
    instr->type = type;
    instr->val = val;
    instr->next = NULL;
    return instr;
}

static int add_unary()
{
    InstrUnary *instr;
    int retval;

    switch (token->type) {
        case TOK_JRO:
        case TOK_ADD:
        case TOK_SUB: {
            Token *next = (token + 1);
            if (!is_value(next->type)) {
                assemble_error("Invalid value for ADD, SUB or JRO instruction.");
                return -1;
            } else {
                Value val = make_src_value(next);
                instr = make_unary_instr(token->type, val);
            }
            retval = 2;
            break;
        }

        case TOK_NOP:
            instr = make_unary_instr(TOK_ADD, LITERAL_VALUE(0));
            retval = 1;
            break;

        default:
            assemble_error("Reached impossible case in add_unary()");
            return -1;
            break;
    }

    if (instr == NULL) {
        assemble_error("Memory error in add_unary()");
        return -1;
    }

    SET_NEXT_INSTR(instr);
    return retval;
}

static int add_jump()
{
    OpCode type;
    if (token->type == TOK_JMP) {
        type = OP_JMP;
    } else if (token->type == TOK_JEZ) {
        type = OP_JEZ;
    } else if (token->type == TOK_JNZ) {
        type = OP_JNZ;
    } else if (token->type == TOK_JGZ) {
        type = OP_JGZ;
    } else if (token->type == TOK_JLZ) {
        type = OP_JLZ;
    } else {
        assemble_error("Trying to make jump instruction with non-jump token.");
        return -1;
    }

    Token *next = (token + 1);
    if (next->type != TOK_LABEL) {
        assemble_error("Non-label operator passed to jump instruction.");
        return -1;
    }

    size_t name_length = strlen(next->text) + 1;
    char *name = malloc(name_length);
    if (!name) {
        assemble_error("Memory error in add_jump()");
        return -1;
    }
    InstrJump *instr = malloc(sizeof (InstrJump));
    if (!instr) {
        free(name);
        assemble_error("Memory error in add_jump()");
        return -1;
    }

    instr->name = name;
    strncat(instr->name, next->text, name_length);
    instr->name[name_length] = '\0';
    instr->next = NULL;
    SET_NEXT_INSTR(instr);
    return 2;
}

static int add_label()
{
    // TODO: Error if adding label with already-existing name
    size_t name_length = strlen(token->text) + 1;
    char *name = malloc(name_length);
    if (!name) {
        assemble_error("Memory error in add_label()");
        return -1;
    }
    Label *label = malloc(sizeof (label));
    if (!label) {
        free(name);
        assemble_error("Memory error in add_label()");
        return -1;
    }

    label->name = name;
    strncat(label->name, token->text, name_length);
    label->name[name_length] = '\0';
    // The label should point to the next instruction, but that
    // doesn't exist yet, so point to pt and go to next later
    label->to = node->pc;
    label->next = NULL;

    Label *curr_label = node->labels;
    if (curr_label == NULL) {
        node->labels = label;
    } else {
        while (curr_label->next != NULL) {
            curr_label++;
        }
        curr_label->next = label;
    }
    return 1;
}

static bool set_jumps()
{
    for (Instr *curr = node->program_start; curr->next != NULL; curr = curr->next) {
        if (is_jump(curr->type)) {
            InstrJump *instr = (InstrJump*)curr;
            bool found = false;
            for (Label *label = node->labels; label != NULL; label = label->next) {
                if (strlen(instr->name) == strlen(label->name)
                        && !strcmp(instr->name, label->name)) {
                    found = true;
                    instr->to = label->to->next == NULL ? node->program_start : label->to->next;
                }
            }
            if (!found) {
                assemble_error("Jump instruction to undefined label");
                return false;
            }
        }
    }
    return true;
}

bool assemble(Node *in_node, TokenList *token_list)
{
    int step;
    node = in_node;
    token = token_list->tokens;
    while (token->type != TOK_END) {
        switch (token->type) {
            case TOK_MOV:
                step = add_mov();
                break;

            case TOK_SWP:
            case TOK_SAV:
            case TOK_NEG:
                step = add_nullary();
                break;

            case TOK_NOP:
            case TOK_ADD:
            case TOK_SUB:
            case TOK_JRO:
                step = add_unary();
                break;

            case TOK_JMP:
            case TOK_JEZ:
            case TOK_JNZ:
            case TOK_JGZ:
            case TOK_JLZ:
                step = add_jump();
                break;

            case TOK_LABEL:
                step = add_label();
                break;

            case TOK_ACC:
                assemble_error("Cannot start instruction with ACC.");
                break;

            case TOK_UP:
            case TOK_DOWN:
            case TOK_LEFT:
            case TOK_RIGHT:
            case TOK_ANY:
            case TOK_LAST:
                assemble_error("Cannot start instruction with port.");
                break;

            case TOK_NIL:
            case TOK_LITERAL:
                assemble_error("Cannot start instruction with literal or NIL.");
                break;

            case TOK_END: break;
        }

        if (assembler_error || step == -1) {
            return false;
        } else {
            token += step;
        }
    }

    set_jumps();

    return true;
}
