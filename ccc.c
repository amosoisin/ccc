#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
    TK_RESERVED,    // operator
    TK_NUM,         // integer token
    TK_EOF,         // end of file
} TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind; // token type
    Token *next;    // next token
    uint32_t val;   // integer if kind is number
    char *str;      // string if kind is operator
};

Token *token;   // current token

// report error
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if token is expected
// move next token and return true
// else false
bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op) return false;
    token = token->next;
    return true;
}

// if token is expected operator
// move next token
// else report error
void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op){
        error("is not '%c'", op);
    }
    token = token->next;
}

// if token is expected number
// move next token and return number
// else report error
uint32_t expect_number(){
    if (token->kind != TK_NUM) error("is not number");
    uint32_t val = token->val;
    token = token->next;
    return val;
}


bool at_eof(){
    return token->kind == TK_EOF;
}


// create new token 
// connect to backward token
Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}


Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        if (isspace(*p)){
            p++;
            continue;
        }

        if (*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenize");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}


int main(uint32_t argc, uint8_t **argv){
    if (argc != 2){
        fprintf(stderr, "Invalid argument!!");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("    mov rax, %d\n", expect_number());
    
    while (!at_eof()){
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
