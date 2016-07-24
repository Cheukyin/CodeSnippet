#include "process.h"
#include "lib.h"

/*****************************************************************************
 * p-procos-app
 *
 *   This application simply starts a child process and then waits for it
 *   to exit.  Both processes print messages to the screen.
 *
 *****************************************************************************/

static void check(int actual_value, int expected_value, const char *type);
static void myThread(void);

void
pmain(void)
{
	volatile int checker = 30; /* This variable checks for some common
				      stack errors. */
	pid_t p;
	int i, status;

	app_printf("About to start a new process...\n");


    sys_newthread(myThread);
    sys_newthread(myThread);
    sys_newthread(myThread);

    app_printf("new thread started\n");


	p = sys_fork();
	if (p == 0) {
		// Check that the kernel correctly copied the parent's stack.
		check(checker, 30, "new child");
		checker = 30 + sys_getpid();

		// Yield several times to help test Exercise 3
		app_printf("Child process %d!\n", sys_getpid());
		for (i = 0; i < 20; i++)
			sys_yield();

		// Check that no one else corrupted our stack.
		check(checker, 30 + sys_getpid(), "end child");
		sys_exit(1000);

	} else if (p > 0) {
		// Check that the child didn't corrupt our stack.
		check(checker, 30, "main parent");

		app_printf("Main process %d!\n", sys_getpid());
		do {
			status = sys_wait(p);
            app_printf("W");
		} while (status == WAIT_TRYAGAIN);
		app_printf("Child %d exited with status %d!\n", p, status);
		check(status, 1000, "sys_wait for child");

		// Check again that the child didn't corrupt our stack.
		check(checker, 30, "end parent");
		sys_exit(0);

	} else {
		app_printf("Error!\n");
		sys_exit(1);
	}
}


static void myThread(void)
{
    app_printf("start myThread\n");
    sys_exit(0);
}


/*****************************************************************************
 * check(actual_value, expected_value, type)
 *
 *   Checks that actual_value == expected_value.  If not, prints an error
 *   message and exits.
 *
 *****************************************************************************/

static void
check(int actual_value, int expected_value, const char *type)
{
	if (actual_value != expected_value) {
		app_printf("Problem at %s (got %d, expected %d)!\n",
			   type, actual_value, expected_value);
		sys_exit(1);
	}
}
