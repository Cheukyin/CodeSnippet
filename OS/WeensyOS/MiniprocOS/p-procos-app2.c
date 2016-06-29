#include "process.h"
#include "lib.h"

/*****************************************************************************
 * p-procos-app2
 *
 *   This application as 1024 new processes and then waits for them to
 *   exit.  All processes print messages to the screen.
 *
 *****************************************************************************/

volatile int counter;

void run_child(void);
static void check(int actual_value, int expected_value, const char *type);

void
pmain(void)
{
	volatile int checker = 30; /* This variable checks for some common
				      stack errors. */
	pid_t p;
	int status;

	counter = 0;

	while (counter < 1025) {
		int n_started = 0;
		check(checker, 30, "parent loop");

		// Start as many processes as possible, until we fail to start
		// a process or we have started 1025 processes total.
		while (counter + n_started < 1025) {
			p = sys_fork();
			if (p == 0) {
				check(checker, 30, "child");
				checker = 30 + counter;
				run_child();
			} else if (p > 0)
				n_started++;
			else
				break;
		}

		// If we could not start any new processes, give up!
		if (n_started == 0)
			break;


		// We started at least one process, but then could not start
		// any more.
		// That means we ran out of room to start processes.
		// Retrieve old processes' exit status with sys_wait()
		// to make room for new processes.
		for (p = 2; p < NPROCS; p++)
			(void) sys_wait(p);
	}

	check(checker, 30, "after parent loop");
	sys_exit(0);
}

void run_child(void)
{
	int input_counter = counter;

	counter++;		/* Note that all "processes" share an address
				   space, so this change to 'counter' will be
				   visible to all processes. */

	app_printf("Process %d lives, counter %d!\n",
               sys_getpid(), input_counter);
	sys_exit(input_counter);
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
