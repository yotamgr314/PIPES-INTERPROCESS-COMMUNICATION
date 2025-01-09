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

    // Close the unused read end
    close(read_fd);

    // Send a message to Child 2
    const char *message_to_child2 = "Hello from A!";
    write(write_fd, message_to_child2, strlen(message_to_child2));
    printf("A: Sent message to B: %s\n", message_to_child2);

    // Close the write end after sending the message
    close(write_fd);

    return 0;
}
