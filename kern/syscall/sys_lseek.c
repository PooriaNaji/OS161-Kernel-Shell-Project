#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/syscall.h>
#include <kern/seek.h>
#include <kern/stat.h>
#include <current.h>
#include <spinlock.h>
#include <synch.h>
#include <kern/iovec.h>
#include <uio.h>
#include <copyinout.h>
#include <syscall.h>
#include <file_table.h>
#include <proc.h>
#include <vfs.h>
#include <vnode.h>

int sys_lseek(int fd, off_t pos, int whence, int32_t *retval)
{
    int result;
    off_t npos = 0;
    struct stat statbuf;

    *retval = -1;

    // Check for invalid file descriptor
    if (fd < 0 || fd > OPEN_MAX)
        return EBADF;

    // Check if the file is seekable
    if (!VOP_ISSEEKABLE(curproc->file_table[fd].node))
        return ESPIPE;

    // Check if the file descriptor has a valid filename buffer
    if (curproc->file_table[fd].filename_buf == NULL) {
        return EBADF;
    }

    // Set the new file position based on the 'whence' parameter
    if (whence == SEEK_SET) {
        // Seek from the beginning of the file
        if (pos < 0) {
            return EINVAL;
        }
        curproc->file_table[fd].offset = pos;
        npos = pos;
    }
    else if (whence == SEEK_CUR) {
        // Seek from the current position in the file
        if (curproc->file_table[fd].offset + pos < 0) {
            return EINVAL;
        }
        curproc->file_table[fd].offset += pos;
        npos = curproc->file_table[fd].offset;
    }
    else if (whence == SEEK_END) {
        // Seek from the end of the file
        result = VOP_STAT(curproc->file_table[fd].node, &statbuf);
        if (result) {
            return result;
        }
        npos = statbuf.st_size;
        if (npos + pos < 0) {
            return EINVAL;
        }
        curproc->file_table[fd].offset = statbuf.st_size + pos;
        npos = curproc->file_table[fd].offset;
    }

    *retval = npos;                 // Set the return value to the new file position

    return 0;
}
