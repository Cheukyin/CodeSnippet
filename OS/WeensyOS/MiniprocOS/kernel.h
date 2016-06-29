#ifndef WEENSYOS_KERNEL_H
#define WEENSYOS_KERNEL_H
#include "const.h"
#include "x86.h"

// Process state type
typedef enum procstate {
	P_EMPTY = 0,			// The process table entry is empty
					// (i.e. this is not a process)
	P_RUNNABLE,			// This process is runnable
	P_BLOCKED,			// This process is blocked
	P_ZOMBIE			// This process has exited, but no one
					// has called sys_wait() yet
} procstate_t;

// Process descriptor type
typedef struct process {
    uint32_t waitingQ[NPROCS]; // parent's Process ID
	pid_t p_pid;			// Process ID

	registers_t p_registers;	// Current process state: registers,
					// stack location, EIP, etc.
					// 'registers_t' defined in x86.h
	procstate_t p_state;		// Process state; see above
	int p_exit_status;		// Process's exit status (if it has
					// exited and p_state == P_ZOMBIE)
} process_t;


// Top of the kernel stack
#define KERNEL_STACK_TOP	0x80000

// Functions defined in kernel.c
void interrupt(registers_t *reg);
void schedule(void);

// Functions defined in x86.c
void segments_init();
void special_registers_init(process_t *proc);
void console_clear(void);
int console_read_digit(void);
// Function defined in k-loader.c
void program_loader(int programnumber, uint32_t *entry_point);

extern process_t *current;
void run(process_t *proc) __attribute__((noreturn));

#endif
