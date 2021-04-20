#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <sys/prctl.h>

#define BUFF_SIZE 256

int read_A;
int read_C;

void parent_death(int sig) {
    exit(EXIT_FAILURE);
}

void parent_end(int sig) {
    close(read_A);
    close(read_C);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    pid_t parent_pid = atoi(argv[1]);
    prctl(PR_SET_PDEATHSIG, SIGABRT);
    signal(SIGABRT, parent_death);
    if (getppid() != parent_pid) {
        parent_death(SIGABRT);
    }
    signal(SIGTERM, parent_end);

    read_A = atoi(argv[2]);
    read_C = atoi(argv[3]);

    size_t from_A;
    size_t from_C;
    while (1) {
        read(read_A, &from_A, sizeof(size_t));
        printf("B got length %zu from A\n", from_A);
        read(read_C, &from_C, sizeof(size_t));
        printf("B got length %zu from C\n", from_C);
    }

    return 0;
}

