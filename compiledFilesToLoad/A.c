#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <read_fd> <write_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert file descriptors from strings to integers
    int read_fd = atoi(argv[1]);
    int write_fd = atoi(argv[2]);

    // Close the read end since this process will only write
    close(read_fd);

    // Write a message to the pipe
    const char *message = "Hello from A!";
    if (write(write_fd, message, strlen(message)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("A: Sent message to the pipe.\n");
    fflush(stdout);

    // Close the write end
    close(write_fd);

    return 0;
}
