#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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

    // Read and send chunks to B
    char buffer[256];
    ssize_t bytes;
    while ((bytes = read(file, buffer, sizeof(buffer))) > 0) {
        write(write_fd, buffer, bytes);
    }
    close(file);

    // Read response from B
    char response[256];
    read(read_fd, response, sizeof(response));
    printf("Child A received response: %s\n", response);

    // Close the file descriptors
    close(write_fd);
    close(read_fd);

    // Print final message before exit
    printf("Child A: My life has reached its worthy end. Goodbye.\n");
    exit(0);
}
