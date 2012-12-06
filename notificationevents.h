#ifndef _NOTIFICATIONEVENTS_H_
#define _NOTIFICATIONEVENTS_H_

#include "wx_commonheader.h"
#include <utility>

enum{
    PAUSE_ROUND_START,
    PAUSE_FISH_ROUND_FINISHED,
    PAUSE_INIT,
    PAUSE_REVIVE,
    PAUSE_SEQUENCE_DECIDED,
    PAUSE_WAIT_FOR_MOVE_ANIMATION,
};

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_TEXT_PROMPT, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_ROUND_STARTED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_PAUSE_SIGNAL, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_GAME_ENDED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_SEQUENCE_DECIDED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FOOD_REFRESHED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_BORN, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_REVIVED, -1)
    DECLARE_LOCAL_EVENT_TYPE(fTEVT_FISH_IN_ACTION, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_DEAD, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_MOVE, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_ATTACK, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_EXP_INCREASED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_LEVEL_UP, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_HP_MODIFIED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_ACTION_FINISHED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_HEALTH_INCREASED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_SPEED_INCREASED, -1)
    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_STRENGTH_INCREASED, -1)

    DECLARE_LOCAL_EVENT_TYPE(ftEVT_FISH_MOVE_ANIMATION_FINISHED, -1)
END_DECLARE_EVENT_TYPES()

class ftTextPromptEvent:public wxNotifyEvent{
public:
    ftTextPromptEvent(const wxString& str = wxT(""), wxEventType commandType = ftEVT_TEXT_PROMPT, int id = 0);
    virtual wxEvent* Clone() const;

    const wxString& GetStr() const;
private:
    wxString strToPrompt;

    DECLARE_DYNAMIC_CLASS(ftTextPromptEvent);
};

typedef void (wxEvtHandler::*ftTextPromptEventFunction)(ftTextPromptEvent&);

#define FT_EVT_TEXT_PROMPT(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_TEXT_PROMPT, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftTextPromptEventFunction, &fn), (wxObject*) NULL),

#define FT_EVT_ROUND_STARTED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_ROUND_STARTED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftTextPromptEventFunction, &fn), (wxObject*) NULL),

class ftPauseSignalEvent:public wxNotifyEvent{
public:
    ftPauseSignalEvent(int pt = PAUSE_ROUND_START, wxEventType commandType = ftEVT_PAUSE_SIGNAL, int id = 0);
    virtual wxEvent* Clone() const;

    int GetPauseType() const;
private:
    int pauseType;

    DECLARE_DYNAMIC_CLASS(ftPauseSignalEvent);
};

typedef void (wxEvtHandler::*ftPauseSignalEventFunction)(ftPauseSignalEvent&);

#define FT_EVT_PAUSE_SIGNAL(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_PAUSE_SIGNAL, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftPauseSignalEventFunction, &fn), (wxObject*) NULL),

class ftEventWithIntArray:public wxNotifyEvent{
public:
    ftEventWithIntArray(wxEventType commandType= wxEVT_NULL, int id = 0);
    virtual wxEvent* Clone() const;

    const int *arr1, *arr2;
private:
    DECLARE_DYNAMIC_CLASS(ftEventWithIntArray)
};

typedef void (wxEvtHandler::*ftEventWithIntArrayFunction)(ftEventWithIntArray&);

#define FT_EVT_GAME_ENDED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_GAME_ENDED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIntArrayFunction, &fn), (wxObject*) NULL),

#define FT_EVT_SEQUENCE_DECIDED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_SEQUENCE_DECIDED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIntArrayFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FOOD_REFRESHED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FOOD_REFRESHED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIntArrayFunction, &fn), (wxObject*) NULL),

class ftEventWithID:public wxNotifyEvent{
public:
    ftEventWithID(int fishId = 0, int x = 0, int y = 0 , int x1 = 0, int y1 = 0,
                  wxEventType commandType = wxEVT_NULL, int id = 0);
    virtual wxEvent* Clone() const;

    int iId, x, y, x1, y1;
private:

    DECLARE_DYNAMIC_CLASS(ftEventWithID)
};

typedef void (wxEvtHandler::*ftEventWithIDFunction)(ftEventWithID&);

#define FT_EVT_FISH_BORN(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_BORN, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_REVIVED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_REVIVED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_IN_ACTION(fn) \
    DECLARE_EVENT_TABLE_ENTRY( fTEVT_FISH_IN_ACTION, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_DEAD(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_DEAD, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_MOVE(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_MOVE, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_ATTACK(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_ATTACK, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_EXP_INCREASED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_EXP_INCREASED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_LEVEL_UP(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_LEVEL_UP, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_HP_MODIFIED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_HP_MODIFIED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_ACTION_FINISHED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_ACTION_FINISHED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_HEALTH_INCREASED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_HEALTH_INCREASED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_SPEED_INCREASED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_SPEED_INCREASED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_STRENGTH_INCREASED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_STRENGTH_INCREASED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#define FT_EVT_FISH_MOVE_ANIMATION_FINISHED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( ftEVT_FISH_MOVE_ANIMATION_FINISHED, -1, -1, \
    (wxObjectEventFunction)(wxEventFunction) \
    wxStaticCastEvent( ftEventWithIDFunction, &fn), (wxObject*) NULL),

#endif // _NOTIFICATIONEVENTS_H_
