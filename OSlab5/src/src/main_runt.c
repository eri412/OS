#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <dlfcn.h>

#define MAX_INPUT_LEN 256

int main() {
    int cur_impl = 1;
    void *pi_1_h = dlopen("./bin/libpi_1.so", RTLD_NOW);
    void *pi_2_h = dlopen("./bin/libpi_2.so", RTLD_NOW);
    void *square_1_h = dlopen("./bin/libsquare_1.so", RTLD_NOW);
    void *square_2_h = dlopen("./bin/libsquare_2.so", RTLD_NOW);
    if (pi_1_h == NULL || pi_2_h == NULL ||
        square_1_h == NULL || square_2_h == NULL) 
    {
        perror("error");
        exit(1);
    }
    float (*pi)(int) = dlsym(pi_1_h, "pi");
    float (*square)(float, float) = dlsym(square_1_h, "square");
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
        if (strcmp(argv[0], "0") == 0) {
            if (argc != 1) {
                fprintf(stderr, "bad arguments\n");
                continue;
            }
            if (cur_impl == 1) {
                pi = dlsym(pi_2_h, "pi");
                square = dlsym(square_2_h, "square");
                cur_impl = 2;
            }
            else {
                pi = dlsym(pi_1_h, "pi");
                square = dlsym(square_1_h, "square");
                cur_impl = 1;
            }
            printf("implementation changed to %d\n", cur_impl);
        }
        else if (strcmp(argv[0], "1") == 0) {
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
    dlclose(pi_1_h);
    dlclose(pi_2_h);
    dlclose(square_1_h);
    dlclose(square_2_h);

    return 0;
}