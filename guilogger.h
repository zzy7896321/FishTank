#ifndef _GUILOGGER_H_
#define _GUILOGGER_H_

#include "eventlogger.h"
#include "wx_commonheader.h"
#include "MainFrame.h"

class GuiLogger:public EventLogger{
public:
    GuiLogger();
    ~GuiLogger();

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
    MainFrame* mainFrame;
};

#endif // _GUILOGGER_H_
