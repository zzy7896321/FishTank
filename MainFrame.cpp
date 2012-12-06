#include "MainFrame.h"
#include "tankcanvas.h"
#include "host.h"
#include "syncobj.h"
#include <cstring>
#include <algorithm>

MainFrame::MainFrame():
    wxFrame(NULL, ID_MainFrame, wxT("FishTank"), wxPoint(10, 10), wxDefaultSize,
            wxDEFAULT_FRAME_STYLE & (~wxMAXIMIZE_BOX) & (~wxRESIZE_BORDER)),
    tankCanvas(0), iListWidth(600), host(&(RetrieveHost())),
    mutex(RetrieveHostDataMutex()), condition(RetrieveResumeCondition()),
    iHostStatus(HOST_STOPED)
{
    SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* MainSizer = new wxBoxSizer(wxHORIZONTAL);

    tankCanvas = new TankCanvas(this,ID_TankFrame, wxPoint(0,0), wxSize(600, 620));
    MainSizer->Add(tankCanvas, 0);

    wxBoxSizer* PropertyWindowSizer = new wxBoxSizer(wxVERTICAL);
    MainSizer->Add(PropertyWindowSizer, 0);

    wxBoxSizer* DataChoiceSectionSizer = new wxBoxSizer(wxHORIZONTAL);
    PropertyWindowSizer->Add(DataChoiceSectionSizer, 0);

    DataChoiceSectionSizer->Add(new wxStaticText(this, wxID_ANY, wxT("   Sort by: ")), 0);
    wxArrayString DataChoiceInitializer;
    DataChoiceInitializer.Add(wxT("ID"));
    DataChoiceInitializer.Add(wxT("Score"));
    DataChoiceInitializer.Add(wxT("Action sequence"));
    DataChoice = new wxChoice(this, ID_DataChoice, wxDefaultPosition, wxSize(-1, -1), DataChoiceInitializer);
    DataChoiceSectionSizer->Add(DataChoice, 0);
    DataChoice->Select(1);   //sort by score by default

    DataList = new wxListCtrl(this, ID_DataList, wxDefaultPosition, wxSize(iListWidth, 300),
                              wxLC_REPORT);
    PropertyWindowSizer->Add(DataList, 0);
    {
        wxString s[15] = { wxT("ID"), wxT("Author"), wxT("Status"), wxT("Point"),
                           wxT("Pos"), wxT("HP"), wxT("MaxHP"), wxT("Att"),
                           wxT("Sp"), wxT("Exp"), wxT("Bonus"),
                           wxT("Lv"), wxT("Kill"), wxT("Death"), wxT("PropPoint")
                         };
        int w[15] = {30,50,50,45,55,35,50,35,35,40,50,30,35,50,80};
        for (int i = 0; i<15; ++i){
            DataList->InsertColumn(i, s[i], wxLIST_FORMAT_LEFT, w[i]);
        }
    }

    LogList = new wxTextCtrl(this, ID_LogList, wxT(""), wxDefaultPosition, wxSize(iListWidth, 280),
                             wxTE_MULTILINE | wxTE_READONLY);
    PropertyWindowSizer->Add(LogList, 0);

    MenuBar = new wxMenuBar();
        MenuControl = new wxMenu();
        MenuControl->Append(ID_MenuControl_Start, wxT("&Start\tCtrl-S"));
        MenuControl->Append(ID_MenuControl_Stop, wxT("&End\tCtrl-E"));
        MenuControl->Append(ID_MenuControl_Pause, wxT("&Pause\tCtrl-P"));
        MenuBar->Append(MenuControl, wxT("&Control"));

        MenuPauseWhen = new wxMenu();
        MenuPauseWhen->AppendCheckItem(ID_MenuPauseWhen_RoundStart, wxT("&Round started\tCtrl-R"));
        MenuPauseWhen->AppendCheckItem(ID_MenuPauseWhen_FishInitialized, wxT("Fish &initialized\tCtrl-I"));
        MenuPauseWhen->AppendCheckItem(ID_MenuPauseWhen_FishRevived, wxT("Fish re&vived\tCtrl-V"));
        MenuPauseWhen->AppendCheckItem(ID_MenuPauseWhen_FishActionEnded, wxT("Fish action e&nded\tCtrl-N"));
        MenuPauseWhen->AppendCheckItem(ID_MenuPauseWhen_ActionSequenceDecided, wxT("Action sequence &decided\tCtrl-D"));
        MenuBar->Append(MenuPauseWhen, wxT("&PauseWhen"));
    SetMenuBar(MenuBar);
        MenuControl->Enable(ID_MenuControl_Stop, false);
        MenuControl->Enable(ID_MenuControl_Pause, false);

        MenuPauseWhen->Check(ID_MenuPauseWhen_RoundStart, false);
        MenuPauseWhen->Check(ID_MenuPauseWhen_FishInitialized, false);
        MenuPauseWhen->Check(ID_MenuPauseWhen_FishRevived, false);
        MenuPauseWhen->Check(ID_MenuPauseWhen_FishActionEnded, false);
        MenuPauseWhen->Check(ID_MenuPauseWhen_ActionSequenceDecided, false);

    CreateStatusBar(3, wxFULL_REPAINT_ON_RESIZE);

    ToolBar = CreateToolBar();
    ToolBar->AddTool(ID_StartButton, wxT("Start"), *(new wxBitmap(wxT("img\\start.bmp"), wxBITMAP_TYPE_BMP)));
    ToolBar->AddTool(ID_PauseButton, wxT("Pause"), *(new wxBitmap(wxT("img\\pause.bmp"), wxBITMAP_TYPE_BMP)));
    ToolBar->AddTool(ID_StopButton, wxT("Stop"), *(new wxBitmap(wxT("img\\stop.bmp"), wxBITMAP_TYPE_BMP)));
    ToolBar->Realize();
    ToolBar->EnableTool(ID_StopButton, false);
    ToolBar->EnableTool(ID_PauseButton, false);

    SetSizer(MainSizer);
    MainSizer->Fit(this);
}

MainFrame::~MainFrame(){
    IfMainFrameAvailable(false);
}

void MainFrame::SetLeftStatusBar(const wxString& str){
    SetStatusText(str, 0);
}

void MainFrame::SetMiddleStatusBar(const wxString& str){
    SetStatusText(str, 1);
}

void MainFrame::SetRightStatusBar(const wxString& str){
    SetStatusText(str, 2);
}

void MainFrame::OnStartButtonClicked(wxCommandEvent& event){
    switch (iHostStatus){
    case HOST_STARTED:
        break;
    case HOST_STOPED:
        StartGame();
        if (iHostStatus == HOST_STARTED) {
            ToolBar->EnableTool(ID_PauseButton, true);
            ToolBar->EnableTool(ID_StopButton, true);
            ToolBar->EnableTool(ID_StartButton, false);
            MenuControl->Enable(ID_MenuControl_Pause, true);
            MenuControl->Enable(ID_MenuControl_Stop, true);
            MenuControl->Enable(ID_MenuControl_Start, false);
        }
        break;
    case HOST_PAUSED:
        iHostStatus = HOST_STARTED;
        ToolBar->EnableTool(ID_PauseButton, true);
        ToolBar->EnableTool(ID_StartButton, false);
        MenuControl->Enable(ID_MenuControl_Pause, true);
        MenuControl->Enable(ID_MenuControl_Start, false);
        mutex.Lock();
        condition.Signal();
        mutex.Unlock();
        break;
    case HOST_TO_STOP:
        break;
    }
}

void MainFrame::OnPauseButtonClicked(wxCommandEvent& event){
    switch (iHostStatus){
    case HOST_STARTED:
        iHostStatus = HOST_PAUSED;
        ToolBar->EnableTool(ID_PauseButton, false);
        ToolBar->EnableTool(ID_StartButton, true);
        MenuControl->Enable(ID_MenuControl_Pause, false);
        MenuControl->Enable(ID_MenuControl_Start, true);
        break;
    case HOST_STOPED:
        break;
    case HOST_PAUSED:
        break;
    case HOST_TO_STOP:
        break;
    }
}

void MainFrame::OnStopButtonClicked(wxCommandEvent& event){
    switch (iHostStatus){
    case HOST_STARTED:
        iHostStatus = HOST_TO_STOP;
        ToolBar->EnableTool(ID_StartButton, true);
        ToolBar->EnableTool(ID_PauseButton, false);
        ToolBar->EnableTool(ID_StopButton, false);
        MenuControl->Enable(ID_MenuControl_Start, true);
        MenuControl->Enable(ID_MenuControl_Pause, false);
        MenuControl->Enable(ID_MenuControl_Stop, false);
        break;
    case HOST_STOPED:
        break;
    case HOST_PAUSED:
        iHostStatus = HOST_STOPED;
        ToolBar->EnableTool(ID_StopButton, false);
        MenuControl->Enable(ID_MenuControl_Stop, false);
        ForceEnd(true);
        mutex.Lock();
            condition.Signal();
        mutex.Unlock();
    case HOST_TO_STOP:
        break;
    }
}

class HostThread:public wxThread{
public:
    HostThread():
        wxThread(wxTHREAD_DETACHED){}
    virtual void* Entry(){
        RetrieveHost().Start();
        return 0;
    }
};

void MainFrame::StartGame(){
    static wxMessageDialog* ErrorDialog = new wxMessageDialog(this, wxT("Cannot create host thread. Trying restarting Tankfish may help."),
                              wxT("Error"), wxOK | wxICON_ERROR);
    iPlayerCount = 0;
    tankCanvas->Reset();
    DataList->DeleteAllItems();
    LogList->Clear();
    SetLeftStatusBar(wxT(""));
    SetMiddleStatusBar(wxT(""));
    SetRightStatusBar(wxT(""));
    host->Reset();
    HostThread *hostThread = new HostThread();
    if (hostThread->Create() == wxTHREAD_NO_ERROR){
        iHostStatus = HOST_STARTED;
        hostThread->Run();
    } else {
        iHostStatus = HOST_STOPED;
        ErrorDialog->ShowModal();
    }
}

void MainFrame::OnFishBorn(ftEventWithID& event){
    PromptText(wxString::Format(wxT("Fish %d is born at (%d, %d)."), event.iId, event.x, event.y));
    ipIDTable[event.iId] = iPlayerCount;
    tankCanvas->SetGrid(event.x, event.y, event.iId, 100);
    DataList->InsertItem(iPlayerCount++, wxT(""));

        SetDataItem(event.iId, DL_ID, event.iId);
        SetDataItem(event.iId, DL_STATUS, wxT("ALIVE"));
        SetDataItem(event.iId, DL_POINT, 0);
        SetDataItem(event.iId, DL_EXP, 0);
        SetDataItem(event.iId, DL_BONUS, 0);
        SetDataItem(event.iId, DL_LEVEL, 1);
        SetDataItem(event.iId, DL_KILL, 0);
        SetDataItem(event.iId, DL_DEATH, 0);
        SetDataItem(event.iId, DL_POS, event.x, event.y);
    wxMutexLocker lock(mutex);
        strAuthor[event.iId] = host->getIdentifier(event.iId);
        SetDataItem(event.iId, DL_AUTHOR, strAuthor[event.iId]);
        SetDataItem(event.iId, DL_HP, host->getHP(event.iId));
        SetDataItem(event.iId, DL_MAXHP, host->getMaxHP(event.iId));
        SetDataItem(event.iId, DL_ATTACK, host->getAtt(event.iId));
        SetDataItem(event.iId, DL_SPEED, host->getSp(event.iId));
        SetDataItem(event.iId, DL_PROPPOINT, host->getPropertyPoint(event.iId));
}

void MainFrame::OnGameEnded(ftEventWithIntArray& event){
    for (int i = 0; i<iPlayerCount; ++i){
        ipIDTable[event.arr1[i]] = i;
    }
    for (int i = 0; i<iPlayerCount; ++i){
        int id = event.arr1[i];
        RetrieveAllDataFromHost(id);
    }
}

void MainFrame::OnSequenceDecided(ftEventWithIntArray& event){
    PromptText(wxT("Action sequence is decided."));
    memcpy(ipActionSequence, event.arr1, (iPlayerCount * sizeof(int)));
    iInActionSeq = -1; iInAction = 0;
    wxMutexLocker lock(mutex);
    SetRightStatusBar(wxString::Format(wxT("Next: %d Author: %s"), ipActionSequence[0],
                                    strAuthor[ipActionSequence[0]].c_str()));
    for (int i=0; i<iPlayerCount; ++i){
        bStatus[ipActionSequence[i]] = (host->getHP(ipActionSequence[i]) > 0);
    }

    //Resort the Datalist
    mutex.Lock();
    SortDataList();
    mutex.Unlock();
}

void MainFrame::OnFoodRefreshed(ftEventWithIntArray& event){
    const int *x = event.arr1, *y = event.arr2;
    tankCanvas->EraseGrid(FOOD);
    for (int i = 0; i<MAX_FOOD; ++i)
        tankCanvas->SetGrid(x[i], y[i], FOOD);
}

void MainFrame::OnFishRevived(ftEventWithID& event){
    tankCanvas->SetGrid(event.x, event.y, event.iId, event.x1 * 100 / event.y1);
    PromptText(wxString::Format(wxT("Fish %d is revived at (%d, %d)."), event.iId, event.x, event.y));

    SetDataItem(event.iId, DL_STATUS, wxT("ALIVE"));
    SetDataItem(event.iId, DL_POS, event.x, event.y);
    SetDataItem(event.iId, DL_HP, event.x1);
    SetDataItem(event.iId, DL_MAXHP, event.y1);
    wxMutexLocker lcok(mutex);
    SetDataItem(event.iId, DL_ATTACK, host->getAtt(event.iId));
    SetDataItem(event.iId, DL_SPEED, host->getSp(event.iId));
    SetDataItem(event.iId, DL_PROPPOINT, host->getPropertyPoint(event.iId));
}

void MainFrame::OnFishDead(ftEventWithID& event){
    tankCanvas->EraseGrid(event.x, event.y);
    PromptText(wxString::Format(wxT("Fish %d is dead."), event.iId));
    SetDataItem(event.iId, DL_STATUS, wxT("DEAD"));
    SetDataItem(event.iId, DL_HP, wxT("0"));
    SetDataItem(event.iId, DL_DEATH, event.x1);
    SetDataItem(event.iId, DL_POS, INVALID_VALUE, INVALID_VALUE);
    bStatus[event.iId] = false;
}

void MainFrame::OnFishInAction(ftEventWithID& event){
    tankCanvas->ActivateFish(event.x, event.y);
    PromptText(wxString::Format(wxT("Fish %d is now in action."), event.iId));
    while (ipActionSequence[++iInActionSeq]!=event.iId);
    iInAction = ipActionSequence[iInActionSeq];
    int nextOne = iInActionSeq+1;
    while (nextOne<iPlayerCount && !bStatus[ipActionSequence[nextOne]]) ++nextOne;
    if (nextOne < iPlayerCount)
        SetRightStatusBar(wxString::Format(wxT("In action: %d author = %s, Next: %d author = %s"),
                                    iInAction, strAuthor[iInAction].c_str(),
                                    ipActionSequence[nextOne], strAuthor[ipActionSequence[nextOne]].c_str()));
    else
        SetRightStatusBar(wxString::Format(wxT("In action: %d author = %s"),
                                    iInAction, strAuthor[iInAction].c_str()));
}

void MainFrame::OnFishMove(ftEventWithID& event){
    PromptText(wxString::Format(wxT("Fish %d attempts to move from (%d, %d) to (%d, %d). Success."),
                                event.iId, event.x, event.y, event.x1, event.y1));
    DataList->SetItem(ipIDTable[event.iId], DL_POS, wxString::Format(wxT("(%d, %d)"), event.x1, event.y1));
    tankCanvas->MoveFish(event.x, event.y, event.x1, event.y1);
    //IMPLEMENT
}

void MainFrame::OnFishAttack(ftEventWithID& event){
    PromptText(wxString::Format(wxT("Fish %d attemps to attack %s at (%d, %d). Success."),
                                event.iId,
                                (event.x1 == FOOD) ? wxT("FOOD") : (wxString::Format(wxT("Fish %d"), event.x1).c_str()),
                                event.x, event.y));
    if (event.x1==FOOD) tankCanvas->EraseGrid(event.x, event.y);
    SetDataItem(event.iId, DL_KILL, event.y1);
}

void MainFrame::OnFishExpIncreased(ftEventWithID& event){
    SetDataItem(event.iId, DL_EXP, event.x);
    SetDataItem(event.iId, DL_BONUS, event.y);
    SetDataItem(event.iId, DL_POINT, event.x1);
}

void MainFrame::OnFishLevelUp(ftEventWithID& event){
    SetDataItem(event.iId, DL_LEVEL, event.x);
}

void MainFrame::OnFishHPModified(ftEventWithID& event){
    SetDataItem(event.iId, DL_HP, event.x);
    tankCanvas->SetGrid(event.x1, event.y1, event.iId, event.x * 100 /event.y);
}

void MainFrame::OnFishActionFinished(ftEventWithID& event){
    PromptText(wxT("Action finished"));
    tankCanvas->DeactivateFish(event.x, event.y);
    //validation
        //ValidateFish(event.iId);
}

void MainFrame::ValidateFish(int id){
    wxMutexLocker lock(mutex);

}

void MainFrame::OnFishMoveAnimationFinished(ftEventWithID& event){
    mutex.Lock();
    condition.Signal();
    mutex.Unlock();
}

/*void MainFrame::ProcessAllPendingNotifications(){
    wxLogMessage(wxString::Format(wxT("id = %d, target = %d"), ipActionSequence[iInAction], iTarget));
    if (iTarget!=EMPTY) PromptText(wxString::Format(wxT("Fish %d attemps to attack %s at (%d, %d). Success."),
                                ipActionSequence[iInAction],
                                (iTarget == FOOD)? wxT("FOOD") : (wxString::Format(wxT("fish %d"), iTarget).c_str()),
                                iTargetX, iTargetY));
    if (iTarget == FOOD){
        tankCanvas->SetGrid(iTargetX, iTargetY, 0);
    }
    else if (iTarget != EMPTY) {
        if (iTargetHP == 0){
            tankCanvas->SetGrid(iTargetX, iTargetY, 0);
            DataList->SetItem(ipIDTable[iTarget], DL_STATUS, wxT("DEAD"));
            DataList->SetItem(ipIDTable[iTarget], DL_POS, wxString::Format(wxT("(%d, %d)"), INVALID_VALUE, INVALID_VALUE));
            DataList->SetItem(ipIDTable[iTarget], DL_HP, wxT("0"));
            mutex.Lock();
                DataList->SetItem(ipIDTable[iTarget], DL_DEATH, wxString::Format(wxT("%d"), host->getDeadCount(iTarget)));
            mutex.Unlock();
        } else {
            tankCanvas->SetGrid(iTargetX, iTargetY, iTarget, iTargetHP*100 / iTargetMaxHP);
            DataList->SetItem(ipIDTable[iTarget], DL_HP, wxString::Format(wxT("%d"), iTargetHP));
        }
    }
        int id = ipActionSequence[iInAction];
        if (iNewAttack!=-2) DataList->SetItem(ipIDTable[id], DL_ATTACK, wxString::Format(wxT("%d"), iNewAttack));
        if (iNewExp!=-2) DataList->SetItem(ipIDTable[id], DL_EXP, wxString::Format(wxT("%d"), iNewExp));
        if (iNewHP!=-2) DataList->SetItem(ipIDTable[id], DL_HP, wxString::Format(wxT("%d"), iNewHP));
        if (iNewLevel!=-2) DataList->SetItem(ipIDTable[id], DL_LEVEL, wxString::Format(wxT("%d"), iNewLevel));
        if (iNewMaxHP!=-2) DataList->SetItem(ipIDTable[id], DL_MAXHP, wxString::Format(wxT("%d"), iNewHP));
        if (iNewPropPoint!=-2) DataList->SetItem(ipIDTable[id], DL_PROPPOINT, wxString::Format(wxT("%d"), iNewPropPoint));
        if (iNewSpeed!=-2) DataList->SetItem(ipIDTable[id], DL_SPEED, wxString::Format(wxT("%d"), iNewSpeed));
        mutex.Lock();
            if (iNewHP!=-2) tankCanvas->SetGrid(host->getX(id), host->getY(id), id, iNewHP*100 / iNewMaxHP);
            DataList->SetItem(ipIDTable[id], DL_BONUS, wxString::Format(wxT("%d"), host->getKillBonus(id)));
            DataList->SetItem(ipIDTable[id], DL_KILL, wxString::Format(wxT("%d"), host->getKillCount(id)));
        mutex.Unlock();
}   */
void MainFrame::OnFishHealthIncreased(ftEventWithID& event){
    SetDataItem(event.iId, DL_MAXHP, event.x);
    SetDataItem(event.iId, DL_PROPPOINT, event.y);
    // When health increased, a FT_EVT_FISH_HP_MODIFIED will be posted as well
    // The HP Bar will be updated in function OnFishHPModified
}

void MainFrame::OnFishSpeedIncreased(ftEventWithID& event){
    SetDataItem(event.iId, DL_SPEED, event.x);
    SetDataItem(event.iId, DL_PROPPOINT, event.y);
}

void MainFrame::OnFishStrengthIncreased(ftEventWithID& event){
    SetDataItem(event.iId, DL_ATTACK, event.x);
    SetDataItem(event.iId, DL_PROPPOINT, event.y);
}

void MainFrame::OnPauseSignal(ftPauseSignalEvent& event){
    switch (event.GetPauseType()){
    case PAUSE_ROUND_START:
        if (iHostStatus == HOST_STARTED){
            if (MenuPauseWhen->IsChecked(ID_MenuPauseWhen_RoundStart)){
                iHostStatus = HOST_PAUSED;
                ToolBar->EnableTool(ID_StartButton, true);
                ToolBar->EnableTool(ID_PauseButton, false);
                MenuControl->Enable(ID_MenuControl_Start, true);
                MenuControl->Enable(ID_MenuControl_Pause, false);
            } else {
                mutex.Lock();
                condition.Signal();
                mutex.Unlock();
            }
        }
        else if (iHostStatus == HOST_TO_STOP){
            iHostStatus = HOST_STOPED;
            ForceEnd(true);
            mutex.Lock();
            condition.Signal();
            mutex.Unlock();
        }
        break;
    case PAUSE_INIT:
        if (iHostStatus == HOST_STARTED){
            if (MenuPauseWhen->IsChecked(ID_MenuPauseWhen_FishInitialized)){
                iHostStatus = HOST_PAUSED;
                ToolBar->EnableTool(ID_StartButton, true);
                ToolBar->EnableTool(ID_PauseButton, false);
                MenuControl->Enable(ID_MenuControl_Start, true);
                MenuControl->Enable(ID_MenuControl_Pause, false);
            } else {
                mutex.Lock();
                condition.Signal();
                mutex.Unlock();
            }
        }
        else if (iHostStatus == HOST_TO_STOP){
            iHostStatus = HOST_STOPED;
            ForceEnd(true);
            mutex.Lock();
            condition.Signal();
            mutex.Unlock();
        }
        break;
    case PAUSE_REVIVE:
        if (iHostStatus == HOST_STARTED){
            if (MenuPauseWhen->IsChecked(ID_MenuPauseWhen_FishRevived)){
                iHostStatus = HOST_PAUSED;
                ToolBar->EnableTool(ID_StartButton, true);
                ToolBar->EnableTool(ID_PauseButton, false);
                MenuControl->Enable(ID_MenuControl_Start, true);
                MenuControl->Enable(ID_MenuControl_Pause, false);
            } else {
                mutex.Lock();
                condition.Signal();
                mutex.Unlock();
            }
        }
        else if (iHostStatus == HOST_TO_STOP){
            iHostStatus = HOST_STOPED;
            ForceEnd(true);
            mutex.Lock();
            condition.Signal();
            mutex.Unlock();
        }
        break;
    case PAUSE_FISH_ROUND_FINISHED:
        if (iHostStatus == HOST_STARTED){
            if (MenuPauseWhen->IsChecked(ID_MenuPauseWhen_FishActionEnded)){
                iHostStatus = HOST_PAUSED;
                ToolBar->EnableTool(ID_StartButton, true);
                ToolBar->EnableTool(ID_PauseButton, false);
                MenuControl->Enable(ID_MenuControl_Start, true);
                MenuControl->Enable(ID_MenuControl_Pause, false);
            } else {
                mutex.Lock();
                condition.Signal();
                mutex.Unlock();
            }
        }
        else if (iHostStatus == HOST_TO_STOP){
            iHostStatus = HOST_STOPED;
            ForceEnd(true);
            mutex.Lock();
            condition.Signal();
            mutex.Unlock();
        }
        break;
    case PAUSE_SEQUENCE_DECIDED:
        if (iHostStatus == HOST_STARTED){
            if (MenuPauseWhen->IsChecked(ID_MenuPauseWhen_ActionSequenceDecided)){
                iHostStatus = HOST_PAUSED;
                ToolBar->EnableTool(ID_StartButton, true);
                ToolBar->EnableTool(ID_PauseButton, false);
                MenuControl->Enable(ID_MenuControl_Start, true);
                MenuControl->Enable(ID_MenuControl_Pause, false);
            } else {
                mutex.Lock();
                condition.Signal();
                mutex.Unlock();
            }
        }
        else if (iHostStatus == HOST_TO_STOP){
            iHostStatus = HOST_STOPED;
            ForceEnd(true);
            mutex.Lock();
            condition.Signal();
            mutex.Unlock();
        }
        break;
    }
}

void MainFrame::OnRoundStarted(ftTextPromptEvent& event){
    PromptText(event.GetStr());
    SetMiddleStatusBar(event.GetStr());
}

void MainFrame::OnDataChoice(wxCommandEvent& event){
    if (iHostStatus != HOST_STOPED){
        mutex.Lock();
        SortDataList();
        mutex.Unlock();
    }
}

static inline bool CmpByScore(int x, int y){
    static Host* host = &(RetrieveHost());
    return host->getPoint(x) > host->getPoint(y);
}

void MainFrame::SortDataList(){
    switch (DataChoice->GetCurrentSelection()){
    case 0: //ID
        {
            int IDList[MAX_PLAYER+1];
            memcpy(IDList, ipActionSequence, iPlayerCount * sizeof(int));
            std::sort(IDList, IDList+iPlayerCount);
            for (int i = 0; i<iPlayerCount; ++i){
                ipIDTable[IDList[i]] = i;
                RetrieveAllDataFromHost(IDList[i]);
            }
        }
        break;
    case 1: //Score
        {
            int IDList[MAX_PLAYER+1];
            memcpy(IDList, ipActionSequence, iPlayerCount * sizeof(int));
            std::sort(IDList, IDList+iPlayerCount, CmpByScore);
            for (int i = 0; i<iPlayerCount; ++i){
                ipIDTable[IDList[i]] = i;
                RetrieveAllDataFromHost(IDList[i]);
            }
        }
        break;
    case 2: //ActionSequence
        {
            const int *IDList = ipActionSequence;
            for (int i = 0; i<iPlayerCount; ++i){
                ipIDTable[IDList[i]] = i;
                RetrieveAllDataFromHost(IDList[i]);
            }
        }
        break;
    }
}

void MainFrame::RetrieveAllDataFromHost(int id){
    SetDataItem(id, DL_ID, id);
    SetDataItem(id, DL_AUTHOR, strAuthor[id]);
    SetDataItem(id, DL_STATUS, (host->getHP(id)) ? wxT("ALIVE") : wxT("DEAD"));
    SetDataItem(id, DL_POINT, host->getPoint(id));
    SetDataItem(id, DL_POS, host->getX(id), host->getY(id));
    SetDataItem(id, DL_HP, host->getHP(id));
    SetDataItem(id, DL_MAXHP, host->getMaxHP(id));
    SetDataItem(id, DL_ATTACK, host->getAtt(id));
    SetDataItem(id, DL_SPEED, host->getSp(id));
    SetDataItem(id, DL_EXP, host->getExp(id));
    SetDataItem(id, DL_BONUS, host->getKillBonus(id));
    SetDataItem(id, DL_LEVEL, host->getLevel(id));
    SetDataItem(id, DL_KILL, host->getKillCount(id));
    SetDataItem(id, DL_DEATH, host->getDeadCount(id));
    SetDataItem(id, DL_PROPPOINT, host->getPropertyPoint(id));
}

void MainFrame::OnPromptText(ftTextPromptEvent& event){
    LogList->AppendText(event.GetStr() + wxT("\n"));
}

void MainFrame::PromptText(const wxString& str){
    LogList->AppendText(str + wxT("\n"));
}

void MainFrame::SetDataItem(int id, int col, const wxString& d){
    DataList->SetItem(ipIDTable[id], col, d);
}

void MainFrame::SetDataItem(int id, int col, int d){
    DataList->SetItem(ipIDTable[id], col, wxString::Format(wxT("%d"), d));
}

void MainFrame::SetDataItem(int id, int col, int d, int d2){
    DataList->SetItem(ipIDTable[id], col, wxString::Format(wxT("(%d, %d)"), d, d2));
}

const wxString& MainFrame::GetAuthor(int id) const{
    return strAuthor[id];
}

void MainFrame::test_key(wxKeyEvent& event){
    wxLogMessage(wxT("Invoked in main frame"));
    switch (event.GetKeyCode()){
    case 'C':
        {wxMutexLocker lock(mutex);
        condition.Signal();}
        break;
    case 'A':
        {
            wxLogMessage(DataList->GetItemText(1));
        }
        break;
    }
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CHOICE(ID_DataChoice, MainFrame::OnDataChoice)
    EVT_TOOL(ID_StartButton, MainFrame::OnStartButtonClicked)
    EVT_TOOL(ID_PauseButton, MainFrame::OnPauseButtonClicked)
    EVT_TOOL(ID_StopButton, MainFrame::OnStopButtonClicked)
    EVT_MENU(ID_MenuControl_Start, MainFrame::OnStartButtonClicked)
    EVT_MENU(ID_MenuControl_Pause, MainFrame::OnPauseButtonClicked)
    EVT_MENU(ID_MenuControl_Stop, MainFrame::OnStopButtonClicked)

    FT_EVT_TEXT_PROMPT(MainFrame::OnPromptText)
    FT_EVT_ROUND_STARTED(MainFrame::OnRoundStarted)
    FT_EVT_PAUSE_SIGNAL(MainFrame::OnPauseSignal)
    FT_EVT_GAME_ENDED(MainFrame::OnGameEnded)
    FT_EVT_SEQUENCE_DECIDED(MainFrame::OnSequenceDecided)
    FT_EVT_FOOD_REFRESHED(MainFrame::OnFoodRefreshed)
    FT_EVT_FISH_BORN(MainFrame::OnFishBorn)
    FT_EVT_FISH_REVIVED(MainFrame::OnFishRevived)
    FT_EVT_FISH_IN_ACTION(MainFrame::OnFishInAction)
    FT_EVT_FISH_DEAD(MainFrame::OnFishDead)
    FT_EVT_FISH_MOVE(MainFrame::OnFishMove)
    FT_EVT_FISH_ATTACK(MainFrame::OnFishAttack)
    FT_EVT_FISH_EXP_INCREASED(MainFrame::OnFishExpIncreased)
    FT_EVT_FISH_LEVEL_UP(MainFrame::OnFishLevelUp)
    FT_EVT_FISH_HP_MODIFIED(MainFrame::OnFishHPModified)
    FT_EVT_FISH_ACTION_FINISHED(MainFrame::OnFishActionFinished)
    FT_EVT_FISH_HEALTH_INCREASED(MainFrame::OnFishHealthIncreased)
    FT_EVT_FISH_SPEED_INCREASED(MainFrame::OnFishSpeedIncreased)
    FT_EVT_FISH_STRENGTH_INCREASED(MainFrame::OnFishStrengthIncreased)

    FT_EVT_FISH_MOVE_ANIMATION_FINISHED(MainFrame::OnFishMoveAnimationFinished)
END_EVENT_TABLE()
