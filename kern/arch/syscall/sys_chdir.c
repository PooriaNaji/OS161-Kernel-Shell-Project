#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/syscall.h>
#include <current.h>
#include <vfs.h>
#include <copyinout.h>
#include <proc.h>
#include <spinlock.h>
#include <syscall.h>
#include <addrspace.h>

int sys_chdir(const char * pathname) {

    //char kpath[255];  // Buffer to store the kernel path
    int result;
    size_t len;
    char *path;

    if (pathname == NULL)
        return EFAULT;  // Return an error if the pathname is NULL


    len = strlen(pathname) + 1;

    path = kmalloc(len);
    if (path == NULL)
        return ENOMEM;


    result = copyinstr((const_userptr_t)pathname, path, len, NULL);  // Copy the user-provided pathname to kernel space
    if (result)
        goto err;  // If there was an error during copying, go to the error handling section

    result = vfs_chdir(path);  // Call the vfs_chdir function to change the current working directory
    if (result)
        goto err;  // If there was an error during the vfs_chdir call, go to the error handling section

    return 0;  // Return 0 to indicate success

err:
    kfree(path);  // Free the kernel path buffer in case of an error
    return result;  // Return the error code
}
