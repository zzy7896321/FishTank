#include "notificationevents.h"

DEFINE_LOCAL_EVENT_TYPE(ftEVT_TEXT_PROMPT)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_ROUND_STARTED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_PAUSE_SIGNAL)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_GAME_ENDED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_SEQUENCE_DECIDED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FOOD_REFRESHED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_BORN)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_REVIVED)
DEFINE_LOCAL_EVENT_TYPE(fTEVT_FISH_IN_ACTION)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_DEAD)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_MOVE)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_ATTACK)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_EXP_INCREASED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_LEVEL_UP)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_HP_MODIFIED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_ACTION_FINISHED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_HEALTH_INCREASED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_SPEED_INCREASED)
DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_STRENGTH_INCREASED)

DEFINE_LOCAL_EVENT_TYPE(ftEVT_FISH_MOVE_ANIMATION_FINISHED)
IMPLEMENT_DYNAMIC_CLASS(ftTextPromptEvent, wxNotifyEvent)

ftTextPromptEvent::ftTextPromptEvent(const wxString& str, wxEventType commandType, int id):
    wxNotifyEvent(commandType, id), strToPrompt(str){}

wxEvent* ftTextPromptEvent::Clone() const{
    return new ftTextPromptEvent(*this);
}

const wxString& ftTextPromptEvent::GetStr() const {
    return strToPrompt;
}

IMPLEMENT_DYNAMIC_CLASS(ftPauseSignalEvent, wxNotifyEvent)

ftPauseSignalEvent::ftPauseSignalEvent(int pt, wxEventType commandType, int id):
    wxNotifyEvent(commandType, id), pauseType(pt){}

wxEvent* ftPauseSignalEvent::Clone() const{
    return new ftPauseSignalEvent(*this);
}

int ftPauseSignalEvent::GetPauseType() const{
    return pauseType;
}

IMPLEMENT_DYNAMIC_CLASS(ftEventWithIntArray, wxNotifyEvent)

ftEventWithIntArray::ftEventWithIntArray(wxEventType commandType, int id):
    wxNotifyEvent(commandType, id){}

wxEvent* ftEventWithIntArray::Clone() const{
    return new ftEventWithIntArray(*this);
}

IMPLEMENT_DYNAMIC_CLASS(ftEventWithID, wxNotifyEvent)

ftEventWithID::ftEventWithID(int fishId, int x, int y, int x1, int y1,
                             wxEventType commandType, int id):
    wxNotifyEvent(commandType, id), iId(fishId), x(x), y(y), x1(x1), y1(y1){}

wxEvent* ftEventWithID::Clone() const{
    return new ftEventWithID(*this);
}
