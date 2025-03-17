#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SEEK_SET      0      /* Seek relative to beginning of file */
#define SEEK_CUR      1      /* Seek relative to current position in file */
#define SEEK_END      2      /* Seek relative to end of file */

int main() {

    int fd1,fd2, ret;
    char buffer[100];
    size_t nbytes = sizeof(buffer);

    // Open a file for reading
    fd1 = open("Test_read1.txt", O_RDONLY);
    if (fd1 < 0) {
        return 1;
    }
    printf(" fd1 open %d", fd1);

    // Read from the file
    ret = read(fd1, buffer, nbytes);
    printf(" ret read %d", ret);
    if (ret < 0) {
        return 1;
    }

    fd2 = open("Test_read2.txt", O_WRONLY);
    if (fd2 < 0) {
        return 1;
    }
    
    // Seek to a specific position
    ret = lseek(fd2, 0, SEEK_SET);
    printf(" ret lseek %d", ret);
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

    // Close the files
    close(fd1);
    close(fd2);
    return 0;
}