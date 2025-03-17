#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <current.h>
#include <kern/iovec.h>
#include <uio.h>
#include <copyinout.h>
#include <syscall.h>
#include <file_table.h>
#include <kern/fcntl.h>
#include <proc.h>
#include <stat.h>
#include <addrspace.h>
#include <vfs.h>
#include <vnode.h>

int sys_open(const char *filename, int flags, int32_t *fd) {
    struct vnode *file;
    mode_t mode;
    size_t len;
    int result;
    char *fnamebuf = NULL;

    file = NULL;
    mode = 0;

    // Check the flags for valid combinations
    switch(flags) {
        case O_RDONLY: break;
        case O_WRONLY: break;
        case O_RDWR: break;
        case O_RDONLY|O_CREAT: break;
        case O_WRONLY|O_CREAT: break;
        case O_RDWR|O_CREAT: break;
        case O_RDWR|O_CREAT|O_TRUNC: break;
        case O_WRONLY|O_APPEND: break;
        case O_RDWR|O_APPEND: break;      
        default:
            return EINVAL;
    }

    // Check if the filename pointer is valid
    if (filename == NULL)
        return EFAULT;

    // Calculate the length of the filename
    len = strlen(filename) + 1;
    
    // Allocate memory for the filename buffer
    fnamebuf = kmalloc(len * sizeof(char));
    if (fnamebuf == NULL)
        return ENOMEM;

    // Copy the filename from user space into kernel space
    result = copyinstr((const_userptr_t)filename, fnamebuf, len, NULL);
    if (result)
        goto err;

    // Open the file using the VFS layer
    result = vfs_open(fnamebuf, flags, mode, &file);
    if (result)
        goto err;

    int index = 0;
    // Find an available index in the file table
    for (int i = 3; i < OPEN_MAX; i++) {
        if (curproc->file_table[i].filename_buf == NULL) {
            index = i;
            break;
        }
    }

    // Set the file table entry values
    curproc->file_table[index].node = file;
    curproc->file_table[index].flags = flags;
    curproc->file_table[index].offset = 0;
    strcpy(curproc->file_table[index].filename_buf, fnamebuf);

    // If O_APPEND flag is set, set the offset to the end of the file
    if ((flags & O_APPEND) != 0) {
        struct stat file_stat;
        int result = VOP_STAT(file, &file_stat);
        if (result)
            goto err;
        curproc->file_table[index].offset = file_stat.st_size;
    }

    *fd = index;
    // kprintf("open ok \n");
    return 0;

err:
    // kprintf("open nnok \n");
    return result;
}
