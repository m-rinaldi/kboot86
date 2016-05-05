#pragma once

#include <stdbool.h>
#include <token.h>

int lexer_init(const char *);
void lexer_process(void);
bool lexer_has_token(void);
token_t lexer_get_token(void);

