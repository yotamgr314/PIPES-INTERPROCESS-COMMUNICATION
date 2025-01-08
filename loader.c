#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for pipe(), fork(), close(), write(), read() */
#include <sys/types.h>
#include <sys/wait.h>

int main() {

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) // NOTE creating internal pipe in the kernal operating system and recive read and write file descriptors to it (fd[0] for read, fd[1] for write).
    { 
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (!pid1) // child1 process 
    {
        // Child process 1: Load A compiled programn.
        char read_fd_str[10], write_fd_str[10];
        snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe_fd[0]);
        snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd[1]);

        execl("./A", "./A", read_fd_str, write_fd_str, NULL); // NOTE: executing the compiled A.c and passing to its main function the file descriptors as variable arguments.

        perror("execl"); // NOTE: if we reach here it means that the execl function as failed.
        exit(EXIT_FAILURE);
    }





    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (!pid2) //  child2 process.
    {
        // Child process 2: Load B comipled programn.
        char read_fd_str[10], write_fd_str[10];
        snprintf(read_fd_str, sizeof(read_fd_str), "%d", pipe_fd[0]);
        snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fd[1]);

        execl("./B", "./B", read_fd_str, write_fd_str, NULL); // NOTE: executing the compiled B.c and passing to its main function the file descriptors as variable arguments.

        perror("execl"); // NOTE: if we reach here it means that the execl function as failed.
        exit(EXIT_FAILURE);
    }


    // Parent process: Wait for both children to finish
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    wait(NULL);
    wait(NULL);

    printf("Both child processes have finished.\n");

    return 0;
}
/* 
    01)NOTE: keep in mind - how fork execution work flow: 
              - The parent process (let’s call it P) creates a child process (let’s call it C1).
                - Both P and C1 continue executing from the next statement after fork(). even tho all the code of Process P is loaded into process C1

    02)NOTE: Each process has its own memory, and it is not possible to access data directly between different processes. So how does the Pipe used for sharing information between different processes?
              - The pipe is an Operating System (Kernel) Resource mutual to the father process and his children process.
              - When the pipe(fd) function is called, the operating system creates an internal buffer in the kernel's memory, this buffer acts as a FIFO(first in first out queue).
              - The operating system keeps the pipe in its memory and allows processes to access it through File Descriptors (FDs) returned by the pipe() function.
                - the pipe(fd) function returns two file descriptors:
                    - fd[0] for reading from that pipe. 
                    - fd[1] for writing to that pipe.
                - these file descriptors are stored in the parent process's file descriptor table.
                - When the parent process calls fork(), the child process inherits the parent's file descriptors.
                    - it means that both the parent and child processes hold references(via the file descriptors) to the same pipe in the kernel's memory. 
        
    03)NOTE: Why Is It Important to Close the Read or Write End of a Pipe When It's Not in Use?
              - A pipe is a system resource (a buffer in the kernel's memory).
                - If you don’t close the file descriptor (FD) after you’ve finished using it, the resource remains occupied, which can cause problems such as:
                    - Resource leaks
                    - Limits on the number of open FDs that a process can handle - if we we will open many pipes without closing them we will get "too many open files" error.
                    - Blocking Reads: as mentioned above the pipe is an Operating System (Kernel) Resource mutual to the father process and his children process.
                        - by default when calling pipe(fd) both the pipe ends (fd[0] && fd[1]) are open ! 
                        - this means that opening the pipe for write (fd[1]) is a global action which affects all fater process in his children which are reading (fd[0]).
                        - As a result, as long as any process keeps fd[1] (the write end) open any process which tries to read from the pipe using fd[0] will keep waiting for data and will not receive EOF (End of File).


















 */



