#include <stdio.h>
#include <stdint.h>

#include "ccc.h"

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
