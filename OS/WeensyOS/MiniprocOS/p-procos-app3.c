#include "process.h"
#include "lib.h"

/*****************************************************************************
 * p-procos-app3
 *
 *   This demonstration application checks for several common errors in
 *   your implementation of miniprocos.
 *
 *****************************************************************************/

static void check_after_exit(const char *where);

void
pmain(void)
{
	pid_t p1, p2, p3, p4;
	int i, status, status2;

	/* Check exit status returns */
	p1 = sys_fork();
	if (p1 == 0) {
		/* Schedule several times to make sure p2 exits first */
		for (i = 0; i < 20; ++i)
			sys_yield();
		sys_exit(345);
		check_after_exit("p1");
	}

	p2 = sys_fork();
	if (p2 == 0) {
		sys_exit(678);
		check_after_exit("p2");
	}

	do {
		status = sys_wait(p1);
	} while (status == WAIT_TRYAGAIN);
	if (status == 345)
		app_printf("1st sys_wait(p1) OK\n");
	else {
		app_printf("1st sys_wait(p1) returns incorrect exit status %d!\n", status);
		if (status == 678)
			app_printf("(Looks like it got p2's exit status.)\n");
		sys_exit(1);
	}

	status = sys_wait(p1);
	if (status < 0 && status != WAIT_TRYAGAIN)
		app_printf("2nd sys_wait(p1) OK\n");
	else {
		app_printf("2nd sys_wait(p1) should return error (returned %d)!\n", status);
		sys_exit(1);
	}

	do {
		status = sys_wait(p2);
	} while (status == WAIT_TRYAGAIN);
	if (status == 678)
		app_printf("1st sys_wait(p2) OK\n");
	else {
		app_printf("1st sys_wait(p2) returns incorrect exit status %d!\n", status);
		sys_exit(1);
	}

	status = sys_wait(p2);
	if (status < 0 && status != WAIT_TRYAGAIN)
		app_printf("2nd sys_wait(p2) OK\n");
	if (status >= 0 || status == WAIT_TRYAGAIN) {
		app_printf("2nd sys_wait(p2) should return error (returned %d)!\n", status);
		sys_exit(1);
	}


	/* Check multiple waiters */
	p3 = sys_fork();
	if (p3 == 0) {
		/* Schedule several times to make sure p2 runs */
		for (i = 0; i < 20; ++i)
			sys_yield();
		sys_exit(122);
		check_after_exit("p3");
	}

	app_printf("Following this should be two lines, '122' and '-1', in either order:\n");
	p4 = sys_fork();
	if (p4 == 0) {
		do {
			status2 = sys_wait(p3);
		} while (status2 == WAIT_TRYAGAIN);
		app_printf("  %d\n", status2);
		sys_exit(0);
		check_after_exit("p4");
	}

	do {
		status = sys_wait(p3);
	} while (status == WAIT_TRYAGAIN);
	app_printf("  %d\n", status);

	/* Schedule several times to make sure p4 runs */
	for (i = 0; i < 20; ++i)
		sys_yield();

	app_printf("procos-app3 checks complete!\n");
        sys_exit(0);
}

static void
check_after_exit(const char *where)
{
	app_printf("%s: sys_exit() returned! It should never return!\n", where);
 loop:	goto loop;
}
