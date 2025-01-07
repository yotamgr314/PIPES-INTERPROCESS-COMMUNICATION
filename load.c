#include <stdio.h>
#include <unistd.h> /* for pipe(), fork(), close(), write(), read() */
#include <pthread.h> /* for pid_t */



int main() {
    int fd[2];  // NOTE: creates an array to contain the read&&write file descriptos of the kernel's pipe.
    
    if (pipe(fd) == -1) // NOTE creating internal pipe in the kernal operating system and recive read and write file descriptors to it (fd[0] for read, fd[1] for write).
    {
        perror("Pipe failed");
        return 1;
    }

/*  01)NOTE: Each process has its own memory, and it is not possible to access data directly between different processes. So how does the Pipe used for sharing information between different processes?
              - The pipe is an Operating System (Kernel) Resource mutual to the father process and his children process.
              - When the pipe(fd) function is called, the operating system creates an internal buffer in the kernel's memory, this buffer acts as a FIFO(first in first out queue).
              - The operating system keeps the pipe in its memory and allows processes to access it through File Descriptors (FDs) returned by the pipe() function.
                - the pipe(fd) function returns two file descriptors:
                    - fd[0] for reading from that pipe. 
                    - fd[1] for writing to that pipe.
                - these file descriptors are stored in the parent process's file descriptor table.
                - When the parent process calls fork(), the child process inherits the parent's file descriptors.
                    - it means that both the parent and child processes hold references(via the file descriptors) to the same pipe in the kernel's memory. 
        
    02)NOTE: Why Is It Important to Close the Read or Write End of a Pipe When It's Not in Use?
              - A pipe is a system resource (a buffer in the kernel's memory).
                - If you don’t close the file descriptor (FD) after you’ve finished using it, the resource remains occupied, which can cause problems such as:
                    - Resource leaks
                    - Limits on the number of open FDs that a process can handle - if we we will open many pipes without closing them we will get "too many open files" error.
                    - Blocking Reads: as mentioned above the pipe is an Operating System (Kernel) Resource mutual to the father process and his children process.
                        - by default when calling pipe(fd) both the pipe ends (fd[0] && fd[1]) are open ! 
                        - this means that opening the pipe for write (fd[1]) is a global action which affects all fater process in his children which are reading (fd[0]).
                        - As a result, as long as any process keeps fd[1] (the write end) open any process which tries to read from the pipe using fd[0] will keep waiting for data and will not receive EOF (End of File).
 */ 

    pid_t pid = fork();  // creating the first child. 
    if (!pid) { //child process code.
        close(fd[1]); // close the writing pipe end. 

        char buffer[20];
        read(fd[0], buffer, sizeof(buffer));  // reads from the pipe, saves the data into the buffer, and reads exactly 20 bytes from the buffer. 
        printf("Child received: %s\n", buffer);
        close(fd[0]); // close the reading pipe end.

    } else { // father process code.
        close(fd[0]); // close the reading pipe end. 
        char messageBuffer[] = "Hello from parent!";
        write(fd[1], messageBuffer, sizeof(messageBuffer));  // writing messageBuffer size of byes from the messageBuffer into the pipe.
        close(fd[1]); // closes the writing pipe end.
    }

    return 0;
}

