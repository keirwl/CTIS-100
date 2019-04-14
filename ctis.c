#include <stdio.h>
#include <stdlib.h>

#include "assembler.h"
#include "debugging.h"
#include "node.h"
#include "scanner.h"

char test_source[] =
    "START:\n"
    "MOV LEFT, ACC\n"
    "MOV ACC, RIGHT\n"
    "JGZ ONE\n"
    "MOV 0, DOWN\n"
    "JMP START\n"
    "ONE: MOV 1, DOWN";

int main()
{
    Node node;
    init_node(&node);
    TokenList tokens;

    bool scan_success = scan(&tokens, test_source);

    if (scan_success) {
        printf("------------------\n");
        for (int i = 0; i < tokens.count; i++) print_token(tokens.tokens[i]);
    } else {
        return EXIT_FAILURE;
    }

    bool assemble_success = assemble(&node, &tokens);

    if (assemble_success) {
        Instr *instr = node.program_start;
        while (instr != NULL) {
            print_instr(instr);
            instr = instr->next;
        }
    } else {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < tokens.count; i++) {
        Token token = tokens.tokens[i];
        if (token.type == TOK_LABEL && token.text) free(token.text);
    }
    free_node(&node);
}
