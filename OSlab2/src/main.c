#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/prctl.h>

#include <fcntl.h>
#include <sys/stat.h>

#define PATH_MAX 4096
#define INPUT_BUFFER 4096
#define MAX_PROCESSES 2
#define READ 0
#define WRITE 1
#define BOUNDARY 10

#define STDIN 0

size_t my_read(char *buff, size_t max_bytes, int fd) {
    char temp;
    size_t i;
    for (i = 0; i < max_bytes - 1; ++i) {
        if (read(fd, &temp, sizeof(char)) == 0 || temp == '\n') {
            break;
        }
        buff[i] = temp;
    }
    buff[i] = '\0';
    return i;
}

void parent_death(int sig) {
    exit(0);
}

void parentjob(int fd[MAX_PROCESSES][2]) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        close(fd[i][READ]);
    }
    char filename[PATH_MAX + 1];
    for (int i = 0; i < MAX_PROCESSES; i++) {
        //fgets(filename, PATH_MAX + 1, stdin);
        my_read(filename, PATH_MAX + 1, STDIN);
        write(fd[i][WRITE], filename, PATH_MAX + 1);
    }
    char input[INPUT_BUFFER + 1];
    //while (fgets(input, INPUT_BUFFER + 1, stdin) != NULL) {
    while (my_read(input, INPUT_BUFFER + 1, STDIN) != 0) {
        if (strlen(input) - 1 <= BOUNDARY) {
            write(fd[0][WRITE], input, INPUT_BUFFER + 1);
        }
        else {
            write(fd[1][WRITE], input, INPUT_BUFFER + 1);
        }
    }
}

void childjob(int fd[2], pid_t parent_pid) {
    prctl(PR_SET_PDEATHSIG, SIGTERM);
    signal(SIGTERM, parent_death);
    if (getppid() != parent_pid) {
        parent_death(SIGTERM);
    }
    close(fd[WRITE]);
    char filename[PATH_MAX + 1];
    read(fd[READ], filename, PATH_MAX + 1);
    //FILE *fp = fopen(filename, "w");
    int file_des = open(filename, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
    char input[INPUT_BUFFER + 1];
    while (1) {
        read(fd[READ], input, INPUT_BUFFER + 1);
        size_t input_len = strlen(input);
        for (int i = 0; i < input_len / 2; i++) {
            char temp = input[i];
            input[i] = input[input_len - 1 - i];
            input[input_len - 1 - i] = temp;
        }
        //fputs(input, fp);
        write(file_des, input, strlen(input));
        write(file_des, "\n", 1);
    }
}

int main() {
    int fd[MAX_PROCESSES][2];
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pipe(fd[i]);
    }
    pid_t parent_pid = getpid();
    pid_t temp_pid = 1;
    unsigned int id = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (temp_pid != 0) {
            id = i;
            temp_pid = fork();
            if (temp_pid == -1) {
                perror("fork error");
                exit(1);
            }
        }
        else {
            break;
        }
    }
    if (temp_pid == 0) {
        childjob(fd[id], parent_pid);
    }
    else {
        parentjob(fd);
    }
    
    return 0;
}