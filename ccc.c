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
    ND_EQL,
    ND_NEQL,
    ND_LOW,
    ND_LOW_EQL,
}NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;  // node type
    Node *lhs;      // Left Node
    Node *rhs;      // Right Node
    uint32_t val;   // use if kind is ND_NUM
};


Node *expr(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
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

Node *new_node_num(uint32_t val){
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
    uint32_t val;   // integer if kind is number
    char *str;      // string if kind is operator
    size_t len;     // operation length
};

Token *token;   // current token

// report error
char *user_input;
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    uint32_t pos = loc - user_input;
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
bool consume(char *op){
    if (token->kind != TK_RESERVED || 
            strlen(op) != token->len ||
            memcmp(token->str, op, token->len) != 0) {
        return false;
    }
    token = token->next;
    return true;
}

// if token is expected operator
// move next token
// else report error
void expect(char *op){
    if (token->kind != TK_RESERVED || 
            strlen(op) != token->len ||
            memcmp(token->str, op, token->len) != 0){
        error_at(token->str, "is not '%c'", op);
    }
    token = token->next;
}

// if token is expected number
// move next token and return number
// else report error
uint32_t expect_number(){
    if (token->kind != TK_NUM) {
        error_at(token->str, "is not number");
    }
    uint32_t val = token->val;
    token = token->next;
    return val;
}


// expr = equality
Node *expr(){
    return equality();
}


// equality = relational ("==", relational | "!=" relational)*
Node *equality(){
    Node *node = relational();

    while(1){
        if (consume("==")){
            node = new_node(ND_EQL, node, relational());
        }else if (consume("!=")){
            node = new_node(ND_NEQL, node, relational());
        }else{
            return node;
        }
    }
}


// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
    Node *node = add();

    while(1){
        if (consume("<")){
            node = new_node(ND_LOW, node, add());
        }else if(consume("<=")){
            node = new_node(ND_LOW_EQL, node, add());
        }else if(consume(">")){
            node = new_node(ND_LOW, add(), node);
        }else if(consume(">=")){
            node = new_node(ND_LOW_EQL, add(), node);
        }else{
            return node;
        }
    }
}

// add = mul ("+" mul | "-" mul)*
Node *add(){
    Node *node = mul();

    while(1){
        if (consume("+")){
            node = new_node(ND_ADD, node, mul());
        }else if(consume("-")){
            node = new_node(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

// mul = unary ('*' unary | "/" unary)*
Node *mul(){
    Node *node = unary();

    while(1){
        if (consume("*")){
            node = new_node(ND_MUL, node, unary());
        }else if (consume("/")){
            node = new_node(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

// unary = ("+" | "-")? primary
Node *unary(){
    if (consume("+")){
        return primary();
    }
    if (consume("-")){
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}

// primary = num | "(" expr ")"
Node *primary(){
    if (consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}

bool at_eof(){
    return token->kind == TK_EOF;
}


// create new token 
// connect to backward token
Token *new_token(TokenKind kind, Token *cur, char *str, size_t len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
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

        if (memcmp(p, "==", 2) == 0 ||
                memcmp(p, "!=", 2) == 0 ||
                memcmp(p, "<=", 2) == 0 ||
                memcmp(p, ">=", 2) == 0){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' 
                || *p == '*' || *p == '/'
                || *p == '(' || *p == ')'
                || *p == '<' || *p == '>'){
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "cannot tokenize");
    }
    new_token(TK_EOF, cur, p, 0);
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
    }else if (node->kind == ND_EQL){
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
    }else if (node->kind == ND_NEQL){
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
    }else if (node->kind == ND_LOW){
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
    }else if (node->kind == ND_LOW_EQL){
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
    }
    printf("    push rax\n");
}

int main(uint32_t argc, int8_t **argv){
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
