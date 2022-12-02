#pragma once
#include "TerrainChunk.h"
#include <string>

const int WorldSize = 12;
const int MaxPlayers = 8;

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
    TerrainChunk* base = nullptr;
    bool defeated = false;
    


    //tcp handler here:
public:
    std::string nick;
    bool isConnected = false;

    Player(std::string nick = "");
    ~Player();

    bool setBase(Player * players, TerrainChunk& base);
    PlayerStaus getPlayerStatus();

private:

};

class GameWorld
{
private:
    TerrainChunk chunks[WorldSize][WorldSize];
    Player       players[MaxPlayers];

    int FindPlayer(std::string nick);
    int FindFreePlayerSlot();
    int FindActiveConnectedPlayer();

public:
    GameWorld(unsigned int seed = 12);
    ~GameWorld();

    TerrainChunk getChunk(int x, int y);
    bool         JoinPlayer(std::string nick);
    bool         LeftPlayer(std::string nick);
    bool         SetPlayerBase(std::string nick, Vect2 terrLoc);

    void PrintMap();
    void PrintPlayes();

private:


};
