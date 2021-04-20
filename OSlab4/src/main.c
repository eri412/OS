#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#define BUFFSIZE 256
#define PROC_NUM 2

sig_atomic_t sigs_got = 0;

void wait_sig(int sig) {
    ++sigs_got;
}

void parent_death(int sig) {
    exit(1);
}

void parent_job(char shared_mem[BUFFSIZE + 1], pid_t child_pid[PROC_NUM]) {
    sigset_t empty_sigset;
    sigemptyset(&empty_sigset);
    while (sigs_got != 2) {
    }
    sigset_t blocking_sigset;
    sigemptyset(&blocking_sigset);
    sigaddset(&blocking_sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &blocking_sigset, NULL);
    while (fgets(shared_mem, BUFFSIZE + 1, stdin) != NULL) {
        if (strlen(shared_mem) > 10) {
            kill(child_pid[1], SIGUSR1);
        }
        else {
            kill(child_pid[0], SIGUSR1);
        }
        sigsuspend(&empty_sigset);
    }
}

void child_job(char *shared_mem, pid_t parent_pid, char *filename) {
    prctl(PR_SET_PDEATHSIG, SIGTERM);
    signal(SIGTERM, parent_death);
    if (getppid() != parent_pid) {
        parent_death(SIGTERM);
    }
    sigset_t empty_sigset;
    sigemptyset(&empty_sigset);
    sigset_t blocking_sigset;
    sigemptyset(&blocking_sigset);
    sigaddset(&blocking_sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &blocking_sigset, NULL);
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error");
        kill(parent_pid, SIGINT);
    }
    kill(parent_pid, SIGUSR1);
    while (1) {
        sigsuspend(&empty_sigset);
        for (int i = strlen(shared_mem) - 2; i >= 0; --i) {
            fputc(shared_mem[i], fp);
        }
        fputc('\n', fp);
        kill(parent_pid, SIGUSR1);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error: wrong number of arguments, should be <prog_name> <filename1> <filename2>\n");
        exit(1);
    }
    char *shared_mem = mmap(NULL, BUFFSIZE + 1, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
    if (shared_mem == MAP_FAILED) {
        perror("Error");
        exit(1);
    }  
    signal(SIGUSR1, wait_sig);
    char *filename;
    pid_t child_pid[PROC_NUM];
    pid_t parent_pid = getpid();
    pid_t temp_pid = 1;
    for (int i = 0; i < PROC_NUM; ++i) {
        if (temp_pid != 0) {
            temp_pid = fork();
            filename = argv[i + 1];
            child_pid[i] = temp_pid;
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
        child_job(shared_mem, parent_pid, filename);
    }
    else {
        parent_job(shared_mem, child_pid);
    }

    return 0;
}