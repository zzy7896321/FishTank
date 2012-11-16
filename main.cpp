#include "host.h"

int main(){

    Host& host = RetrieveHost();
    host.FirstSetup();

    if (!host.Start()) return -1;

    return 0;
}
