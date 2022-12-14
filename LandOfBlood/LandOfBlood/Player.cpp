#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <vector>

#include "GameWorld.h"
#include "TerrainChunk.h"
#include "Resource.h"
#include "Utils.h"


using namespace std;

//------------------------ Player //------------------------
Player::Player(string name)
{
    nick = name;
}

Player::~Player()
{
}

bool Player::SetBase(Player* players, TerrainChunk& selectedBase) {
    //base can be selected ony once
    if (base != nullptr)
        return false;
    //start field cannot be the Water
    if (selectedBase.Type == Water)
        return false;
    //terrain own to somebody
    if (selectedBase.TerrainOwner != nullptr)
        return false;
    if (getPlayerStatus() != NoReady)
        return false;

    //check that field is free
    for (size_t i = 0; i < MaxPlayers; i++)
        if ((&players[i] != this) && (&selectedBase == players[i].base))
            return false;

    //set player base
    base = &selectedBase;
    base->GenResoucesAsPlayerBase(this);
    return true;
}

void Player::BaseUpdate() {
    base->OnModifed();
}

bool Player::SendUnits(GameWorld& world, int count, Vect2 dest) {
    Resource cost = Unit::CalculateCost(base->Loc, dest, count, world);
    // we have to affotrt that to go
    if (base->NaturalRes.canSubstract(cost)) {

        Unit u;
        if (base->StcjonaryUnit.sendUnits(count, dest, &u)) {
            //take cost
            base->NaturalRes.subResource(cost);
            // upadte player
            world.OnPlayerUpadate(*this);
            // send units and upadte base
            base->MovingUnits.push_back(u);
            base->ToUpadte();
            world.ReportEvent("Units send to " + dest.ToString(), this);
            return true;
        }
    }
    return false;
}

/*
    it looks like SendUnits Back Units are similar, mabe send from territory to territory
    will be better function(base including )
    but this in future(maybe not in project release)
*/
bool Player::BackUnits(GameWorld& world, int count, Vect2 teritory) {
    //cannot send form base to base
    if (base->Loc.CompareValues(teritory))
        return false;

    TerrainChunk* chunk = world.getChunkForUpadte(teritory.X, teritory.Y);

    if (chunk->TerrainOwner != this)
        return false;

    Resource cost = Unit::CalculateCost(teritory, base->Loc, count, world);

    /*cout << "costs:" << endl;
    cost.PrintResources();*/

    // we have to affotrt that to go
    if (base->NaturalRes.canSubstract(cost)) {
        Unit u;
        if (chunk->StcjonaryUnit.sendUnits(count, base->Loc, &u)) {
            // take cost
            base->NaturalRes.subResource(cost);
            // upade player
            world.OnPlayerUpadate(*this);
            // upade tertiory
            chunk->MovingUnits.push_back(u);
            chunk->ToUpadte();
            world.ReportEvent("Units send to base " + base->Loc.ToString(), this);
            return true;
        }
    }
    else {
        world.ReportEvent("You cant afford for send units from " + teritory.ToString() + " to your base " + base->Loc.ToString(), this);
    }
    return false;
}

bool Player::IsPlayerBase(TerrainChunk& terrain) {
    if (base == nullptr)
        return false;
    return (terrain.Loc.CompareValues(base->Loc));
}
bool Player::HasAnyOtherTeriory(GameWorld& world) {
    for (size_t x = 0; x < WorldSize; x++)
        for (size_t y = 0; y < WorldSize; y++) {
            TerrainChunk chunk = world.getChunk(x, y);
            // if any terrain is owned by player but is not his base succes
            if (chunk.TerrainOwner == this && !IsPlayerBase(chunk))
                return true;
        }
    return false;
}

PlayerStaus Player::getPlayerStatus() {
    // if player fail
    if (nick == "")
        return NonRegistred;
    if (defeated)
        return Observer;
    if (base == nullptr)
        return NoReady;

    return Readay;
}
Vect2 Player::getBaseLoc() {
    return base->Loc;
}
Resource* Player::getPlayerResources() {
    return &base->NaturalRes;
}
void  Player::setPlayerDefeated() {
    defeated = true;

}