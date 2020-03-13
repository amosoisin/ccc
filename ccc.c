#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
}NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;  // node type
    Node *lhs;      // Left Node
    Node *rhs;      // Right Node
    int val;        // use if kind is ND_NUM
};


Node *expr(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int32_t val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

typedef enum{
    TK_RESERVED,    // operator
    TK_NUM,         // integer token
    TK_EOF,         // end of file
} TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind; // token type
    Token *next;    // next token
    int32_t val;   // integer if kind is number
    char *str;      // string if kind is operator
};

Token *token;   // current token

// report error
char *user_input;
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int32_t pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
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
        error_at(token->str, "is not '%c'", op);
    }
    token = token->next;
}

// if token is expected number
// move next token and return number
// else report error
int32_t expect_number(){
    if (token->kind != TK_NUM) {
        error_at(token->str, "is not number");
    }
    int32_t val = token->val;
    token = token->next;
    return val;
}


// expr = mul ("+" mul | "-" mul)*
Node *expr(){
    Node *node = mul();

    while(1){
        if (consume('+')){
            node = new_node(ND_ADD, node, mul());
        }else if (consume('-')){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

// mul = primary ('*' primary | "/" primary)
Node *mul(){
    Node *node = unary();
    while(1){
        if (consume('*')){
            node = new_node(ND_MUL, node, unary());
        }else if (consume('/')){
            node = new_node(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

// unary = ("+" | "-")? primary
Node *unary(){
    if (consume('+')){
        return primary();
    }
    if (consume('-')){
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

// primary = num | "(" expr ")"
Node *primary(){
    if (consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }
    return new_node_num(expect_number());
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

        if (*p == '+' || *p == '-' 
                || *p == '*' || *p == '/'
                || *p == '(' || *p == ')'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "cannot tokenize");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}


void gen(Node *node){
    if (node->kind == ND_NUM){
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    if (node->kind == ND_ADD){
        printf("    add rax, rdi\n");
    }else if (node->kind == ND_SUB){
        printf("    sub rax, rdi\n");
    }else if (node->kind == ND_MUL){
        printf("    imul rax, rdi\n");
    }else if (node->kind == ND_DIV){
        printf("    cqo\n");
        printf("    idiv rdi\n");
    }
    printf("    push rax\n");
}

int main(int32_t argc, int8_t **argv){
    if (argc != 2){
        fprintf(stderr, "Invalid argument!!");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(user_input);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");

    return 0;
}
