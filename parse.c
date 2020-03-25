#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

#include "ccc.h"

Token *token;   // current token

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

Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);

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
