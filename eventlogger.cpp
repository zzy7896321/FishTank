#include "eventlogger.h"
#include "host.h"

const char* pstrResultString[7] = {"Success.",     //0
                            "Failure: invalid target coordinate.",      //1
                            "Failure: target is already occupied.",     //2
                            "Failure: target is out of range.",     //3
                            "Failure: target is empty.",        //4
                            "Failure: operation is allowed once each round."    //5
                            "Failure: it's not good to commit suicide."     //6
                           };

EventLogger::EventLogger(const std::string& strId):strIdentifier(strId), host(RetrieveHost()){}

EventLoggerHub::EventLoggerHub():elList(){}

void EventLoggerHub::AddLogger(EventLogger* elLogger){
    elList.push_back(elLogger);
}

unsigned EventLoggerHub::GetSize() const{
    return elList.size();
}

const std::string EventLoggerHub::GetIdentifier(unsigned iIndex) const{
    return (iIndex>=0 && iIndex<elList.size()) ? elList[iIndex]->strIdentifier : "INVALID_INDEX";
}

void EventLoggerHub::DeleteLogger(unsigned iIndex){
    if (iIndex>=0 && iIndex<elList.size()){
        EventLogger* elToDelete = elList[iIndex];
        elList.erase(elList.begin() + iIndex);
        delete elToDelete;
    }
}

void EventLoggerHub::HostInitialized(time_t tmTime){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->HostInitialized(tmTime);
}

void EventLoggerHub::AISetup(time_t tmTime, int iCount){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->AISetup(tmTime, iCount);
}

void EventLoggerHub::GameStarted(time_t tmTime){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->GameStarted(tmTime);
}

void EventLoggerHub::GameEnded(time_t tmTime, const int* iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->GameEnded(tmTime, iId);
}

void EventLoggerHub::HostDestroyed(time_t tmTime){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->HostDestroyed(tmTime);
}

void EventLoggerHub::FishBorn(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishBorn(iId);
}

void EventLoggerHub::FoodRefreshed(const int* ipPosX, const int* ipPosY){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FoodRefreshed(ipPosX, ipPosY);
}

void EventLoggerHub::FishInitializing(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishInitializing(iId);
}

void EventLoggerHub::RoundStarted(int iRoundNumber){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->RoundStarted(iRoundNumber);
}

void EventLoggerHub::FishRevived(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishRevived(iId);
}

void EventLoggerHub::SequenceDecided(const int* iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->SequenceDecided(iId);
}

void EventLoggerHub::FishInAction(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishInAction(iId);
}

void EventLoggerHub::FishMove(int iId, int iFormerPosX, int iFormerPosY, int iTargetPosX, int iTargetPosY, int iResult){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishMove(iId, iFormerPosX, iFormerPosY, iTargetPosX, iTargetPosY, iResult);
}

void EventLoggerHub::FishAttack(int iId, int iTargetPosX, int iTargetPosY, int iTarget, int iResult){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishAttack(iId, iTargetPosX, iTargetPosY, iTarget, iResult);
}

void EventLoggerHub::FishDead(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishDead(iId);
}

void EventLoggerHub::FishExpIncreased(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishExpIncreased(iId);
}

void EventLoggerHub::FishLevelUp(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishLevelUp(iId);
}

void EventLoggerHub::FishHPModified(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishHPModified(iId);
}

void EventLoggerHub::FishActionFinished(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishActionFinished(iId);
}

void EventLoggerHub::FishTimeout(int iId, int iTimeUsage){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishTimeout(iId, iTimeUsage);
}

void EventLoggerHub::FishHealthIncreased(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishHealthIncreased(iId);
}

void EventLoggerHub::FishSpeedIncreased(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishSpeedIncreased(iId);
}

void EventLoggerHub::FishStrengthIncreased(int iId){
   for (unsigned i=0; i<elList.size(); ++i)
       elList[i]->FishStrengthIncreased(iId);
}
