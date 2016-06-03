#include "Server.hpp"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

const int PORT = 5051;
static Server svr(PORT);

static void sigintHandler(int sign_no)
{
    printf("GoodBye!\n");
    exit(0);
}

int main()
{
    signal(SIGINT, sigintHandler);

    svr.run();
    return 0;
}