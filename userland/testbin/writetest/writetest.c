#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int fd, ret;
    const char *message = " | WOW this is working!";
    size_t nbytes = strlen(message);

    // Open a file for writing
    fd = open("Test_write.txt", O_WRONLY | O_CREAT);
    printf(" fd open: %d \r\n",fd);
    if (fd < 0) {
        close(fd);
        return 1;
    }

    // Write to the file
    ret = write(fd, message, nbytes);
    printf(" ret write: %d \r\n",fd);
    if (ret < 0) {
        close(fd);
        return 1;
    }

    // Close the file
    close(fd);

    return 0;
}
