#ifndef ctis_scanner_h
#define ctis_scanner_h

#define MAX_TOKENS 64

typedef enum {
    // Registers
    TOK_NIL, TOK_ACC, TOK_UP,
    TOK_DOWN, TOK_LEFT, TOK_RIGHT,
    TOK_ANY, TOK_LAST,

    // Instructions
    TOK_NOP, TOK_MOV, TOK_SWP, 
    TOK_SAV, TOK_ADD, TOK_SUB,
    TOK_NEG, TOK_JMP, TOK_JEZ,
    TOK_JNZ, TOK_JGZ, TOK_JLZ,
    TOK_JRO,

    // Other
    TOK_LITERAL, TOK_LABEL, TOK_END
} TokenType;

typedef struct {
    union {
        char *text;
        short num;
    };
    TokenType type;
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int count;
} TokenList;

extern const char *keywords[];

bool scan(TokenList *token_list, const char *source);

#endif
