#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <read_fd> <write_fd>\n", argv[0]); // printing error message in the standart error channel. <read_fd> and <write_fd> are just placeholders to the expected arguments and argv[0] is the name of the programmn (as we passed via excv in loader.c)
        exit(EXIT_FAILURE);
    }

    // NOTE: Convert file descriptors from strings to integers (becuse when we passed the file descriptors to the const char* argv[] we are to convert them to their corresponding char* values).
    int read_fd = atoi(argv[1]);
    int write_fd = atoi(argv[2]);

    
    //NOTE: int close(int fd); --> Closes an open file descriptor.
    close(read_fd); // Close the read end since this process will only write

    const char *message = "Hello from A!";
    //NOTE: size_t write(int fileDescriptor, const void *buf, size_t count); --> where int filedescriptor file descriptor to which you want to write data. void* buf is a pointer to the buffer containing the data to write. and size_t count is The number of bytes to write from the buffer.
    if (write(write_fd, message, strlen(message)) == -1) // write a message ot the pipe.
    {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("A: Sent message to the pipe.\n");
    fflush(stdout);

    // Close the write end
    close(write_fd);

    return 0;
}
