#include <types.h>
#include <proc.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>

int sys_exit(int status)
{
    // Get a reference to the current process
    struct proc *proc = curproc;

    // Ensure that the process is not NULL
    KASSERT(proc != NULL);

    // Set the exit status of the process
    proc->exitstatus = status;

    // Mark the process as exited and completed
    proc->exited = true;
    proc->completed = true;

    // Exit the current thread
    thread_exit();

    // Uncomment the following line if you want to print the exit status
    // kprintf("%d", status);

    // Return the exit status
    return status;
}
