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

    // Close the write end since this process will only read
    close(write_fd);

    // Read a message from the pipe
    char buffer[128];
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("B: Received message: %s\n", buffer);

        // Send a response back to A
        const char *response = "Thank you, A!";
        if (write(write_fd, response, sizeof(response)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("read");
    }

    close(read_fd);
    return 0;
}
