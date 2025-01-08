#include <stdio.h>
#include <stdlib.h>
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
    if (write(write_fd, message, sizeof(message)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("A: Sent message to the pipe.\n");
    fflush(stdout);  // Ensure immediate output

    // Wait for a response from B
    char buffer[128];
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("A: Received response from B: %s\n", buffer);
    }

    close(write_fd);
    return 0;
}
