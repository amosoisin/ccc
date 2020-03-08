#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(uint32_t argc, uint8_t **argv){
    if (argc != 2){
        fprintf(stderr, "Invalid argument!!");
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", atoi(argv[1]));
    printf("    ret\n");

    return 0;
}
