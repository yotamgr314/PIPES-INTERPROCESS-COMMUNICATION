#include <stdio.h>
#include <unistd.h> /* for pipe(), fork(), close(), write(), read() */
#include <pthread.h>> /* for pid_t */



int main() {
    int fd[2];  // NOTE: creates an array to contain the read&&write file descriptos of the kernel's pipe.
    
    if (pipe(fd) == -1) // NOTE using the 
    {
        perror("Pipe failed");
        return 1;
    }

/* NOTE: Each process has its own memory, and it is not possible to access data directly between different processes. So how does the Pipe used for sharing information between different processes?
         01)  The pipe is an Operating System (Kernel) Resource.
              - When the pipe(fd) function is called, the operating system creates an internal buffer in the kernel's memory, this buffer acts as a FIFO(first in first out queue).
              - The operating system keeps the pipe in its memory and allows processes to access it through File Descriptors (FDs) returned by the pipe() function.
                - the pipe(fd) function returns two file descriptors:
                    - fd[0] for reading from that pipe. 
                    - fd[1] for writing to that pipe.
                - these file descriptors are stored in the parent process's file descriptor table.
                - When the parent process calls fork(), the child process inherits the parent's file descriptors.
                    - it means that both the parent and child processes hold references(via the file descriptors) to the same pipe in the kernel's memory. 
              






 */ 




    pid_t pid = fork();  // יצירת תהליך ילד

    if (pid == 0) {
        // קוד של תהליך הילד
        close(fd[1]);  // סוגר את צד הכתיבה של ה-Pipe
        char buffer[20];
        read(fd[0], buffer, sizeof(buffer));  // קורא מה-Pipe
        printf("Child received: %s\n", buffer);
        close(fd[0]);  // סוגר את צד הקריאה
    } else {
        // קוד של תהליך האב
        close(fd[0]);  // סוגר את צד הקריאה של ה-Pipe
        char message[] = "Hello from parent!";
        write(fd[1], message, sizeof(message));  // כותב ל-Pipe
        close(fd[1]);  // סוגר את צד הכתיבה
    }

    return 0;
}

