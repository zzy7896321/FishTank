#include "fish.h"
#include "host.h"
#include <iostream>
#include <string>

Host& fish::host = RetrieveHost();

void fish::setID(int id){
    if (iId == INVALID_VALUE) iId = id;
}

fish::fish():iId(INVALID_VALUE){
    setID(host.AllocateID(this));
}

fish::~fish(){}

bool fish::move(int x, int y){
    return host.move(iId, x, y);
}

bool fish::attack(int x, int y){
    return host.attack(iId, x, y);
}

int fish::getPoint(){
    return host.getPropertyPoint(iId);
}

int fish::getLevel(){
    return host.getLevel(iId);
}

int fish::getExp(){
    return host.getExp(iId);
}

int fish::getX(){
    return host.getX(iId);
}

int fish::getY(){
    return host.getY(iId);
}

int fish::getHP(){
    return host.getHP(iId);
}

int fish::getMaxHP(){
    return host.getMaxHP(iId);
}

int fish::getAtt(){
    return host.getAtt(iId);
}

int fish::getSp(){
    return host.getSp(iId);
}

int fish::getID(){
    return iId;
}

int fish::askWhat(int x, int y){
    return host.getMapContentWithId(x, y, iId);
}

int fish::askHP(int id){
    return host.getHP(id);
}

bool fish::increaseHealth(){
    return host.increaseHealth(iId);
}

bool fish::increaseStrength(){
    return host.increaseStrength(iId);
}

bool fish::increaseSpeed(){
    return host.increaseSpeed(iId);
}
