#include <types.h>
#include <kern/errno.h>
#include <kern/wait.h>
#include <limits.h>
#include <proc.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>
#include <copyinout.h>
#include <addrspace.h>
#include <wchan.h>

int sys_waitpid(pid_t pid, int *status, int options, int32_t *retval) {
    int result, p_status;

    // Ensure that the current process exists
    KASSERT(curproc != NULL);

    result = 0;
    *retval = -1;

    // Check if the given process ID is valid
    if (pid <= 0 || proc_table[pid] == NULL) {
        *retval = -1;
        return EINVAL;
    }
    kprintf("here1 \n\r");
    /* Filter out unsupported options */
    if (options != 0) {
        *retval = -1;
        return EINVAL;
    }
kprintf("here2 \n\r");
    // Check if the status pointer is valid
    if (status == NULL) {
        *retval = -1;
        return EFAULT;
    }
kprintf("here3 \n\r");
    // Acquire the lock for the parent process
  //  spinlock_acquire(&proc_table[proc_table[pid]->p_pid]->p_lock);////////////

    // Wait until the process with the given ID has exited
    while (proc_table[pid]->exited == false);

    // Release the lock for the parent process
  //  spinlock_release(&proc_table[proc_table[pid]->p_pid]->p_lock);

    // Copy the process status to the user-provided status pointer
    if (status) {
        result = copyout((const void *)&p_status, (userptr_t)status, sizeof(int));
        if (result)
            return result;
    }

    // Set the return value to the given process ID
    *retval = pid;

    return 0;
}
