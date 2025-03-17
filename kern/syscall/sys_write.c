#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>
#include <kern/limits.h>
#include <current.h>
#include <synch.h>
#include <kern/iovec.h>
#include <uio.h>
#include <copyinout.h>
#include <syscall.h>
#include <proc.h>
#include <vfs.h>
#include <vnode.h>

int sys_write(int fd, const_userptr_t buf, size_t buflen, int32_t *wrote) {
    int result;
    struct iovec wiov; // Structure for I/O vectors
    struct uio wuio; // Structure for user I/O

    
    result = 0;
    *wrote = -1;
	int _fd = fd;

    if (fd < 0 || fd >= OPEN_MAX) {
        return EBADF; // Invalid file descriptor
    }

    if (buf == NULL ) {
        return EFAULT; // Invalid buffer or buffer length
    }

    if (fd < 3) {
        // Assign the corresponding vnode for stdin, stdout, stderr
        struct vnode *node = NULL; 
        switch (fd) {
            case STDIN_FILENO:
                _fd = curproc->std_in;
                break;
            case STDOUT_FILENO:
                _fd = curproc->std_out;
                break;
            case STDERR_FILENO:
                _fd = curproc->std_err;
                break;
            default:
                return EBADF; // Invalid file descriptor
        }

        char filename_buf[10];

		strcpy(filename_buf,curproc->file_table[_fd].filename_buf);

        int res = vfs_open(filename_buf, O_RDWR, 0, &node);
        if (res != 0)
            return res; // Error opening the vnode

        // Update the file_table entry for stdin, stdout, stderr
        curproc->file_table[_fd].node = node;

        curproc->file_table[_fd].flags = O_RDWR;


    }



    if (curproc->file_table[_fd].filename_buf == NULL || curproc->file_table[_fd].flags == O_RDONLY) {
        return EBADF; // Invalid file descriptor or read-only mode
    }



    uio_kinit(&wiov, &wuio, (void *)buf, buflen, curproc->file_table[_fd].offset,UIO_WRITE);

    // Perform the write operation
    result = VOP_WRITE(curproc->file_table[_fd].node, &wuio);
    if (result)
        return result; // Error writing to the vnode

    curproc->file_table[_fd].offset = wuio.uio_offset;
    *wrote = buflen - wuio.uio_resid;

    return 0; // Success
}
