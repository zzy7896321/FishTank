#ifndef _HOST_H_
#define _HOST_H_

#include "constants.h"
#include "eventlogger.h"
#include <string>

class fish;

struct FishData_t{
        int iId;
        int iPropertyPoint;
        int iHP, iMaxHP;
        int iStrength, iSpeed;
        int iExp, iLevel;
        int iPosX, iPosY;
        int iKillBonus;      // = 2 * sigma(max{0,level(victim)-level(killer))
        int iKillCount, iDeadCount;
        std::string strIdentifier;
        enum {START, AFTER_MOVE, AFTER_ATTACK, END} iPhase;
        enum {ALIVE = 0, DEAD = 1} iStatus;
        int iRoundSinceDead;
        fish* fishInstance;
        FishData_t(int id, fish* fIns);
        ~FishData_t();
};

class Host{
public:
    Host();
    ~Host();

    void FirstSetup();
    bool Start();   //start game
    void Reset();   //reset game
    void SetLogger();


    int AllocateID(fish* fIns);   //Apply for an ID and Data Space

    int getPropertyPoint(int id) const; //retrieve the remaining property point of id
    int getPoint(int id) const;   //retrieve the point of id
    int getLevel(int id) const;   //retrieve the level of id
    int getExp(int id) const;   //retrieve the experience of id
    int getX(int id) const;     //retrieve the x coordinate of id
    int getY(int id) const;     //retrieve the y corrdinate of id
    int getHP(int id) const;    //retrieve the HP of id
    int getMaxHP(int id) const;     //retrieve the MaxHP of id
    int getAtt(int id) const;   //retrieve the attack of id
    int getSp(int id) const;    //retrieve the speed of id
    int getMapContent(int x, int y) const;   //retrieve the map content on (x,y)
    int getMapContentWithId(int x, int y, int id) const;    //retrieve the map content on (x,y) with verifying of fish's phase
    int getKillCount(int id) const;
    int getDeadCount(int id) const;
    int getKillBonus(int id) const;
    std::string getIdentifier(int id) const;

    bool move(int id, int x, int y);    //move id to (x,y)
    bool attack(int id, int x, int y);  //id commence an attack on (x,y)
    bool increaseHealth(int id, int delta = 1); //increase id's Health by delta
    bool increaseStrength(int id, int delta = 1); //increase id's Strength by delta
    bool increaseSpeed(int id, int delta = 1); //increase id's Speed by delta

    int getPlayerCount() const;  //retrieve the number of players

private:

    enum {iReviveAfterRound = 2, iIdPoolSize = static_cast<int>(MAX_PLAYER)};

    int iPlayerCount;    //the number of players online
    int ipIdPool[iIdPoolSize];     //ids in [iPlayerCount..iIdPoolSize-1] are available for allocation

    FishData_t* fdpIdTable[iIdPoolSize+1];    //to retrieve fishdata with id
    FishData_t* fdpFishTable[MAX_PLAYER];   //for traversal of all fish

    int ipMapContent[N+1][M+1];   //map

    int iDeathCount;    //the number of dead fish
    FishData_t* fdpDeathTable[MAX_PLAYER];   //fish to revive nextRound

    void Initialize();  //reset all
    void Destroy(); //destroy dynamic objects of FishData_t and fish
    void SetupAI();  //create all fish subclass objects
    enum {READY, ONGOING, END, DESTROYED, INITIALIZED, BEFOREFIRSTSETUP} iHostStatus;  //indicates the host's status

    bool IfIdValid(int id) const;   //check if the id is valid

    //caution: the following modification functions do not check the validation of id
    int IncreaseHP(int id, int delta);  //increase the HP of id, return value is the HP actually increased
    bool DecreaseHP(int id, int delta);  //decrease the HP of id, true indicates id is dead
    int IncreaseExp(int id, int delta); //increase the exp of id, return how many levels id is upgraded

    //Food related functions
    void RefreshFood();     //refresh food, need to be invoked every FOOD_ROUND rounds

    EventLoggerHub elhLog;
};

inline Host& RetrieveHost(){
    static Host host;
    return host;
}

#endif  //_HOST_H_
