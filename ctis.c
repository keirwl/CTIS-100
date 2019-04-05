#include <stdio.h>
#include <stdlib.h>

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

int main()
{
    TokenList tokens;
    if (scan(&tokens, test_source)) {
        printf("------------------\n");
        for (int i = 0; i < tokens.count; i++) print_token(tokens.tokens[i]);
    }
    
    for (int i = 0; i < tokens.count; i++) {
        Token token = tokens.tokens[i];
        if (token.type == TOK_LABEL && token.text) free(token.text);
    }
}
