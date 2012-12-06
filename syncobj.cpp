#include "syncobj.h"
#include "fishtankapp.h"

MutexTryLocker::MutexTryLocker(wxMutex& m):mutex(m), bLocked(false){
    if (mutex.TryLock() == wxMUTEX_NO_ERROR) bLocked = true;
}

MutexTryLocker::~MutexTryLocker(){
    if (bLocked) mutex.Unlock();
}

bool ForceEnd(bool enable){
    static bool bIfForceEnd = false;
    static wxMutex mutex;
    mutex.Lock();
        bool b = bIfForceEnd;
        bIfForceEnd = enable;
    mutex.Unlock();
    return b;
}

void SendPauseSignal(int pt){
    static MainFrame* mainFrame = wxGetApp().GetMainFrame();
    ftPauseSignalEvent event(pt);
    mainFrame->AddPendingEvent(event);
}
