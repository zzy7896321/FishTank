#include "host.h"

#include <iostream>

int main(){

    Host& host = RetrieveHost();
    host.FirstSetup();

    std::cout << host.getPlayerCount() << std::endl;

    if (!host.Start()) return -1;

    return 0;
}
