#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <read_fd> <write_fd>\n", argv[0]); // printing error message in the standart error channel. <read_fd> and <write_fd> are just placeholders to the expected arguments and argv[0] is the name of the programmn (as we passed via excv in loader
        exit(EXIT_FAILURE);
    }

    // NOTE: Convert file descriptors from strings to integers (becuse when we passed the file descriptors to the const char* argv[] we are to convert them to their corresponding char* values).
    int read_fd = atoi(argv[1]);
    int write_fd = atoi(argv[2]);

     //NOTE: int close(int fd); --> Closes an open file descriptor.
    close(write_fd);// Close the write end since this process will only read

    
    char buffer[128]; // Read a message from the pipe

    //NOTE: ssize_t read(int fd, void *buf, size_t count); --> int fd is the file desctriptor to read from (pipe/file/socket). void * buf is a pointer to the buffer where the read data will be stored. size_t count is the maximu, mnumber of bytes to be read into the buffer.
    ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("B: Received message: %s\n", buffer);
    } else {
        perror("read");
    }

    // Close the read end
    close(read_fd);

    return 0;
}
