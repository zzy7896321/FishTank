#ifndef _SYNCOBJ_H_
#define _SYNCOBJ_H_

#include "wx_commonheader.h"
#include "notificationevents.h"

inline wxMutex& RetrieveHostDataMutex(){
    static wxMutex mutex;
    return mutex;
}

inline wxCondition& RetrieveResumeCondition(){
    static wxCondition condition(RetrieveHostDataMutex());
    return condition;
}

class MutexTryLocker{
public:
    MutexTryLocker(wxMutex& m);
    ~MutexTryLocker();
private:
    wxMutex& mutex;
    bool bLocked;

    MutexTryLocker(const MutexTryLocker&);
    MutexTryLocker& operator=(const MutexTryLocker&);
};

void SendPauseSignal(int);

bool ForceEnd(bool enable = false);

#endif // _SYNCOBJ_H_
