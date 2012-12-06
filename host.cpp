#include "host.h"
#include "fish.h"
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <utility>
#include "filelogger.h"
#include "guilogger.h"
#include <algorithm>
#include "syncobj.h"

#ifdef _DEBUG_VER_
    #include <iostream>
#endif

static inline int min(int a, int b){
    return (a<b)?a:b;
}

static inline int max(int a, int b){
    return (a>b)?a:b;
}

static inline int RandInt(int a, int b){
    return static_cast<int>(((static_cast<unsigned>(rand()) << 16) + (static_cast<unsigned>(rand()))) % (b-a+1) + a);
}

static std::pair<int, int> GetRandomCoord(){
    static Host& host = RetrieveHost();
    int x, y;
    do{
        x = RandInt(1, N);
        y = RandInt(1, M);
    } while (host.getMapContent(x, y) != EMPTY);
    return std::make_pair(x, y);
}

static inline bool IfCoordValid(int x, int y){
    return x>0 && x<=N && y>0 && y<=M;
}
inline bool Host::IfIdValid(int id) const{
    return id>=1 && id<=iIdPoolSize && fdpIdTable[id];
}

static inline int ManhattanDistance(int x1, int y1, int x2, int y2){
    return abs(x1-x2) + abs(y1-y2);
}

static inline bool ActionPriority(FishData_t* l, FishData_t* r){
    static Host &host = RetrieveHost();
    return (l->iStatus < r->iStatus) ||
           ((l->iStatus == r->iStatus) &&
            ( (l->iSpeed > r->iSpeed) ||
              ((l->iSpeed == r->iSpeed) &&
                (  (l->iHP * r->iMaxHP > r->iHP * l->iMaxHP) ||
                   ((l->iHP * r->iMaxHP == r->iHP * l->iMaxHP) &&
                      (host.getPoint(l->iId) < host.getPoint(r->iId)) )))));
}

static inline bool PointPriority(FishData_t* l, FishData_t* r){
    static Host &host = RetrieveHost();
    return (host.getPoint(l->iId) > host.getPoint(r->iId));
}

Host::Host():iPlayerCount(0), iDeathCount(0), iHostStatus(BEFOREFIRSTSETUP),
             elhLog(), mutex(RetrieveHostDataMutex()), condition(RetrieveResumeCondition()){
    srand(static_cast<unsigned>(time(0)));
}

Host::~Host(){
    Destroy();
}

void Host::SetLogger(){
    elhLog.AddLogger(new FileLogger("result.txt"));
    elhLog.AddLogger(new GuiLogger());
}

void Host::FirstSetup(){
    if (iHostStatus == BEFOREFIRSTSETUP) {
        SetLogger();
        iHostStatus = DESTROYED;
        Initialize();
        SetupAI();
    }
}

void Host::Reset(){
    if (iHostStatus == END){
        Destroy();
        Initialize();
        SetupAI();
    }
}

void Host::Destroy(){
    if (iHostStatus == END){
        wxMutexLocker lock(mutex);
        for (int i = 0; i!=MAX_PLAYER; ++i)
            delete fdpFishTable[i];
        iHostStatus = DESTROYED;

        elhLog.HostDestroyed(time(0));
    }
}


void Host::Initialize(){
    if (iHostStatus == DESTROYED){
        wxMutexLocker lock(mutex);
        iPlayerCount = 0;
        for (int i = 0; i!=iIdPoolSize; ++i){
            ipIdPool[i] = i+1;
        }
        for (int i = 0; i<=iIdPoolSize; ++i){
            fdpIdTable[i+1] = 0;
        }
        iDeathCount = 0;
        for (int i = 0; i!=MAX_PLAYER; ++i){
            fdpFishTable[i] = 0;
            fdpDeathTable[i] = 0;
        }
        for (int i = 1; i<=N; ++i)
        for (int j = 1; j<=M; ++j)
            ipMapContent[i][j] = EMPTY;
        iHostStatus = INITIALIZED;

        elhLog.HostInitialized(time(0));
    }
}

FishData_t::FishData_t(int id, fish* fIns):iId(id), iPropertyPoint(INITIAL_PROPERTY_POINT), iHP(0), iMaxHP(0), iStrength(0), iSpeed(0), iExp(0),
                                           iLevel(1), iPosX(INVALID_VALUE), iPosY(INVALID_VALUE), iKillBonus(0), iKillCount(0), iDeadCount(0),
                                           strIdentifier(), iPhase(END), iStatus(DEAD), iRoundSinceDead(0), fishInstance(fIns)
                                           {}

FishData_t::~FishData_t(){
    delete fishInstance;
}

int Host::AllocateID(fish* fIns){
    wxMutexLocker lock(mutex);
    if (iPlayerCount == MAX_PLAYER) return INVALID_VALUE; //number of player exceed the limit
    //Allocate ID
    {
        int iChosenIndex = RandInt(iPlayerCount, iIdPoolSize-1);
        if (iPlayerCount!=iChosenIndex){
            ipIdPool[iPlayerCount]^=ipIdPool[iChosenIndex];
            ipIdPool[iChosenIndex]^=ipIdPool[iPlayerCount];
            ipIdPool[iPlayerCount]^=ipIdPool[iChosenIndex];
        }
    }
    //Allocate Data Space
    fdpFishTable[iPlayerCount] = new FishData_t(ipIdPool[iPlayerCount], fIns);
    fdpIdTable[ipIdPool[iPlayerCount]] = fdpFishTable[iPlayerCount];

    return ipIdPool[iPlayerCount++];
}

int Host::getPropertyPoint(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iPropertyPoint) : INVALID_VALUE;
}

int Host::getPoint(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iKillBonus + fdpIdTable[id]->iExp) : INVALID_VALUE;
}

int Host::getLevel(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iLevel) : INVALID_VALUE;
}

int Host::getExp(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iExp) : INVALID_VALUE;
}

int Host::getX(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iPosX) : INVALID_VALUE;
}

int Host::getY(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iPosY) : INVALID_VALUE;
}

int Host::getHP(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iHP) : INVALID_VALUE;
}

int Host::getMaxHP(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iMaxHP) : INVALID_VALUE;
}

int Host::getAtt(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iStrength) : INVALID_VALUE;
}

int Host::getSp(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iSpeed) : INVALID_VALUE;
}

int Host::getMapContent(int x, int y) const{
    return (IfCoordValid(x,y)) ? (ipMapContent[x][y]) : EMPTY;
}

int Host::getMapContentWithId(int x, int y, int id) const{
    return (IfIdValid(id) && fdpIdTable[id]->iStatus != FishData_t::DEAD) ? getMapContent(x,y) : EMPTY;
}

int Host::getDeadCount(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iDeadCount) : INVALID_VALUE;
}

int Host::getKillCount(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iKillCount) : INVALID_VALUE;
}

int Host::getKillBonus(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->iKillBonus) : INVALID_VALUE;
}

int Host::getPlayerCount() const{
    return iPlayerCount;
}

wxString Host::getIdentifier(int id) const{
    return (IfIdValid(id)) ? (fdpIdTable[id]->strIdentifier) : wxT("INV");
}

void Host::setIdentifier(const wxString& str){
    if (iPlayerCount>0){
        fdpFishTable[iPlayerCount-1]->strIdentifier = str;
    }
}

void Host::setIdentifier(int id, const wxString& str){
    if (IfIdValid(id)) fdpIdTable[id]->strIdentifier = str;
}

bool Host::move(int id, int x, int y){
    if (!IfIdValid(id))  {
        return false;
    }  //invalid id
    if (!IfCoordValid(x, y)){
        elhLog.FishMove(id, getX(id), getY(id), x, y, 1);
        return false;
    }   //invalid coordinate
    if (getMapContent(x, y) != EMPTY){
        elhLog.FishMove(id, getX(id), getY(id), x, y, 4);
        return false;
    }  //position already occupied
    if (fdpIdTable[id]->iPhase!=FishData_t::START){
        elhLog.FishMove(id, getX(id), getY(id), x, y, 5);
        return false;
    }  // already moved or attacked
    if (fdpIdTable[id]->iSpeed < ManhattanDistance(getX(id), getY(id), x, y)){
        elhLog.FishMove(id, getX(id), getY(id), x, y, 3);
        return false;
    }

    mutex.Lock();
    int iFormerX = getX(id), iFormerY = getY(id);
    if (IfCoordValid(iFormerX, iFormerY))
        ipMapContent[iFormerX][iFormerY] = EMPTY;   //empty the original grid
    ipMapContent[x][y] = id;    //fill (x,y) with id
    fdpIdTable[id]->iPosX = x;  //update coord in FishData
    fdpIdTable[id]->iPosY = y;
    fdpIdTable[id]->iPhase = FishData_t::AFTER_MOVE;
    mutex.Unlock();
    elhLog.FishMove(id, iFormerX, iFormerY, x, y, 0);
    return true;

}

int Host::IncreaseHP(int id, int delta){
    if (delta<=0) return 0; //invalid delta

    delta = min(delta, fdpIdTable[id]->iMaxHP - fdpIdTable[id]->iHP);
    fdpIdTable[id]->iHP += delta;
    if (delta) elhLog.FishHPModified(id);
    return delta;
}

bool Host::DecreaseHP(int id, int delta){
    if (delta<=0) return false; //invalid delta

    fdpIdTable[id]->iHP -= delta;
    if (fdpIdTable[id]->iHP > 0) {
        if (delta) elhLog.FishHPModified(id);
        return false;
    }
    else{
        int iTempX = getX(id), iTempY = getY(id);
        fdpIdTable[id]->iHP = 0;
        fdpIdTable[id]->iStatus = FishData_t::DEAD;
        fdpIdTable[id]->iPhase = FishData_t::END;
        fdpIdTable[id]->iRoundSinceDead = 0;
        ipMapContent[iTempX][iTempY] = EMPTY;
        fdpIdTable[id]->iPosX = INVALID_VALUE;
        fdpIdTable[id]->iPosY = INVALID_VALUE;
        ++fdpIdTable[id]->iDeadCount;
        fdpDeathTable[iDeathCount++] = fdpIdTable[id];
        elhLog.FishDead(id, iTempX, iTempY);
        return true;
    }
}

int Host::IncreaseExp(int id, int delta){
    if (delta<=0) return 0; //invalide delta

    fdpIdTable[id]->iExp += delta;
    elhLog.FishExpIncreased(id);
    int newLevel = trunc((sqrt(9 + 8 * fdpIdTable[id]->iExp) - 1) / 2);
    while (((newLevel+3) * newLevel / 2) <= fdpIdTable[id]->iExp) ++newLevel;   //in case of float error
    while (((newLevel+2) * (newLevel-1) / 2) > fdpIdTable[id]->iExp) --newLevel;
    newLevel -= fdpIdTable[id]->iLevel;
    fdpIdTable[id]->iPropertyPoint+= 3 * newLevel;
    fdpIdTable[id]->iLevel+=newLevel;
    if (newLevel) elhLog.FishLevelUp(id);
    return newLevel;
}

void Host::RefreshFood(){
    static int ipPosX[MAX_FOOD], ipPosY[MAX_FOOD];
    wxMutexLocker lock(mutex);
    for (int i=0; i<N; ++i)
    for (int j=0; j<M; ++j)
        if (ipMapContent[i][j] == FOOD) ipMapContent[i][j] = EMPTY;

    for (int i=0; i<MAX_FOOD; ++i){
        std::pair<int, int> coord = GetRandomCoord();
        ipMapContent[coord.first][coord.second] = FOOD;
        ipPosX[i] = coord.first;
        ipPosY[i] = coord.second;
    }
    elhLog.FoodRefreshed(ipPosX, ipPosY);
}

bool Host::attack(int id, int x, int y){
    if (!IfIdValid(id)){
        return false;
    }  // invalid id
    if (!IfCoordValid(x, y)){
        elhLog.FishAttack(id, x, y, EMPTY, 1);
        return false;
    }  //invalid coordinate
    if (fdpIdTable[id]->iPhase != FishData_t::START && fdpIdTable[id]->iPhase!=FishData_t::AFTER_MOVE){
        elhLog.FishAttack(id, x, y, getMapContent(x, y), 5);
        return false;
    }  //attack denied during current phase
    if (getMapContent(x, y) == EMPTY){
        elhLog.FishAttack(id, x, y, getMapContent(x, y), 4);
        return false;
    } //nothing to attack
    if (getMapContent(x, y) == id){
        elhLog.FishAttack(id, x, y, getMapContent(x, y), 6);
        return false;
    }   //are you serious? YOU WANNA ATTACK YOURSELF?
    if (ManhattanDistance(getX(id), getY(id), x, y) > 1){
        elhLog.FishAttack(id, x, y, getMapContent(x, y), 3);
        return false;
    }  //target is not in range

    wxMutexLocker lock(mutex);
    fdpIdTable[id]->iPhase = FishData_t::AFTER_ATTACK;
    if (getMapContent(x, y) == FOOD){
        ipMapContent[x][y] = EMPTY;
        elhLog.FishAttack(id, x, y, FOOD, 0);
        IncreaseHP(id, max(2, getMaxHP(id) / 10));
        IncreaseExp(id, 1);
        return true;
    } else {
        int target = getMapContent(x, y);
        elhLog.FishAttack(id, x, y, getMapContent(x, y), 0);
        if (DecreaseHP(target, fdpIdTable[id]->iStrength)){
            IncreaseExp(id, max(1, fdpIdTable[target]->iLevel/2));
            fdpIdTable[id]->iKillBonus += (max(0, fdpIdTable[target]->iLevel - fdpIdTable[id]->iLevel) << 1);
            ++fdpIdTable[id]->iKillCount;
        }
        return true;
    }
}

bool Host::increaseHealth(int id, int delta){
    if ( (!IfIdValid(id))  //invalid id
      || (delta <= 0)   //invalid delta
      || (fdpIdTable[id]->iStatus == FishData_t::DEAD)  //dead fish cannot increase propery points
      || (fdpIdTable[id]->iPropertyPoint < delta)   //no sufficient property point
    ) return false;

    wxMutexLocker lock(mutex);
    fdpIdTable[id]->iMaxHP += (delta << 1);
    elhLog.FishHealthIncreased(id);
    fdpIdTable[id]->iHP += (delta << 1);
    elhLog.FishHPModified(id);
    fdpIdTable[id]->iPropertyPoint-=delta;
    return true;
}

bool Host::increaseStrength(int id, int delta){
    if ( (!IfIdValid(id))  //invalid id
      || (delta <= 0) //invalid delta
      || (fdpIdTable[id]->iStatus == FishData_t::DEAD)  //dead fish cannot increase propery points
      || (fdpIdTable[id]->iPropertyPoint < delta)   //no sufficient property point
    ) return false;

    wxMutexLocker lock(mutex);
    fdpIdTable[id]->iStrength += delta;
    elhLog.FishStrengthIncreased(id);
    fdpIdTable[id]->iPropertyPoint-=delta;
    return true;
}

bool Host::increaseSpeed(int id, int delta){
    if ( (!IfIdValid(id))  //invalid id
      || (delta <= 0) //invalid delta
      || (fdpIdTable[id]->iStatus == FishData_t::DEAD)  //dead fish cannot increase propery points
      || (fdpIdTable[id]->iPropertyPoint < delta)   //no sufficient property point
    ) return false;

    wxMutexLocker lock(mutex);
    fdpIdTable[id]->iSpeed += delta;
    elhLog.FishSpeedIncreased(id);
    fdpIdTable[id]->iPropertyPoint-=delta;
    return true;
}

void Host::InitializeFish(int i){
    {
        wxMutexLocker lock(mutex);
        std::pair<int, int> coord = GetRandomCoord();
        fdpFishTable[i]->iPosX = coord.first;
        fdpFishTable[i]->iPosY = coord.second;
        ipMapContent[coord.first][coord.second] = fdpFishTable[i]->iId;
        fdpFishTable[i]->iStatus = FishData_t::ALIVE;
    }
    fdpFishTable[i]->fishInstance->init();
    elhLog.FishBorn(fdpFishTable[i]->iId);
}

bool Host::ReviveFish(int id){
    FishData_t* fd = fdpIdTable[id];
    if (fd->iRoundSinceDead == iReviveAfterRound){
        std::pair<int, int> coord;
        fd->fishInstance->revive(coord.first, coord.second);

        {
            wxMutexLocker lock(mutex);
            if (!IfCoordValid(coord.first, coord.second) ||
                ipMapContent[coord.first][coord.second]!=EMPTY)
                coord = GetRandomCoord();
            ipMapContent[coord.first][coord.second] = id;
            fd->iPosX = coord.first;
            fd->iPosY = coord.second;
            fd->iStatus = FishData_t::ALIVE;
            fd->iHP = max(fd->iMaxHP / 10, 1);
        }
        elhLog.FishRevived(id);
        return true;
    } else {
        wxMutexLocker lock(mutex);
        ++fd->iRoundSinceDead;
        return false;
    }
}

void Host::DecideActionSequence(){
    static int ipIdList[MAX_PLAYER];
    std::sort(fdpFishTable, fdpFishTable+iPlayerCount, &ActionPriority);
    for (int i = 0; i<iPlayerCount; ++i)
        ipIdList[i] = fdpFishTable[i]->iId;
    elhLog.SequenceDecided(ipIdList);
}

void Host::ActivateFish(int id){
    FishData_t* fd = fdpIdTable[id];
    if (fd->iStatus == FishData_t::DEAD) return;

    fd->iPhase = FishData_t::START;
    elhLog.FishInAction(id);
    fd->fishInstance->play();
    fd->iPhase = FishData_t::END;
    elhLog.FishActionFinished(id);
}

void Host::EndGame(){
    static int ipIdList[MAX_PLAYER];
    std::sort(fdpFishTable, fdpFishTable+iPlayerCount, &PointPriority);
    for (int i = 0; i<iPlayerCount; ++i)
        ipIdList[i] = fdpFishTable[i]->iId;
    elhLog.GameEnded(time(0), ipIdList);
    iHostStatus = END;
}

bool Host::Pause(int pauseType){
    wxMutexLocker lock(mutex);
    SendPauseSignal(pauseType);
    condition.Wait();
    if (ForceEnd()) {
        EndGame();
        return true;
    }
    return false;
}

bool Host::Start(){
    if (iHostStatus == READY){
        iHostStatus = ONGOING;
        elhLog.GameStarted(time(0));

        //preparation
            for (int i=0; i<iPlayerCount; ++i){
                InitializeFish(i);
                if (Pause(PAUSE_INIT)) return true;
            }

        //ongoing
        for (int iRoundCount=0; iRoundCount<MAX_ROUND; ++iRoundCount){
            if (iRoundCount % FOOD_ROUND == 0) RefreshFood();     //refresh food every FOOD_ROUND rounds
            if (Pause(PAUSE_ROUND_START)) return true;   //  Wait for resuming signal

            elhLog.RoundStarted(iRoundCount);

            {
                int j = 0;
                for (int i = 0; i<iDeathCount; ++i){
                    if (!ReviveFish(fdpDeathTable[i]->iId)){
                        fdpDeathTable[j++] = fdpDeathTable[i];
                    }
                    if (Pause(PAUSE_REVIVE)) return true;
                }
                iDeathCount = j;
            }   // revive fish

            DecideActionSequence();
            if (Pause(PAUSE_SEQUENCE_DECIDED)) return true;

            // Activate each fish
            for (int i = 0; i<iPlayerCount; ++i){
                ActivateFish(fdpFishTable[i]->iId);
                if (Pause(PAUSE_FISH_ROUND_FINISHED)) return true;
            }
        }

        EndGame(); // report that the game has ended
        return true;
    }
    return false;
}


