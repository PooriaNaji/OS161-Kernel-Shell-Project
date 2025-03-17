#include <types.h>
#include <proc.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>
#include <lib.h>

// System call implementation for getpid
// Retrieves the process ID (p_pid) of the current process and stores it in retval
int sys_getpid(pid_t *retval) {
    // Assign the process ID of the current process to the variable pointed to by retval
    *retval = curproc->p_pid;

    // Print the process ID to the console
    kprintf("pid is %d \r\n", *retval);

    // Return 0 to indicate successful execution of the system call
    return 0;
}
