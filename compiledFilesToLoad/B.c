#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <write_fd> <read_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert the file descriptor strings to integers
    int write_fd = atoi(argv[1]);
    int read_fd = atoi(argv[2]);

    // Read a message from Child 1
    char buffer[256];

    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("B: Received message from A: %s\n", buffer);
    }

    // Send a reply to Child 1
    const char *reply_to_child1 = "Thank you from child2!";
    write(write_fd, reply_to_child1, strlen(reply_to_child1));
    printf("B: Sent reply to A: %s\n", reply_to_child1);

    // Close the file descriptors
    close(read_fd);
    close(write_fd);

    return 0;
}
