#pragma once
//#include "TerrainChunk.h"
//#include "GameWorld.h"

#include "Utils.h"
#include <string>


class GameWorld;
class Unit;
class TerrainChunk;

enum PlayerStaus
{
    NonRegistred = 1,
    Readay = 2,
    Observer = 3,
    NoReady = 4
};

// watch out ! player don't disappear after diconnect
class Player
{
private:
    TerrainChunk* base = nullptr;
    bool defeated = false;

    //tcp handler here:
public:
    std::string nick;
    bool isConnected = false;

    Player(std::string nick = "");
    ~Player();

    bool setBase(Player* players, TerrainChunk& base);
    Vect2 getBaseLoc();
    PlayerStaus getPlayerStatus();

    bool SendUnits(GameWorld& world, int count, Vect2 dest);

};