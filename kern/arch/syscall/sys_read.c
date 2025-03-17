#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <current.h>
#include <spinlock.h>
#include <stat.h>
#include <synch.h>
#include <kern/iovec.h>
#include <uio.h>
#include <copyinout.h>
#include <syscall.h>
#include <file_table.h>
#include <proc.h>
#include <vfs.h>
#include <vnode.h>
#include <limits.h>
#include <kern/fcntl.h>

int sys_read(int fd, userptr_t buf, size_t buflen, int32_t *read) {
    int result;
    struct iovec riov;
    struct uio ruio;

    *read = -1;

    // Check if the file descriptor is valid
    if (fd < 0 || fd >= OPEN_MAX) {
        return EBADF;
    }

    // Check if the buffer pointer is valid
    if (buf == NULL || buflen < 1) {
        return EFAULT;
    }

    // Check if the file is open for writing or if the filename buffer is NULL
    if (curproc->file_table[fd].flags == O_WRONLY || curproc->file_table[fd].filename_buf == NULL) {
        return EBADF;
    }

    // Allocate kernel buffer for reading data from the file
    char *kbuffer = (char *)kmalloc(sizeof(*buf) * buflen);

    // Set up the read I/O vector
    uio_kinit(&riov, &ruio, kbuffer, buflen, curproc->file_table[fd].offset, UIO_READ);

    // Read data from the file using VOP_READ
    result = VOP_READ(curproc->file_table[fd].node, &ruio);
    if (result) {
        kfree(kbuffer);
        return result;
    }

    // Calculate the number of bytes read
    *read = buflen - ruio.uio_resid;
    // Update the file offset
    curproc->file_table[fd].offset = ruio.uio_offset;

    // Copy data from the kernel buffer to the user buffer
    if (read) {
        result = copyout(kbuffer, buf, *read);
        if (result) {
            kfree(kbuffer);
            return result;
        }
        kprintf("%s",kbuffer); //////
    }

    // Free the kernel buffer and return success
    kfree(kbuffer);
    return 0;
}
