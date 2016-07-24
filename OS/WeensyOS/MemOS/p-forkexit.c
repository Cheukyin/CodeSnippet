#include "process.h"
#include "lib.h"
#define ALLOC_SLOWDOWN 100

extern uint8_t end[];

// These global variables go on the data page.
uint8_t* heap_top;
uint8_t* stack_bottom;

void process_main(void) {
    while (1) {
        if (rand() % ALLOC_SLOWDOWN == 0) {
            if (sys_fork() == 0)
                break;
        } else
            sys_yield();
    }

    pid_t p = sys_getpid();
    srand(p);

    // The heap starts on the page right after the 'end' symbol,
    // whose address is the first address not allocated to process code
    // or data.
    heap_top = ROUNDUP((uint8_t*) end, PAGESIZE);

    // The bottom of the stack is the first address on the current
    // stack page (this process never needs more than one stack page).
    stack_bottom = ROUNDDOWN((uint8_t*) read_esp() - 1, PAGESIZE);

    // Allocate heap pages until (1) hit the stack (out of address space)
    // or (2) allocation fails (out of physical memory).
    while (1) {
        int x = rand() % (8 * ALLOC_SLOWDOWN);
        if (x < 8 * p) {
            if (heap_top == stack_bottom || sys_page_alloc(heap_top) < 0)
                break;
            *heap_top = p;      /* check we have write access to new page */
            heap_top += PAGESIZE;
            if (console[CPOS(24, 0)]) /* clear "Out of physical memory" msg */
                console_printf(CPOS(24, 0), 0, "\n");
        } else if (x == 8 * p) {
            if (sys_fork() == 0)
                p = sys_getpid();
        } else if (x == 8 * p + 1)
            sys_exit();
        else
            sys_yield();
    }

    // After running out of memory
    while (1) {
        if (rand() % (2 * ALLOC_SLOWDOWN) == 0)
            sys_exit();
        else
            sys_yield();
    }
}
