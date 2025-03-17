#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/syscall.h>
#include <current.h>
#include <proc.h>
#include <spinlock.h>
#include <syscall.h>

int sys_dup2(int oldfd, int newfd, int32_t *fd)
{
    *fd = -1; // Initialize the return value

    // Check if oldfd is a valid file descriptor
    if (oldfd < 0 || oldfd >= OPEN_MAX)
        return EBADF;

    // Check if newfd is a valid file descriptor
    if (newfd < 0 || newfd >= OPEN_MAX)
        return EBADF;

    // If oldfd and newfd are the same, no need to duplicate, return newfd
    if (oldfd == newfd) {
        *fd = newfd;
        return 0;
    }
    kprintf("here");
    // Check if the filename buffer of newfd is not NULL
    if (curproc->file_table[newfd].filename_buf != NULL) {
        // Clear the filename buffer of newfd
        char empty[255];
        strcpy(curproc->file_table[newfd].filename_buf, empty);
    }
    // Duplicate the file_table_entry from oldfd to newfd
    memcpy(&curproc->file_table[newfd], &curproc->file_table[oldfd], sizeof(struct file_table_entry));

    *fd = newfd; // Set the return value to newfd

    return 0;
}
