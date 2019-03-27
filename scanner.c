#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "node.h"
#include "scanner.h"

#define MAX_SOURCE_LENGTH (NODE_TEXT_WIDTH * NODE_TEXT_LINES + 1)

typedef struct {
    char source[NODE_TEXT_WIDTH * NODE_TEXT_LINES + 1];
    const char *start, *current;
    TokenList *token_list;
    int line, column;
    bool error;
} Scanner;

Scanner scanner;

const char *keywords[] = {
    "NIL", "ACC", "UP", "DOWN", "LEFT", "RIGHT",
    "ANY", "LAST", "NOP", "MOV", "SWP", "SAV",
    "ADD", "SUB", "NEG", "JMP", "JEZ", "JNZ",
    "JGZ", "JLZ", "JRO"
};

void scan_error(const char *message)
{
    fprintf(stderr, "Error at L%d C%d: %s\n", scanner.line, scanner.column, message);
    scanner.error = true;
}

static void add_token(TokenType type, char *text, short num)
{
    if (type == TOK_LABEL) {
        printf("%s\n", text);
        scanner.token_list->tokens[scanner.token_list->count++] = (Token){.type = TOK_LABEL, .text = text};
    } else if (type == TOK_LITERAL) {
        scanner.token_list->tokens[scanner.token_list->count++] = (Token){.type = TOK_LITERAL, .num = num};
    } else {
        scanner.token_list->tokens[scanner.token_list->count++] = (Token){.type = type};
    }
    if (scanner.token_list->count >= MAX_TOKENS) {
        scan_error("Maximum token limit reached");
    }
    scanner.start = scanner.current + 1;
}

static size_t token_length()
{
    // NB: current is the punctuation 1 after the interesting characters
    return scanner.current - scanner.start - 1;
}

static bool is_valid_char(const char c)
{
    return (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        ||  c == ',' || c == '\n'
        ||  c == ':' || c == ' ';
}

static bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

static int keyword()
{
    size_t keyword_count = sizeof keywords / sizeof (char*);
    for (int i = 0; i < keyword_count; i++) {
        // Inefficient. Might be best replaced with a trie-like switch statement
        if (!strncmp(scanner.start, keywords[i], token_length())) return i;
    }
    return -1;
}

static void label()
{
    if (keyword() != -1) {
        scan_error("Cannot use instruction or register name as label.");
        return;
    }
    size_t tok_length = token_length();
    char *text = malloc(tok_length + 1); // TODO: ensure this is freed
    if (text == NULL) {
        scan_error("Memory error.");
        return;
    }

    text[tok_length] = '\0';
    strncat(text, scanner.start, tok_length);

    add_token(TOK_LABEL, text, 0);
}

static void thing() // Great name
{
    int keyword_index = keyword();
    if (keyword_index != -1) {
        add_token(keyword_index, NULL, 0);
        return;
    }

    if (is_digit(*scanner.start)) {
        char *number_end;
        long number = strtol(scanner.start, &number_end, 10);
        if (*number_end != *scanner.current) {
            // A non-digit was found in the number before reaching current
            // punctuation, therefore the number is invalid
            scan_error("Invalid number literal");
            return;
        }
        if (number < REGISTER_MIN || number > REGISTER_MAX) {
            // TODO: check if real TIS-100 disallows large number literals
            scan_error("Magnitude of number too large");
            return;
        }
        if (number < 0) {
            add_token(TOK_LITERAL, NULL, 0 - (short)number);
        } else {
            add_token(TOK_LITERAL, NULL, (short)number);
        }
        return;
    }

    // If not a keyword or number literal, it must be a label
    label();
}

bool scan(TokenList *token_list, const char *source)
{
    scanner.token_list = token_list;
    scanner.token_list->count = 0;
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
    scanner.column = 1;
    char c;

    if (sizeof *source > MAX_SOURCE_LENGTH) {
        scan_error("Source code too long for node.");
        return false;
    }

    while ((c = *scanner.current) != '\0') {
        if (scanner.column > NODE_TEXT_WIDTH + 1) {
            scan_error("Source code line has too many characters.");
            return false;
        }

        if (!is_valid_char(c)) {
            scan_error("Invalid character.");
            return false;
        }

        switch (c) {
            case ':': label(); break;
            case ' ':
            case ',':
                thing(); break;
            case '\n':
                thing();
                scanner.line++;
                scanner.column = 1;
                if (scanner.line > NODE_TEXT_LINES) {
                    scan_error("Source code has too many lines.");
                }
                break;
            default: break;
        }
        if (scanner.error) return false;

        scanner.current++;
        scanner.column++;
    }

    add_token(TOK_END, NULL, 0);

    return true;
}
