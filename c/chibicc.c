#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Kind of token
typedef enum {
    TK_RESERVED, // symbol
    TK_NUM,      // number
    TK_EOF,      // end of input
} TokenKind;

typedef struct Token Token;

// Token
struct Token {
    TokenKind kind;  // type of token
    Token     *next; // next token
    int       val;   // if `kind` is num, val is the number
    char      *str;  // token string
};

// Current token
Token *token;

// Variable number of arguments
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap); // formatted write using variable arguments list
    fprintf(stderr, "\n");
    exit(1);
}

// Input program
char *user_input;

// Report error location
void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// If token is the expected symbol, move to the next token
// and return true
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// If token is the expected symbol, move to the next token.
// Otherwise, report error
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "token is not '%c'.", op);
    token = token->next;
}

// If token is the number, move to the next token.
// Otherwise, report error
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "token is not a number.");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// Connect new token with current token
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = (Token*)calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str  = str;

    cur->next = tok;

    return tok;
}


Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // Skip space
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "Can not tokenize!");
    }

    new_token(TK_EOF, cur, p);

    // head is not used
    return head.next;
}


int main(int argc, char** argv) {
    if (argc != 2) {
        error("Invalid number of arguments.");
        return 1;
    }

    user_input = argv[1];

    // tokenize give argument
    token = tokenize(user_input);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // First token must be number
    printf("    mov rax, %d\n", expect_number());

    // Generate assembly from the stream of token
    while (!at_eof()) {

        // Operator is either '+' or '-'
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }

    printf("    ret\n");
    return 0;
}

