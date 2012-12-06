#ifndef _EVENTLOGGER_H_
#define _EVENTLOGGER_H_

#include <ctime>
#include <vector>
#include <string>
#include "wx_commonheader.h"

extern const wxString strResultString[7]/* = {"Success.",     //0
                            "Failure: invalid target coordinate.",      //1
                            "Failure: target is already occupied.",     //2
                            "Failure: target is out of range.",     //3
                            "Failure: target is empty.",        //4
                            "Failure: operation is allowed once each round."    //5
                            "Failure: it's not good to commit suicide."     //6
                            }*/;

class Host;

class EventLogger{
public:
    EventLogger(const std::string& strId);
    virtual ~EventLogger(){}

    virtual void HostInitialized(time_t tmTime) = 0;
    virtual void AISetup(time_t tmTime, int iCount) = 0;
    virtual void GameStarted(time_t tmTime) = 0;
    virtual void GameEnded(time_t tmTime, const int* iId) = 0;
    virtual void HostDestroyed(time_t tmTime) = 0;

    virtual void FishBorn(int iId) = 0;
    virtual void FoodRefreshed(const int* ipPosX, const int* ipPosY) = 0;
    virtual void FishInitializing(int iId) = 0;
    virtual void RoundStarted(int iRoundNumber) = 0;
    virtual void FishRevived(int iId) = 0;
    virtual void SequenceDecided(const int* iId) = 0;
    virtual void FishInAction(int iId) = 0;
    virtual void FishMove(int iId, int iFormerPosX, int iFormerPosY, int iTargetPosX, int iTargetPosY, int iResult) = 0;
    virtual void FishAttack(int iId, int iTargetPosX, int iTargetPosY, int iTarget, int iResult) = 0;
    virtual void FishDead(int iId, int iPosX, int iPosY) = 0;
    virtual void FishExpIncreased(int iId) = 0;
    virtual void FishLevelUp(int iId) = 0;
    virtual void FishHPModified(int iId) = 0;
    virtual void FishActionFinished(int iId) = 0;
    virtual void FishTimeout(int iId, int iTimeUsage) = 0;

    virtual void FishHealthIncreased(int iId) = 0;
    virtual void FishSpeedIncreased(int iId) = 0;
    virtual void FishStrengthIncreased(int iId) = 0;

    const std::string strIdentifier;
protected:
    Host& host;
};

class EventLoggerHub{
public:
    EventLoggerHub();
    void AddLogger(EventLogger* elLogger);
    unsigned GetSize() const;
    const std::string GetIdentifier(unsigned iIndex) const;
    void DeleteLogger(unsigned iIndex);

    void HostInitialized(time_t tmTime);
    void AISetup(time_t tmTime, int iCount);
    void GameStarted(time_t tmTime);
    void GameEnded(time_t tmTime, const int* iId);
    void HostDestroyed(time_t tmTime);

    void FishBorn(int iId);
    void FoodRefreshed(const int* ipPosX, const int* ipPosY);
    void FishInitializing(int iId);
    void RoundStarted(int iRoundNumber);
    void FishRevived(int iId);
    void SequenceDecided(const int* iId);
    void FishInAction(int iId);
    void FishMove(int iId, int iFormerPosX, int iFormerPosY, int iTargetPosX, int iTargetPosY, int iResult);
    void FishAttack(int iId, int iTargetPosX, int iTargetPosY, int iTarget, int iResult);
    void FishDead(int iId, int iPosX, int iPosY);
    void FishExpIncreased(int iId);
    void FishLevelUp(int iId);
    void FishHPModified(int iId);
    void FishActionFinished(int iId);
    void FishTimeout(int iId, int iTimeUsage);

    void FishHealthIncreased(int iId);
    void FishSpeedIncreased(int iId);
    void FishStrengthIncreased(int iId);

private:
    std::vector<EventLogger*> elList;
};

#endif // _EVENTLOGGER_H_
