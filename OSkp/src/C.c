#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <sys/prctl.h>

#define BUFF_SIZE 256

int read_A;
int write_B;
int write_A;

void parent_death(int sig) {
    exit(EXIT_FAILURE);
}

void parent_end(int sig) {
    close(read_A);
    close(write_B);
    close(write_A);
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
    write_B = atoi(argv[3]);
    write_A = atoi(argv[4]);

    char buff[BUFF_SIZE + 1];
    while (1) {
        read(read_A, buff, BUFF_SIZE + 1);
        printf("C got string: %s", buff);
        size_t input_len = strlen(buff) - 1;
        write(write_B, &input_len, sizeof(size_t));
        int temp = 1;
        write(write_A, &temp, sizeof(int));  
    }

    return 0;
}
