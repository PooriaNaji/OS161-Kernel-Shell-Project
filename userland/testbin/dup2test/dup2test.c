#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
    
    char buffer[100];
    size_t nbytes = sizeof(buffer);

    // Open a file for reading
    int fd = open("Test_dup1.txt", O_RDONLY);
    printf("open fd: %d",fd);
    // Duplicate the file descriptor to a new file descriptor
    int newfd = dup2(fd,3);
    printf("new fd: %d",newfd);
    // Close the original file descriptor
    close(fd);

    // Read from the duplicated file descriptor
    int ret = read(newfd, buffer, nbytes);
    
    //Open another file
    int fd2 = open("Test_dup2.txt", O_WRONLY);
    if (fd2 < 0) {
        return 1;
    }

    // Seek to a specific position
    ret = lseek(fd2, 0, 0);
    if (ret < 0) {
        close(fd2);
        return 1;
    }

    // Write to the file
    ret = write(fd2, buffer, nbytes);
    if (ret < 0) {
        close(fd2);
        return 1;
    }

    // Close the duplicated file descriptor
    close(newfd);
    close(fd2);
    return 0;
}