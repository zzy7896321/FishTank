#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

#include "wx_commonheader.h"
#include "notificationevents.h"
#include "constants.h"

class Host;

class TankCanvas;

class MainFrame:public wxFrame{
public:
    MainFrame();
    virtual ~MainFrame();
    void OnStartButtonClicked(wxCommandEvent& event);
    void OnPauseButtonClicked(wxCommandEvent& event);
    void OnStopButtonClicked(wxCommandEvent& event);
    void OnPauseSignal(ftPauseSignalEvent& event);
    void OnGameEnded(ftEventWithIntArray& event);
    void OnSequenceDecided(ftEventWithIntArray& event);
    void OnFoodRefreshed(ftEventWithIntArray& event);
    void OnFishBorn(ftEventWithID& event);
    void OnFishRevived(ftEventWithID& event);
    void OnFishInAction(ftEventWithID& event);
    void OnFishDead(ftEventWithID& event);
    void OnFishMove(ftEventWithID& event);
    void OnFishAttack(ftEventWithID& event);
    void OnFishExpIncreased(ftEventWithID& event);
    void OnFishLevelUp(ftEventWithID& event);
    void OnFishHPModified(ftEventWithID& event);
    void OnFishActionFinished(ftEventWithID& event);
    void OnFishHealthIncreased(ftEventWithID& event);
    void OnFishSpeedIncreased(ftEventWithID& event);
    void OnFishStrengthIncreased(ftEventWithID& event);
    void OnFishMoveAnimationFinished(ftEventWithID& event);
    void OnRoundStarted(ftTextPromptEvent& event);

    void OnPromptText(ftTextPromptEvent& event);
    void PromptText(const wxString& str);
    void SetLeftStatusBar(const wxString& str);
    void SetMiddleStatusBar(const wxString& str);
    void SetRightStatusBar(const wxString& str);
    const wxString& GetAuthor(int id) const;

    void OnDataChoice(wxCommandEvent& event);

    void test_key(wxKeyEvent& event);

private:
    TankCanvas* tankCanvas;
    wxListCtrl* DataList;
    wxChoice* DataChoice;
    wxTextCtrl* LogList;
    wxToolBar* ToolBar;
    wxMenuBar* MenuBar;
    wxMenu* MenuControl;
    wxMenu* MenuPauseWhen;

    const int iListWidth;

    Host* host;
    wxMutex& mutex;
    wxCondition& condition;

    enum{
        DL_ID = 0, DL_AUTHOR, DL_STATUS, DL_POINT, DL_POS, DL_HP, DL_MAXHP,
        DL_ATTACK, DL_SPEED, DL_EXP, DL_BONUS, DL_LEVEL, DL_KILL, DL_DEATH, DL_PROPPOINT
    };

    int ipIDTable[MAX_PLAYER+1];
    wxString strAuthor[MAX_PLAYER+1];
    int iPlayerCount;
    int ipActionSequence[MAX_PLAYER+1];
    bool bStatus[MAX_PLAYER+1]; //true indicates id's still alive
    int iInAction, iInActionSeq; //iInAction is the id of the fish, iInActionSeq is the index
   // int iTargetX, iTargetY, iTarget, iTargetHP, iTargetMaxHP;
   // int iNewExp, iNewLevel, iNewHP, iNewMaxHP, iNewSpeed, iNewAttack, iNewPropPoint;

//    void ProcessAllPendingNotifications();
    void ValidateFish(int id);

    void StartGame();
    void SortDataList();
    void RetrieveAllDataFromHost(int id);   //This function will not lock the mutex

    enum{
        HOST_STARTED,
        HOST_PAUSED,
        HOST_TO_STOP,
        HOST_STOPED,
    }   iHostStatus;

    void SetDataItem(int id, int col, const wxString& d);
    void SetDataItem(int id, int col, int d);
    void SetDataItem(int id, int col, int d, int d2);

    DECLARE_EVENT_TABLE()
};

inline bool IfMainFrameAvailable(bool opt = true){
    static bool bAvailable = true;
    return (bAvailable) ? (bAvailable = opt, true) : (false);
}

#endif // _MAINFRAME_H_
