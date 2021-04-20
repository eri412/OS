#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>

#define BUFF_SIZE 256

#define READ 0
#define WRITE 1
#define MAX_ARG_LEN 8


int main() {
    int fd_A2C[2];
    int fd_A2B[2];
    int fd_C2A[2];
    int fd_C2B[2];

    if (pipe(fd_A2C) || pipe(fd_A2B) || pipe(fd_C2A) || pipe(fd_C2B)) {
        perror("Error");
        exit(EXIT_FAILURE);
    }

    pid_t parent_pid = getpid();

    pid_t proc_C_pid = fork();
    if (proc_C_pid == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }
    else if (proc_C_pid == 0) {
        close(fd_A2B[READ]);
        close(fd_A2B[WRITE]);
        close(fd_A2C[WRITE]);
        close(fd_C2B[READ]);
        close(fd_C2A[READ]);

        char ppid[MAX_ARG_LEN + 1];
        snprintf(ppid, MAX_ARG_LEN + 1, "%d", parent_pid);

        char read_A[MAX_ARG_LEN + 1];
        snprintf(read_A, MAX_ARG_LEN + 1, "%d", fd_A2C[READ]);

        char write_B[MAX_ARG_LEN + 1];
        snprintf(write_B, MAX_ARG_LEN + 1, "%d", fd_C2B[WRITE]);

        char write_A[MAX_ARG_LEN + 1];
        snprintf(write_A, MAX_ARG_LEN + 1, "%d", fd_C2A[WRITE]);

        execl("./C", "./C", ppid, read_A, write_B, write_A, (char *) NULL);
        kill(parent_pid, SIGABRT);
    }

    pid_t proc_B_pid = fork();
    if (proc_B_pid == -1) {
        perror("Error");
        kill(proc_C_pid, SIGABRT);
        exit(EXIT_FAILURE);
    }
    else if (proc_B_pid == 0) {
        close(fd_A2C[READ]);
        close(fd_A2C[WRITE]);
        close(fd_C2A[READ]);
        close(fd_C2A[WRITE]);
        close(fd_A2B[WRITE]);
        close(fd_C2B[WRITE]);

        char ppid[MAX_ARG_LEN + 1];
        snprintf(ppid, MAX_ARG_LEN + 1, "%d", parent_pid);

        char read_A[MAX_ARG_LEN + 1];
        snprintf(read_A, MAX_ARG_LEN + 1, "%d", fd_A2B[READ]);

        char read_C[MAX_ARG_LEN + 1];
        snprintf(read_C, MAX_ARG_LEN + 1, "%d", fd_C2B[READ]);

        execl("./B", "./B", ppid, read_A, read_C, (char *) NULL);
        kill(parent_pid, SIGABRT);
    }

    close(fd_C2B[READ]);
    close(fd_C2B[WRITE]);
    close(fd_C2A[WRITE]);
    close(fd_A2C[READ]);
    close(fd_A2B[READ]);
    
    char buff[BUFF_SIZE + 1];
    while (fgets(buff, BUFF_SIZE + 1, stdin) != NULL) {
        size_t input_len = strlen(buff) - 1;
        printf("A sent string: %s", buff);
        write(fd_A2B[WRITE], &input_len, sizeof(size_t));
        write(fd_A2C[WRITE], buff, BUFF_SIZE + 1);
        int temp;
        read(fd_C2A[READ], &temp, sizeof(int));
    }

    kill(proc_C_pid, SIGTERM);
    kill(proc_B_pid, SIGTERM);

    close(fd_A2C[WRITE]);
    close(fd_A2B[WRITE]);
    close(fd_C2A[READ]);

    return 0;
}
