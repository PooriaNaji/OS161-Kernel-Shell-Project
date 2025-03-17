#include <types.h>
#include <limits.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <proc.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>
#include <vnode.h>
#include <vfs.h>
#include <addrspace.h>
#include <copyinout.h>
#include <file_table.h>

int sys_execv(const char *program, char **args, int32_t *retval)
{
    int result, arg_size;
    char **kargs;
    size_t psize;
    char p_buffer[255];
    struct vnode *v;
    struct addrspace *as = as_create();        // Create a new address space
    struct addrspace *old_as = proc_getas();   // Get the current address space
    vaddr_t entrypoint, stackptr;

    *retval = -1;

    if (program == NULL || args == NULL)
        return EFAULT;

    /* Copy 'program' string to kernel space */
    result = copyinstr((const_userptr_t)program, p_buffer, 255, &psize);
    if (result) {
        return result;
    }

    int argc = 0;
    while (args[argc] != NULL)
        argc++;

    // Allocate memory for kernel space arguments
    kargs = kmalloc((argc + 1) * sizeof(char *));
    if (kargs == NULL)
        return ENOMEM;

    // Copy arguments to kernel space
    for (int i = 0; i < argc; i++) {
        char arg[10];
        size_t arglen;

        result = copyinstr((const_userptr_t)args[i], arg, 255, &arglen);
        if (result) {
            kfree(kargs);
            return result;
        }

        kargs[i] = kstrdup((const char *)args[i]);
        if (kargs[i] == NULL) {
            kfree(kargs);
            return ENOMEM;
        }
    }

    kargs[argc] = NULL;

    // Create a new address space
    if (as == NULL) {
        proc_setas(old_as);  // Set the old address space back
        return ENOMEM;
    }
    proc_setas(as);     // Set the new address space as the current address space
    as_activate();      // Activate the new address space

    // Open the executable file
    result = vfs_open(p_buffer, O_RDONLY, 0, &v);
    if (result) {
        proc_setas(old_as);  // Set the old address space back
        return result;
    }

    // Load the executable
    result = load_elf(v, &entrypoint);
    if (result) {
        proc_setas(old_as);  // Set the old address space back
        vfs_close(v);
        return result;
    }

    // Close the file
    vfs_close(v);

    // Calculate the size of the argument stack
    arg_size = 0;
    for (int i = 0; i < argc; i++) {
        arg_size += strlen(kargs[i]) + 1; // Include null terminator
    }

    stackptr = USERSTACK - arg_size;

    // Allocate memory for the user stack
    char *user_stack = kmalloc(arg_size);
    if (user_stack == NULL) {
        proc_setas(old_as);     // Set the old address space back
        as_destroy(as);         // Destroy the new address space
        kfree(kargs);
        return ENOMEM;
    }

    vaddr_t arg_ptr = stackptr;

    // Copy arguments to the user stack
    for (int i = 0; i < argc; i++) {
        size_t arg_len = strlen(kargs[i]) + 1; // Include null terminator

        result = copyoutstr(kargs[i], (userptr_t)arg_ptr, arg_len, NULL);
        if (result) {
            proc_setas(old_as);     // Set the old address space back
            as_destroy(as);         // Destroy the new address space
            kfree(kargs);
            kfree(user_stack);
            return result;
        }

        arg_ptr += arg_len;
        kfree(kargs[i]);
    }

    kfree(kargs);

    // Define the user stack in the address space
    result = as_define_stack(as, &stackptr);
    if (result) {
        return result;
    }

    // Warp to user mode
    enter_new_process(argc, (userptr_t)stackptr,
                      NULL /*userspace addr of environment*/,
                      stackptr, entrypoint);

    // enter_new_process does not return
    kprintf("[!] sys_execv: enter_new_process returned\n");
    return EINVAL;
}
