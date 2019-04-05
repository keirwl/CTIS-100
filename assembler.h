#ifndef ctis_assembler_h
#define ctis_assembler_h

#include <stdbool.h>

#include "instruction.h"
#include "node.h"
#include "scanner.h"

bool assemble(Node *node, TokenList *tokens);

#endif
