#include <stdio.h>
#include <stdint.h>

#include "ccc.h"

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
