#include "Server.hpp"

int main()
{
    const int PORT = 5051;
    Server svr(PORT);
    svr.run();
    return 0;
}