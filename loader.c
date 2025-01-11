// loader.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for pipe(), fork(), close(), write(), read()
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// GLOBAL VARIABLES SECTION
#define MAX_REVIVAL 5

int revivalNumber = 0;
pid_t pid1, pid2;
int pipe_fd_ofChild1WriteChild2Read[2];
int pipe_fd_ofChild2WriteChild1Read[2];

void signalCallBackFunc(int signalNumber) {
    if (signalNumber == SIGUSR1) {
        printf("Received SIGUSR1 from child1. Revival number: %d\n\n", revivalNumber + 1);
        revivalNumber++;
        if (revivalNumber >= MAX_REVIVAL) {
            printf("*********************************************\n");
            printf("Reached maximum revival limit. Exiting...\n");
            printf("*********************************************\n");
            exit(0);
        }
    }

    if (signalNumber == SIGCHLD) {
        int status;
        int waitRes = wait(&status); // Collects the dead child to prevent zombies.

        if (waitRes == pid1) {
            char read_fd_str[10], write_fd_str[10];
            snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe_fd_ofChild2WriteChild1Read[0]);
            snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd_ofChild1WriteChild2Read[1]);
            pid1 = fork();
            if (pid1 == 0) {
                // CHILD1: CLOSE UNUSED PIPE ENDS
                close(pipe_fd_ofChild1WriteChild2Read[0]);
                close(pipe_fd_ofChild2WriteChild1Read[1]);

                execl("./compiledFilesToLoad/A", "./compiledFilesToLoad/A", write_fd_str, read_fd_str, NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }
        } else if (waitRes == pid2) {
            char read_fd_str[10], write_fd_str[10];
            snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe_fd_ofChild1WriteChild2Read[0]);
            snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd_ofChild2WriteChild1Read[1]);
            pid2 = fork();
            if (pid2 == 0) {
                // CHILD2: CLOSE UNUSED PIPE ENDS
                close(pipe_fd_ofChild1WriteChild2Read[1]);
                close(pipe_fd_ofChild2WriteChild1Read[0]);

                execl("./compiledFilesToLoad/B", "./compiledFilesToLoad/B", write_fd_str, read_fd_str, NULL);
                perror("execl");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main() {
    signal(SIGUSR1, signalCallBackFunc);
    signal(SIGCHLD, signalCallBackFunc);

    if (pipe(pipe_fd_ofChild1WriteChild2Read) == -1 || pipe(pipe_fd_ofChild2WriteChild1Read) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (!pid1)// child1 process.
    {
        char read_fd_str[10], write_fd_str[10];
        snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe_fd_ofChild2WriteChild1Read[0]);
        snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd_ofChild1WriteChild2Read[1]);

        // CHILD1: CLOSE UNUSED PIPE ENDS
        close(pipe_fd_ofChild1WriteChild2Read[0]);
        close(pipe_fd_ofChild2WriteChild1Read[1]);

        execl("./compiledFilesToLoad/A", "./compiledFilesToLoad/A", write_fd_str, read_fd_str, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (!pid2)// child2
    {
        char read_fd_str[10], write_fd_str[10];
        snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe_fd_ofChild1WriteChild2Read[0]);
        snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd_ofChild2WriteChild1Read[1]);

        // CHILD2: CLOSE UNUSED PIPE ENDS
        close(pipe_fd_ofChild1WriteChild2Read[1]);
        close(pipe_fd_ofChild2WriteChild1Read[0]);

        execl("./compiledFilesToLoad/B", "./compiledFilesToLoad/B", write_fd_str, read_fd_str, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // LOADER: KEEP THE PIPES OPEN
    while (1) {
        pause();
    }

    return 0;
}










 // sigchild ---> when child dies so that the parent will pick him up (the keep alive stuff).
 // 









/*
    01)NOTE: RULE : can't ever read and write to the same pipe. one pipe allows one process to read OR write only ! 
                    - the reason for this is: 
                        - assume two child process needs to communicate via one pipe - child1 can write message, then reads as he expectes for a response from child2, but child1 will read its own message instead of a response from Child 2.
                        - This is because pipe is unidirectional and do not distinguish between messages sent by different processes.
    02)NOTE: keep in mind - how fork execution work flow:
              - The parent process (let’s call it P) creates a child process (let’s call it C1).
                - Both P and C1 continue executing from the next statement after fork(). even tho all the code of Process P is loaded into process C1

    03)NOTE: Each process has its own memory, and it is not possible to access data directly between different processes. So how does the Pipe used for sharing information between different processes?
              - The pipe is an Operating System (Kernel) Resource mutual to the father process and his children process.
              - When the pipe(fd) function is called, the operating system creates an internal buffer in the kernel's memory, this buffer acts as a FIFO(first in first out queue).
              - The operating system keeps the pipe in its memory and allows processes to access it through File Descriptors (FDs) returned by the pipe() function.
                - the pipe(fd) function returns two file descriptors:
                    - fd[0] for reading from that pipe.
                    - fd[1] for writing to that pipe.
                - these file descriptors are stored in the parent process's file descriptor table.
                - When the parent process calls fork(), the child process inherits the parent's file descriptors.
                    - it means that both the parent and child processes hold references(via the file descriptors) to the same pipe in the kernel's memory.

    04)NOTE: Why Is It Important to Close the Read or Write End of a Pipe When It's Not in Use?
              - A pipe is a system resource (a buffer in the kernel's memory).
                - If you don’t close the file descriptor (FD) after you’ve finished using it, the resource remains occupied, which can cause problems such as:
                    - Resource leaks
                    - Limits on the number of open FDs that a process can handle - if we we will open many pipes without closing them we will get "too many open files" error.
                    - Blocking Reads: as mentioned above the pipe is an Operating System (Kernel) Resource mutual to the father process and his children process.
                        - by default when calling pipe(fd) both the pipe ends (fd[0] && fd[1]) are open !
                        - this means that opening the pipe for write (fd[1]) is a global action which affects all fater process in his children which are reading (fd[0]).
                        - As a result, as long as any process keeps fd[1] (the write end) open any process which tries to read from the pipe using fd[0] will keep waiting for data and will not receive EOF (End of File).

    05)NOTE: when a child dies it sends to his parent a signal calld SIGHCHILD which contains its process id. the father process can register to "listen" to that signal event, by the method wait(int status) or waitpid(int status) and handle that signal - by doing so preventing the child to become a zombie process (process which is not deleted by the system yet becuse the father process hasent picked up its data)

















 */




