#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/syscall.h>
#include <current.h>
#include <synch.h>
#include <kern/iovec.h>
#include <uio.h>
#include <copyinout.h>
#include <syscall.h>
#include <proc.h>
#include <vfs.h>
#include <vnode.h>

int sys_close(int fd, int32_t *result)
{
    // Check if the file descriptor is valid
    if (fd >= OPEN_MAX || fd < 0) {
        return EBADF; // Return error if file descriptor is out of range
    }

    // Check if the file table entry is valid
    if (curproc->file_table[fd].filename_buf == NULL) {
        return EBADF; // Return error if file table entry is invalid
    }

    // Clear the filename buffer associated with the file table entry
    char empty[255];
    strcpy(curproc->file_table[fd].filename_buf, empty);

    *result = 0; // Set the result to 0 to indicate success

    return 0; // Return success
}
