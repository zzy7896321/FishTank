#ifndef _WX_COMMONHEADER_H_
#define _WX_COMMONHEADER_H_

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#endif

enum{
    ID_MainFrame = wxID_HIGHEST + 1,
    ID_TankFrame,
    ID_DataList,
    ID_DataChoice,
    ID_LogList,
    ID_StartButton,
    ID_StopButton,
    ID_PauseButton,
    ID_MenuControl_Start,
    ID_MenuControl_Stop,
    ID_MenuControl_Pause,
    ID_MenuPauseWhen_RoundStart,
    ID_MenuPauseWhen_FishInitialized,
    ID_MenuPauseWhen_FishRevived,
    ID_MenuPauseWhen_FishActionEnded,
    ID_MenuPauseWhen_ActionSequenceDecided,

};

#endif // _WX_COMMONHEADER_H_
