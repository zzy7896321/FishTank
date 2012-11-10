#include "stdoutlogger.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include "constants.h"
#include "host.h"

const char* pstrResultString[7] = {"Success.",     //0
                            "Failure: invalid target coordinate.",      //1
                            "Failure: target is already occupied.",     //2
                            "Failure: target is out of range.",     //3
                            "Failure: target is empty.",        //4
                            "Failure: operation is allowed once each round."    //5
                            "Failure: it's not good to commit suicide."     //6
                           };

static void PrintTime(std::ostream& out, time_t tmTime){
    tm* tmLocalTime = localtime(&tmTime);
    out << '[' << tmLocalTime->tm_year+1900 << '/'
        << tmLocalTime->tm_mon << '/'
        << tmLocalTime->tm_mday << ' '
        << tmLocalTime->tm_hour << ':'
        << tmLocalTime->tm_min << ':'
        << tmLocalTime->tm_sec << ']';
}

StdoutLogger::StdoutLogger():EventLogger("StdoutLogger"), Out(std::cout){}

void StdoutLogger::HostInitialized(time_t tmTime){
    PrintTime(Out, tmTime);
    Out << " Host is initialized." << std::endl;
}

void StdoutLogger::AISetup(time_t tmTime, int iCount){
    PrintTime(Out, tmTime);
    Out << ' ' << iCount << " AI(s) are set up." << std::endl;
}

void StdoutLogger::GameStarted(time_t tmTime){
    PrintTime(Out, tmTime);
    Out << " Game is started." << std::endl;
}

void StdoutLogger::GameEnded(time_t tmTime, const int* iId){
    using std::endl;
    using std::setw;
    PrintTime(Out, tmTime);
    Out << " Game is ended." << endl;
    Out << "Result:" << endl;
    Out << std::left;
    Out << setw(7) << "Id"
        << setw(7) << "Points"
        << setw(7) << "Bonus"
        << setw(7) << "Exp"
        << setw(7) << "Level"
        << setw(7) << "Kill"
        << setw(7) << "Dead"
        << setw(7) << "Attack"
        << setw(7) << "Speed"
        << setw(7) << "HP"
        << setw(7) << "MaxHP"
        << endl;
    for (int i = 0; i<host.getPlayerCount(); ++i){
        int x = iId[i];
        Out << setw(7) << x
            << setw(7) << host.getPoint(x)
            << setw(7) << host.getKillBonus(x)
            << setw(7) << host.getExp(x)
            << setw(7) << host.getLevel(x)
            << setw(7) << host.getKillCount(x)
            << setw(7) << host.getDeadCount(x)
            << setw(7) << host.getAtt(x)
            << setw(7) << host.getSp(x)
            << setw(7) << host.getHP(x)
            << setw(7) << host.getMaxHP(x)
            << endl;
    }

}

void StdoutLogger::HostDestroyed(time_t tmTime){
    PrintTime(Out, tmTime);
    Out << " Host is destroyed." << std::endl;
}

void StdoutLogger::FishBorn(int iId){
    Out << "...Fish " << iId << " is born at (" << host.getX(iId) << ", " << host.getY(iId) <<")." << std::endl;
}

void StdoutLogger::FoodRefreshed(const int* ipPosX, const int* ipPosY){
    Out << "...Food is refreshed." << std::endl;
    #ifdef _DEBUG_VER_
        Out << "......<Position List>" << std::endl;
        for (int i = 0; i<MAX_FOOD; ++i)
            Out << "......(" << ipPosX[i] << ", " << ipPosY[i] << ")" << std::endl;
    #endif
}

void StdoutLogger::FishInitializing(int iId){
    Out << "...Fish " << iId << " is being initialized." << std::endl;
}

void StdoutLogger::RoundStarted(int iRoundCount){
    Out << "...Round " << iRoundCount << std::endl;
}

void StdoutLogger::FishRevived(int iId){
    Out << "...Fish " << iId << " is revived at (" << host.getX(iId) << ", " << host.getY(iId) << ")." << std::endl;
}

void StdoutLogger::SequenceDecided(const int* iId){
    Out << "...Sequence is decided." << std::endl;
    #ifdef _DEBUG_VER_
        Out << "......<Sequence>" << std::endl;
        for (int i = 0, tar = host.getPlayerCount(); i<tar; ++i)
            Out << "......Fish " << iId[i] << std::endl;
    #endif
}

void StdoutLogger::FishInAction(int iId){
    Out << "......Fish " << iId << " is now in action." << std::endl;
    #ifdef _DEBUG_VER_
        Out << ".........<Fish Properties>" << std::endl;
        Out << ".........Remaining property points: " << host.getPropertyPoint(iId) << std::endl;
        Out << ".........Points: " << host.getPoint(iId) << std::endl;
        Out << ".........Level: " << host.getLevel(iId) << std::endl;
        Out << ".........Experience: " << host.getExp(iId) << std::endl;
        Out << ".........X-coordinate: " << host.getX(iId) << std::endl;
        Out << ".........Y-coordinate: " << host.getY(iId) << std::endl;
        Out << ".........HP: " << host.getHP(iId) << std::endl;
        Out << ".........MaxHP:" << host.getHP(iId) << std::endl;
        Out << ".........Attack: " << host.getAtt(iId) << std::endl;
        Out << ".........Speed: " << host.getSp(iId) << std::endl;
    #endif
}

void StdoutLogger::FishMove(int iId, int iFormerPosX, int iFormerPosY, int iTargetPosX, int iTargetPosY, int iResult){
    Out << ".........Fish " << iId << " attempts to move from (" << iFormerPosX << ", "
         << iFormerPosY << ") to (" <<  iTargetPosX << ", " << iTargetPosY << "). "
         << pstrResultString[iResult] << std::endl;
}

void StdoutLogger::FishAttack(int iId, int iTargetPosX, int iTargetPosY, int iTarget, int iResult){
    Out << ".........Fish(att= " << host.getAtt(iId) << ") " << iId << " attempts to attack ";
    switch (iTarget){
    case FOOD:
        Out << "food";
        break;
    case EMPTY:
        Out << "null";
        break;
    default:
        Out << "fish " << iTarget;
    }
    Out <<" at (" << iTargetPosX << ", " << iTargetPosY << "). " << pstrResultString[iResult] << std::endl;
}

void StdoutLogger::FishDead(int iId){
    Out << ".........Fish " << iId << " is dead" << std::endl;
}

void StdoutLogger::FishExpIncreased(int iId){
    #ifdef _DEBUG_VER_
        Out << ".........Fish " << iId << "'s exp is increased to "<< host.getExp(iId) << std::endl;
    #endif
}

void StdoutLogger::FishLevelUp(int iId){
    Out << ".........Fish " << iId << " is upgraded to Lv. " << host.getLevel(iId) << std::endl;
}

void StdoutLogger::FishHPModified(int iId){
    Out << ".........Fish " << iId << "'s HP is now " << host.getHP(iId) << std::endl;
}

void StdoutLogger::FishActionFinished(int iId){
    Out << "......Fish " << iId << " quits action normally." << std::endl;
}

void StdoutLogger::FishTimeout(int iId, int iTimeUsage){
    Out << "......Fish " << iId << " exceeds time limit. Time usage: " << iTimeUsage << "ms." << std::endl;
}

void StdoutLogger::FishHealthIncreased(int iId){
}

void StdoutLogger::FishSpeedIncreased(int iId){
}

void StdoutLogger::FishStrengthIncreased(int iId){
}
