This repository contains the source code developed as part of a group project during the Operating Systems course at Politecnico di Torino. The work was completed by three students as part of the course’s main assignment on kernel-level programming using the OS/161 educational operating system.

The project focused on implementing key system calls, enabling multi-process execution, exception handling, and supporting a functional user-level shell. The result was a working shell environment that executed user programs such as `pwd`, `cat`, and `/testbin/sort` through a fully implemented syscall interface.

---

## Project Scope and Learning Goals

This project simulated the work of operating system developers, requiring us to:
- Build a minimal syscall interface (`fork`, `execv`, etc.)
- Set up trap handling and return from user-kernel transitions
- Manage virtual memory (`dumbvm`) and address spaces
- Implement process management and file descriptor logic
- Enable a working command-line shell (`/bin/sh`) under the OS/161 kernel

---

## 🔧 Implemented System Calls

| Process Management | File I/O     | Filesystem     |
|--------------------|--------------|----------------|
| `fork`             | `read`       | `open`         |
| `execv`            | `write`      | `close`        |
| `waitpid`          | `lseek`      | `chdir`        |
| `_exit`            | `dup2`       | `__getcwd`     |
| `getpid`           |              |                |

All system calls were implemented using:
- User-kernel memory copying with `copyin`, `copyout`
- Trapframe preservation
- PID and process table design
- Kernel synchronization and file handle tracking

---

##  Process and Memory Management

- `fork()` was implemented to create a child thread from a parent, duplicating the address space
- `execv()` was used to load an ELF binary and replace the currently running program
- User arguments were passed using `as_define_stack` and low-level memory routines
- The memory model was managed via `dumbvm`, with virtual addresses manually set up

---

##  User Shell and Program Execution

- The user shell `/bin/sh` was executed from the OS161 kernel and successfully invoked programs such as:
  ```sh
  OS/161 shell: pwd
  /
  OS/161 shell: cat hello.txt
  Hello from kernel world!
  OS/161 shell: /testbin/halt
  ```
- Background execution (`&`) and argument passing were supported
- Shell commands internally used `fork` and `execv` via userland system call interfaces

---

##  Folder Structure

| Folder         | Description                                       |
|----------------|---------------------------------------------------|
| `kern/`        | Kernel source (syscalls, traps, memory, process) |
| `userland/`    | User programs and shell testing                  |
| `man/`         | Man pages and documentation                      |
| `mk/`          | Build system definitions                         |
| `design/`      | Planning and group notes (if applicable)         |
| `testscripts/` | Testing tools for syscall evaluation             |

---

## 🛠️ How to Build and Run

### Prerequisites
- OS161 toolchain (`os161-gcc`, `as`, `ld`)
- System/161 simulator
- Cross-GDB (optional)

### Kernel Build
```bash
cd os161-root
./configure ASST2
bmake
bmake install
```

### Run in Emulator
```bash
sys161 kernel
```

From the OS/161 menu:
```sh
p /bin/sh
```

---

##  Contributors (Group Work)

- **Pooria Naji Zavar Garmroudi** – System call implementation, fork-execv logic, shell testing , GitHub repository creation
- **[Payamreza Pourreza](https://github.com/PYR-PRZ)** – Process structure, address space model (`dumbvm`), user program testing
- **Mina Samadian** – File descriptor handling, documentation

---

##  Learning Outcomes

Through this project, the following concepts were practiced:
- Kernel-mode system call implementation and error handling
- Trap dispatch and user-kernel register state transitions
- Memory copying and trapframe design
- File descriptor tracking and user-space argument parsing
- ELF loading and context switching into user mode
- Process lifecycle design and basic file system navigation

---

##  Limitations and Challenges

- The memory system is based on `dumbvm`, without paging or protection
- The implementation remains single-threaded without concurrency support
- File locking, synchronization primitives, and exception recovery were limited
- Debugging was performed manually using kernel messages and reboots, as no symbolic debugging tools were available
- Advanced features such as signals, pipes, inter-process communication, and scheduling policy design were outside the defined scope

---

##  Developer Reflections

This project was completed under academic constraints within a multi-member team. Although the technical objectives were well defined, several aspects of the implementation required significant effort and low-level debugging. The system call logic for `fork`, `execv`, and `getpid` was implemented by the first contributor. These functions required the handling of trapframe preservation, argument copying, process duplication, and user-kernel memory transitions.

Validation was conducted through shell-based testing, by executing multiple programs in user space using the system calls. In the absence of traditional tools such as GDB, debugging was performed manually via console output, frequent reboots, and observation of kernel messages.

This repository has intentionally been preserved in its original academic form to reflect the learning process as experienced during development. The resulting implementation should be regarded as functional and educational rather than fully optimized or production-ready. The limitations described above are recognized as consequences of the project’s intended scope and available development environment.

The present repository was assembled and uploaded by Pooria Naji Zavar Garmroudi for documentation and reference purposes. Although the original work was conducted collaboratively, this version reflects the contributor's own development effort, structure, and documentation as an individual submission artifact.

---
##  Reference Documents & Attribution

This project was completed as part of the Operating Systems course at **Politecnico di Torino**, under the supervision of:

**Prof. Stefano Di Carlo**  
GitHub: [japanninja74](https://github.com/japanninja74)  
Email: stefano.dicarlo@polito.it

The course involved kernel-level development using the OS/161 educational operating system. This repository contains the implementation of **Project 2: System Calls and Shell Interface** from the official assignment brief.

To provide full context, the following supporting materials are included:

- [`docs/OS-Presentation.pdf`](./docs/OS-Presentation.pdf)  
  📑 **Final project presentation** prepared by our group.

- [`docs/Professor_Project_Description.pdf`](./docs/Professor_Project_Description.pdf)  
  📜 **Official course assignment document** provided by Professor Stefano Di Carlo.  
  This document includes **two projects**:
  - 2 **Implemented by our group**: *Project 2 - System Calls and Shell Interface in OS161*
  - 1 Not implemented by us: *Project 1 - Synchronization Primitives and Threading*  
  Only **Project 2** is implemented and available in this repository.


## 📄 License

This project is licensed under the MIT License.
