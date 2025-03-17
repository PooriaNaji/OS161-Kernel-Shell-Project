#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/syscall.h>
#include <current.h>
#include <vfs.h>
#include <copyinout.h>
#include <proc.h>
#include <uio.h>
#include <spinlock.h>
#include <syscall.h>
#include <file_table.h>

int sys___getcwd(char *buf, size_t buflen, int32_t *retval)
{
    // Create necessary variables
    struct iovec bufdata; // I/O vector for buffer data
    struct uio ubuf; // User I/O buffer
    int result;

    *retval = -1;

    if (buf == NULL)
        return EFAULT; // Check if the buffer pointer is valid

    // Set up I/O vector and user I/O buffer
    bufdata.iov_ubase = (userptr_t) buf; // Set base address of the I/O vector to the buffer
    bufdata.iov_len = buflen; // Set the length of the I/O vector
    ubuf.uio_iov = &bufdata; // Set the I/O vector in the user I/O buffer
    ubuf.uio_iovcnt = 1; // Set the count of I/O vectors in the user I/O buffer
    ubuf.uio_offset = 0; // Set the offset of the user I/O buffer
    ubuf.uio_resid = buflen; // Set the remaining size of the user I/O buffer
    ubuf.uio_segflg = UIO_USERSPACE; // Set the user I/O buffer segment flag to indicate user space
    ubuf.uio_rw = UIO_READ; // Set the user I/O buffer read/write flag to read
    ubuf.uio_space = curproc->p_addrspace; // Set the user I/O buffer's address space to the current process's address space

    // Call the VFS function to get the current working directory
    result = vfs_getcwd(&ubuf);
    if (result)
        return result; // Return the error if VFS function fails

    // Calculate the length of the current working directory path
    *retval = buflen - ubuf.uio_offset;

    return 0; // Return success
}
