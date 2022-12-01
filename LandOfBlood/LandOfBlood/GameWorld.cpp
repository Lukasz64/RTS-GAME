#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <vector>

#include "Resource.h"
#include "Utils.h"
#include "GameWorld.h"



using namespace std;

ChunkType getRng() {
    int all = ((1 + TypesCount) * TypesCount) / 2;
    int rng = rand() % all;
    int range = 0;

    for (size_t i = 0; i < TypesCount; i++) {
        range += TypesCount - i;

        if (rng < range)
            return (ChunkType)i;
    }
    return Montain;
}

TerrainChunk GameWorld::getChunk(int x, int y) {
    return chunks[x][y];
}

void GameWorld::PrintMap() {
    COLORS cc[] = { YELLOW,BLUE,GREEN,BLACK };
    int stat[4] = { 0 };

    for (size_t x = 0; x < WorldSize; x++) {
        for (size_t y = 0; y < WorldSize; y++) {
            ChunkType chunk = chunks[x][y].Type;
            stat[chunk]++;
            cout << colorize((cc[chunk])) << "[" << colorize(WHITE) << chunk << colorize((cc[chunk])) << "]" << colorize(NC);
        }
        cout << endl;
    }
    cout << "----------" << endl;

    for (size_t x = 0; x < 4; x++) {
        cout << "[" << x << "]=" << stat[x] << endl;
    }
}
void GameWorld::PrintPlayes() {

    string stas[] = {"NonRegistred","Readay","Observer","NoReady","Connected"};

    for (size_t x = 0; x < MaxPlayers; x++) {
        PlayerStaus s = players[x].getPlayerStatus();
        cout <<"Player: "<< players[x].nick << " " << stas[s-1] << " (" << ((players[x].isConnected) ? "connected)" : "not connected)") << endl;
    }
   
}



GameWorld::GameWorld(unsigned int seed)
{
    //gen world
    srand(seed);
    COLORS cc[] = {
        YELLOW,BLUE,GREEN,BLACK
    };
    for (size_t x = 0; x < WorldSize; x++)
        for (size_t y = 0; y < WorldSize; y++) {
            ChunkType chunk = getRng();
            //init chunks
            chunks[x][y] = TerrainChunk(chunk, Vect2(x, y));
        }
}

GameWorld::~GameWorld()
{
}

int GameWorld::FindPlayer(string nick) {
    for (size_t x = 0; x < MaxPlayers; x++)
        if (players[x].nick == nick)
            return x;
    
    return -1;
}
int GameWorld::FindFreePlayerSlot() {
    for (size_t x = 0; x < MaxPlayers; x++)
        if (players[x].getPlayerStatus() == NonRegistred)
            return x;
    return -1;
}
int GameWorld::FindActiveConnectedPlayer() {
    for (size_t x = 0; x < MaxPlayers; x++)
        if (players[x].getPlayerStatus() == Readay && players[x].isConnected)
            return x;
    return -1;
}
//
bool GameWorld::JoinPlayer(string nick)
{
    int palyerSlot = FindPlayer(nick);
    if (palyerSlot != -1) {
        if (players[palyerSlot].isConnected)
            return false;// dubler join
        else {
            //come back to game
            players[palyerSlot].isConnected = true;
            return true;
        }
    }
    int freeSlot = FindFreePlayerSlot();
    if (freeSlot != -1) {
        players[freeSlot] = Player(nick);
        players[freeSlot].isConnected = true;
    }

    return   false;// room full
}
//
bool GameWorld::LeftPlayer(string nick)
{
    int palyerSlot = FindPlayer(nick);
    if (palyerSlot != -1) {
        if (players[palyerSlot].isConnected) {
            //[TODO]* fix change world owner

            //if player was not reigstred only connected
            if (players[palyerSlot].getPlayerStatus() == NoReady)
                players[palyerSlot] = Player();//delete player at all
            else
                players[palyerSlot].isConnected = false;

            return true;
        }
    }

    return   false;// left error
}
//
bool GameWorld::SetPlayerBase(string nick, Vect2 terrLoc) {
    int player = FindPlayer(nick);
    if (player != -1) {
        players[player].setBase(players, chunks[terrLoc.X][terrLoc.Y]);
        //[TODO] end this
    }
    // player not found
    return false;
}

bool Player::setBase(Player * players, TerrainChunk & selectedBase) {
    //base can be selected ony once
    if (base != nullptr)
        return false;
    //start field cannot be the Water
    if (selectedBase.Type == Water)
        return false;
    //check tahat field is free
    for (size_t i = 0; i < MaxPlayers; i++)
        if ((&players[i] != this) && (&selectedBase == players[i].base))
            return false;
 
    //set player base
    base = &selectedBase;
}

Player::Player(string name)
{
   nick = name;
}

Player::~Player()
{
}
PlayerStaus Player::getPlayerStatus() {
    // if player fail
    if (nick == "")
        return NonRegistred;
    if (defeated)
        return Observer;
    if(base == nullptr)
        return NoReady;

    return Readay;
}
