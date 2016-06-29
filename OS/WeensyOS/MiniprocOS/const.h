#ifndef WEENSYOS_CONST_H
#define WEENSYOS_CONST_H
#include "types.h"

// System call numbers.
// An application calls a system call by causing the specified interrupt.

#define INT_SYS_GETPID		48
#define INT_SYS_FORK		49
#define INT_SYS_YIELD		50
#define INT_SYS_EXIT		51
#define INT_SYS_WAIT		52
#define INT_SYS_THREAD		53
#define INT_SYS_KILL		54

// These system call numbers currently do nothing; feel free to define them
// as you like.

/* #define INT_SYS_USER1		54 */
/* #define INT_SYS_USER2		55 */
/* #define INT_SYS_USER3		56 */
/* #define INT_SYS_USER4		56 */
/* #define INT_SYS_USER5		57 */


// The maximum number of processes in the system.

#define NPROCS			16


// Value returned by sys_wait() to indicate that the caller should try again.

#define WAIT_TRYAGAIN		(-2)


// The current screen cursor position (stored at memory location 0x190000).

extern uint16_t *cursorpos;

#endif
