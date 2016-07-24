#ifndef WEENSYOS_PROCESS_H
#define WEENSYOS_PROCESS_H
#include "const.h"
#include "lib.h"

/*****************************************************************************
 * process.h
 *
 *   This header file defines the C versions of the 5 system calls.
 *   Each system call is defined by assembly code that implements a protected
 *   control transfer to the kernel, using the 'int' machine instruction.
 *   Any arguments to the system call are passed in registers, which have
 *   names like %eax and %ebx.  You'll see how below.
 *   The kernel returns any results in a register, %eax.
 *   You'll also see how we tell the compiler to put the value of %eax
 *   into a C variable.
 *
 *****************************************************************************/


/*****************************************************************************
 * sys_getpid
 *
 *   Returns the current process's process ID.
 *
 *****************************************************************************/

static inline pid_t
sys_getpid(void)
{
	// We call a system call using the 'int' instruction.  This causes a
	// software interrupt, which is sometimes called a "trap".
	// In procos, the type of system call is indicated by the interrupt
	// number -- here, INT_SYS_GETPID.
	// The sys_getpid() call returns a value in the %eax register.

	// The C compiler lets us execute arbitrary assembly instructions
	// with an "asm" statement, like the one below.
	// The arguments to the "asm" statement define which instruction to
	// run, and how to put values from registers into C variables.
	// Here, '"=a" (pid)' tells the C compiler that, after executing the
	// 'int' instruction, it should store the value of the %eax register
	// into the 'pid' variable.
	// That means that after the "asm" instruction (which causes the
	// interrupt), the system call's return value is in the 'pid'
	// variable, and we can just return that value!

	pid_t pid;
	asm volatile("int %1\n"
		     : "=a" (pid)
		     : "i" (INT_SYS_GETPID)
		     : "cc", "memory");
	return pid;
}


/*****************************************************************************
 * sys_fork
 *
 *   Create a new process by copying the current process's state.
 *   (In MiniprocOS, this is just the current process's stack; in a real OS
 *   there would be much more.)
 *   In the child's context, this system call returns 0.
 *   In the parent's context, it returns the child's process ID.
 *   Returns -1 if the system call failed.
 *
 *****************************************************************************/

static inline pid_t
sys_fork(void)
{
	// This system call follows the same pattern as sys_getpid().

	pid_t result;
	asm volatile("int %1\n"
		     : "=a" (result)
		     : "i" (INT_SYS_FORK)
		     : "cc", "memory");
	return result;
}

static inline pid_t sys_newthread( void (*start_function)(void) )
{
	// This system call follows the same pattern as sys_getpid().

	pid_t result;
	asm volatile("int %1\n"
		     : "=a" (result)
             : "i" (INT_SYS_THREAD),
               "a"(start_function)
		     : "cc", "memory");
	return result;
}

static inline int sys_kill(pid_t pid)
{
    int result;
	asm volatile("int %1\n"
		     : "=a" (result)
             : "i" (INT_SYS_KILL),
               "a"(pid)
		     : "cc", "memory");
    return result;
}


/*****************************************************************************
 * sys_yield
 *
 *   Yield control of the CPU to the kernel, which will pick another
 *   process to run.  (It might run this process again, depending on the
 *   scheduling policy.)
 *
 *****************************************************************************/

static inline void
sys_yield(void)
{
	// This system call has no return values, so there's no '=a' clause.
	asm volatile("int %0\n"
		     :
		     : "i" (INT_SYS_YIELD)
		     : "cc", "memory");
}


/*****************************************************************************
 * sys_exit(status)
 *
 *   Exit the current process with exit status 'status'.
 *   This system call should never return.  (But bugs in your kernel might
 *   cause it to appear to return.)
 *
 *****************************************************************************/

static inline void
sys_exit(int status)
{
	// This system call uses another feature: the application passes a
	// PARAMETER to the kernel, namely the "status" variable.
	// It passes that parameter by storing it in the %eax register
	// before causing the interrupt.
	// Then the kernel can look up the parameter by checking the value
	// of that register.

	// The '"a" (status)' clause, below, tells the C compiler
	// to load the value of "status" into %eax before executing
	// the 'int' instruction.
	// You can load other registers with similar syntax; specifically:
	//	"a" = %eax, "b" = %ebx, "c" = %ecx, "d" = %edx,
	//	"S" = %esi, "D" = %edi.

	asm volatile("int %0\n"
		     :
		     : "i" (INT_SYS_EXIT),
		       "a" (status)
		     : "cc", "memory");
}


/*****************************************************************************
 * sys_wait(pid)
 *
 *   Wait until the process with ID 'pid' exits, then return that
 *   process's exit status.
 *   sys_wait(pid) will only return successfully *once* for a given process
 *   'pid'.  If two processes call sys_wait(pid) for the same pid, only one
 *   of them will return the actual exit status.
 *   After that point the process ID might be reused.
 *
 *   Returns -1 if 'pid' does not exist, or equals the current process's ID.
 *   Returns WAIT_TRYAGAIN == -2 if 'pid' has not yet exited.
 *
 *****************************************************************************/

static inline int
sys_wait(pid_t pid)
{
	int retval;
	asm volatile("int %1\n"
		     : "=a" (retval)
		     : "i" (INT_SYS_WAIT),
		       "a" (pid)
		     : "cc", "memory");
	return retval;
}



/*****************************************************************************
 * app_printf(format, ...)
 *
 *   Calls console_printf() (see lib.h).
 *   The cursor position is read from 'cursorpos', a shared variable defined
 *   by the kernel, and written back into that variable.
 *   The initial color is based on the current process ID.
 *
 *****************************************************************************/

void app_printf(const char *format, ...) __attribute__((noinline));

void
app_printf(const char *format, ...)
{
	// set default color based on currently running process
	int color = sys_getpid();
	if (color < 0)
		color = 0x0700;
	else {
		static const uint8_t col[] = { 0x0E, 0x0F, 0x0C, 0x0A, 0x09 };
		color = col[color % sizeof(col)] << 8;
	}

	va_list val;
	va_start(val, format);
	cursorpos = console_vprintf(cursorpos, color, format, val);
	va_end(val);
}

#endif
