#include <types.h>
#include <kern/errno.h>
#include <proc.h>
#include <thread.h>
#include <mips/trapframe.h>
#include <current.h>
#include <syscall.h>
#include <vnode.h>
#include <addrspace.h>
#include <file_table.h>
#include <spl.h>

void help_enter_forked_process(void *data1, unsigned long data2);

int sys_fork(struct trapframe *tf, int32_t *ret)
{
    int result;
    struct proc *src, *newp;  // src=parent newp=child
    struct trapframe *child_tf;  // new trapframe

    src = curproc;

    //int spl = splhigh();
    *ret = -1;

    // Allocate memory for the new trapframe
    child_tf = kmalloc(sizeof(*child_tf));
    if (child_tf == NULL)
        return ENOMEM;
    // Copy the trapframe of the current process to the new trapframe
    memcpy(child_tf, tf, sizeof(*child_tf));
    newp = NULL;
    // Create a new process for the child
    newp = proc_create_runprogram(src->p_name);
    if (newp == NULL) {
        return ENOMEM;
    }
   // newp->parent = src;
  //  newp->p_pid = src->pid;

    struct addrspace *src_addrspace = src->p_addrspace;
    struct addrspace *newp_addrspace;

    // Copy the address space of the parent to the child
    result = as_copy(src_addrspace, &newp_addrspace);
    if(result){
        proc_destroy(newp);
        return result;
    }
    newp->p_addrspace = newp_addrspace;

    // Copy the file table entries from the parent to the child
    for (int i = 0; i < 64; i++) {
        strcpy(newp->file_table[i].filename_buf, src->file_table[i].filename_buf);
        newp->file_table[i].flags = src->file_table[i].flags;
        newp->file_table[i].offset = src->file_table[i].offset;
    }

    spinlock_acquire(&src->p_lock);
    // Copy the current working directory from the parent to the child
    if (src->p_cwd != NULL) {
        VOP_INCREF(src->p_cwd);
        newp->p_cwd = src->p_cwd;
    }
    spinlock_release(&src->p_lock);

    *ret = newp->pid;
    // Fork a new thread for the child process
    thread_fork(src->p_name, newp,
                help_enter_forked_process,
                (void *) child_tf, 0);

    //splx(spl);
    return 0;
}

void help_enter_forked_process(void *data1, unsigned long data2){

    (void) data2;
    // Get the new trapframe

    KASSERT(curthread != NULL);
    KASSERT(curthread->t_curspl == 0);
    KASSERT(curthread->t_iplhigh_count == 0);
    struct trapframe *ntf = (struct trapframe *) data1;
    // Activate the address space of the child process
    as_activate();

    ntf->tf_v0 = 0;
    ntf->tf_a3 = 0;
    ntf->tf_epc += 4;

    kfree(data1);
    mips_usermode(ntf);

}
