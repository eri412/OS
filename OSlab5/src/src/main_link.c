#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "pi.h"
#include "square.h"

#define MAX_INPUT_LEN 256

int main() {
    char input[MAX_INPUT_LEN + 1];
    while (fgets(input, MAX_INPUT_LEN + 1, stdin) != NULL) {
        char dup[MAX_INPUT_LEN + 2];
        strcpy(dup + 1, input);
        dup[0] = '\0';
        size_t len = strlen(input);
        int argc = 0;
        for (size_t i = 1; i < len; ++i) {
            if (!isspace(dup[i]) && isspace(dup[i+1])) {
                ++argc;
            }
        }
        char *argv[argc];
        int cur = 0;
        for (size_t i = 1; i < len + 1; ++i) {
            if (isspace(dup[i])) {
                dup[i] = '\0';
            }
            else if (dup[i-1] == '\0') {
                argv[cur] = &dup[i];
                ++cur;
            }
        }
        if (argc == 0) {
            continue;
        }
        if (strcmp(argv[0], "1") == 0) {
            if (argc != 2) {
                fprintf(stderr, "bad arguments\n");
                continue;
            }
            int n = atoi(argv[1]);
            printf("pi = %f\n", pi(n));
        }
        else if (strcmp(argv[0], "2") == 0) {
            if (argc != 3) {
                fprintf(stderr, "bad arguments\n");
                continue;
            }
            float a = atof(argv[1]);
            float b = atof(argv[2]);
            printf("square = %f\n", square(a, b));
        }
        else {
            fprintf(stderr, "no such command\n");
        }
    }

    return 0;
}