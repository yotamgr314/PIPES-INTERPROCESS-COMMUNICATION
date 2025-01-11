#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <write_fd> <read_fd>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int write_fd = atoi(argv[1]);
    int read_fd = atoi(argv[2]);

    // Open fileB.txt for writing
    int file2 = open("fileB.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (file2 == -1) {
        perror("open fileB.txt");
        exit(EXIT_FAILURE);
    }

    // Child2 Read a message from Child 1
    char buffer[256];
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) // if reaciving the message from child1 was sucesfull.
    {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("\nchild2 Received message from child1 -  %s\n\n", buffer);

        // Write the message to fileB.txt
        write(file2, buffer, bytes_read);
        write(file2, "\n", 1);
    }

    // Send a reply to Child 1
    const char *reply_to_child1 = "Thank you for the informainon, ive recivied it.";
    write(write_fd, reply_to_child1, strlen(reply_to_child1));
    printf("child2: Sends message to child1 -  %s\n\n", reply_to_child1);

    // Close the file descriptors
    close(file2);
    close(read_fd);
    close(write_fd);

    return 0;
}
