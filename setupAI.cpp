#include "host.h"
#include "fish.h"
#include "TAAI.h"

void Host::SetupAI(){
    if (iHostStatus == INITIALIZED){
        for (int i = 0; i!=MAX_PLAYER; ++i){
            new TAAI();
            setIdentifier(wxT("TAAI"));
        }

        iHostStatus = READY;

        elhLog.AISetup(time(0), iPlayerCount);
    }
}
