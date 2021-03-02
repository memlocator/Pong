//Server
#include "Server.h"

void initiateConsole()
{
    AllocConsole();
    FILE* newstdin = nullptr;
    FILE* newstdout = nullptr;
    FILE* newstderr = nullptr;

    freopen_s(&newstdin, "CONIN$", "r", stdin);
    freopen_s(&newstdout, "CONOUT$", "w", stdout);
    freopen_s(&newstderr, "CONOUT$", "w", stderr);
    std::cout << "Server has been initiated." << std::endl;
}

int main()
{
    initiateConsole();

    Server server;
    server.run();

    return 0;
}