#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <write_fd> <read_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int write_fd = atoi(argv[1]);
    int read_fd = atoi(argv[2]);

    // Open fileA.txt for reading
    int file = open("fileA.txt", O_RDONLY);
    if (file == -1) {
        perror("open fileA.txt");
        exit(EXIT_FAILURE);
    }

    // child1 reads from file1 and send chunks to child2.
    char buffer[256];
    ssize_t bytes;
    printf("\nchild1: sends the content he read from file1 to child2.\n");
    while ((bytes = read(file, buffer, sizeof(buffer))) > 0)
    {
        write(write_fd, buffer, bytes);
    }

    close(file);

    // child1 Read response message from childs2 via the pipe.
    char response[256];
    read(read_fd, response, sizeof(response));
    printf("Child1: recivied response from child2 - %s\n", response);

    // Print final message before exit
    printf("Child1: My life has reached its worthy end. Goodbye.\n");

    //Send SIGUSR1 signal to loader (the parent process) before exiting so it can inc the revival counter.
    kill(getppid(), SIGUSR1);
    printf("Child1: Sent SIGUSR1 to loader before exiting.\n\n");

    // Close the file descriptors
    close(write_fd);
    close(read_fd);

    exit(0);
}
