#include "guilogger.h"
#include "fishtankapp.h"
#include "notificationevents.h"
#include <ctime>
#include "host.h"
#include "syncobj.h"

#define SEND_TEXT_PROMPT_EVENT(evtHandler, text ) \
    { \
        ftTextPromptEvent event(text); \
        evtHandler->AddPendingEvent(event); \
    }

static void GetTimeString(wxString& strToPrint, time_t tmTime){
    tm* t = localtime(&tmTime);
    strToPrint.Printf(wxT("[%d/%d/%d %d:%d:%d]"), t->tm_year+1900, t->tm_mon, t->tm_mday,
                      t->tm_hour, t->tm_min, t->tm_sec);
}

GuiLogger::GuiLogger():
    EventLogger("GuiLogger"), mainFrame(wxGetApp().GetMainFrame()){}

GuiLogger::~GuiLogger(){}

void GuiLogger::HostInitialized(time_t tmTime){
    wxString strToPrint;
    GetTimeString(strToPrint, tmTime);
    strToPrint.append(wxT(" Host is initialized."));
    SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint)
}

void GuiLogger::AISetup(time_t tmTime, int iCount){
    wxString strToPrint;
    GetTimeString(strToPrint, tmTime);
    wxString strToPrint2;
    strToPrint2.Printf(wxT(" %d AI(s) are set up."), iCount);
    SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint + strToPrint2)
}

void GuiLogger::GameStarted(time_t tmTime){
    wxString strToPrint;
    GetTimeString(strToPrint, tmTime);
    strToPrint.append(wxT(" Game is started."));
    SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint)
}

void GuiLogger::GameEnded(time_t tmTime, const int* iId){
    wxString strToPrint;
    GetTimeString(strToPrint, tmTime);
    strToPrint.append(wxT(" Game is Ended."));
    SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint)

    ftEventWithIntArray event(ftEVT_GAME_ENDED);
    event.arr1 = iId;
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::HostDestroyed(time_t tmTime){
    if (!IfMainFrameAvailable()) return;
    wxString strToPrint;
    GetTimeString(strToPrint, tmTime);
    strToPrint.append(wxT(" Host is destroyed."));
    SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint)
}

void GuiLogger::FishBorn(int iId){
    ftEventWithID event(iId, host.getX(iId), host.getY(iId), 0, 0, ftEVT_FISH_BORN);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FoodRefreshed(const int* ipPosX, const int* ipPosY){
    ftEventWithIntArray event(ftEVT_FOOD_REFRESHED);
    event.arr1 = ipPosX;
    event.arr2 = ipPosY;
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishInitializing(int iId){}

void GuiLogger::RoundStarted(int iRoundCount){
    wxString strToPrint;
    strToPrint.Printf(wxT("Round %d"), iRoundCount);
    ftTextPromptEvent event(strToPrint, ftEVT_ROUND_STARTED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishRevived(int iId){
    ftEventWithID event(iId, host.getX(iId), host.getY(iId), host.getHP(iId), host.getMaxHP(iId), ftEVT_FISH_REVIVED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::SequenceDecided(const int* iId){
    ftEventWithIntArray event(ftEVT_SEQUENCE_DECIDED);
    event.arr1 = iId;
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishInAction(int iId){
    ftEventWithID event(iId, host.getX(iId), host.getY(iId), 0, 0, fTEVT_FISH_IN_ACTION);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishMove(int iId, int iFormerPosX, int iFormerPosY, int iTargetPosX, int iTargetPosY, int iResult){
    static wxMutex& mutex = RetrieveHostDataMutex();
    static wxCondition& condition = RetrieveResumeCondition();
    if (iResult){
        wxString strToPrint;
        strToPrint.Printf(wxT("Fish %d attempts to move from (%d, %d) to (%d, %d). %s"), iId,
                        iFormerPosX, iFormerPosY, iTargetPosX, iTargetPosY, strResultString[iResult].c_str());
        SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint)
    } else {
        ftEventWithID event(iId, iFormerPosX, iFormerPosY, iTargetPosX, iTargetPosY, ftEVT_FISH_MOVE);
        mutex.Lock();
        mainFrame->AddPendingEvent(event);
        condition.Wait();
        mutex.Unlock();
    }
}

void GuiLogger::FishAttack(int iId, int iTargetPosX, int iTargetPosY, int iTarget, int iResult){
    if (iResult) {
        wxString strToPrint;
        switch (iTarget){
        case FOOD:
            strToPrint.Printf(wxT("Fish %d attempts to attack food at (%d, %d). %s"), iId,
                          iTargetPosX, iTargetPosY, strResultString[iResult].c_str());
            break;
        case EMPTY:
            strToPrint.Printf(wxT("Fish %d attempts to attack null at (%d, %d). %s"), iId,
                            iTargetPosX, iTargetPosY, strResultString[iResult].c_str());
            break;
        default:
            strToPrint.Printf(wxT("Fish %d attempts to attack fish %d at (%d, %d). %s"), iId,
                          iTargetPosX, iTargetPosY, iTarget, strResultString[iResult].c_str());
            break;
        }
        SEND_TEXT_PROMPT_EVENT(mainFrame, strToPrint)
    } else {
        ftEventWithID event(iId, iTargetPosX, iTargetPosY, iTarget, host.getKillCount(iId), ftEVT_FISH_ATTACK);
        mainFrame->AddPendingEvent(event);
    }
}

void GuiLogger::FishDead(int iId, int iPosX, int iPosY){
    ftEventWithID event(iId, iPosX, iPosY, host.getDeadCount(iId), 0, ftEVT_FISH_DEAD);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishExpIncreased(int iId){
    ftEventWithID event(iId, host.getExp(iId), host.getKillBonus(iId), host.getPoint(iId), 0, ftEVT_FISH_EXP_INCREASED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishLevelUp(int iId){
    ftEventWithID event(iId, host.getLevel(iId), 0, 0, 0, ftEVT_FISH_LEVEL_UP);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishHPModified(int iId){
    ftEventWithID event(iId, host.getHP(iId), host.getMaxHP(iId), host.getX(iId), host.getY(iId), ftEVT_FISH_HP_MODIFIED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishActionFinished(int iId){
    ftEventWithID event(iId, host.getX(iId), host.getY(iId), 0, 0, ftEVT_FISH_ACTION_FINISHED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishTimeout(int iId, int iTimeUsage){}

void GuiLogger::FishHealthIncreased(int iId){
    ftEventWithID event(iId, host.getMaxHP(iId), host.getPropertyPoint(iId), 0, 0, ftEVT_FISH_HEALTH_INCREASED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishSpeedIncreased(int iId){
    ftEventWithID event(iId, host.getSp(iId), host.getPropertyPoint(iId), 0, 0, ftEVT_FISH_SPEED_INCREASED);
    mainFrame->AddPendingEvent(event);
}

void GuiLogger::FishStrengthIncreased(int iId){
    ftEventWithID event(iId, host.getAtt(iId), host.getPropertyPoint(iId), 0, 0, ftEVT_FISH_STRENGTH_INCREASED);
    mainFrame->AddPendingEvent(event);
}
