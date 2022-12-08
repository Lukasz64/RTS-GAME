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
TerrainChunk * GameWorld::getChunkForUpadte(int x, int y,bool modfiMark) {
    if(modfiMark)
        chunks[x][y].OnModifed();
    return &chunks[x][y];
}

void GameWorld::PrintMap() {
    COLORS cc[] = { YELLOW,BLUE,GREEN,BLACK };
    int stat[4] = { 0 };

    for (size_t y = 0; y < WorldSize; y++) {
        for (size_t x = 0; x < WorldSize; x++) {
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



GameWorld::GameWorld(std::string worldName,unsigned int seed)
{
    name = worldName;
    //gen world
    srand(seed);
    for (size_t x = 0; x < WorldSize; x++)
        for (size_t y = 0; y < WorldSize; y++) {
            ChunkType chunk = getRng();
            //init chunks
            chunks[x][y] = TerrainChunk(chunk, Vect2(x, y));
            chunks[x][y].StcjonaryUnit = Unit(Vect2(x, y));
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
        if (players[x].isConnected)//players[x].getPlayerStatus() == Readay &&
            return x;
    return -1;
}
//
bool GameWorld::JoinPlayer(string nick)
{
    //dont accept empty nick
    if(nick == "")
        return false;
        
    int palyerSlot = FindPlayer(nick);
    if (palyerSlot != -1) {
        if (players[palyerSlot].isConnected)
            return false;// dubler join
        else {
            //come back to game          
            players[palyerSlot].isConnected = true;
            PlayerUpadate(palyerSlot);
            return true;
        }
    }
    int freeSlot = FindFreePlayerSlot();
    if (freeSlot != -1) {
        players[freeSlot] = Player(nick);
        players[freeSlot].isConnected = true;
        if(worldOwner == -1)
            worldOwner = freeSlot;

        PlayerUpadate(freeSlot);
        return true;
    }

    return   false;// room full
}
void GameWorld::ShowGameOwner(){
    int pl = worldOwner;
    if(pl != -1){
        cout << "---------------------->"<<endl<<"world owner:" << players[pl].nick << endl;
    } else {
        cout << "to delete room" << endl;
    }

}

//
bool GameWorld::LeftPlayer(string nick)
{
    int palyerSlot = FindPlayer(nick);
    if (palyerSlot != -1) {
        if (players[palyerSlot].isConnected) {
            


            //if player was not reigstred only connected
            if (players[palyerSlot].getPlayerStatus() == NoReady)
                players[palyerSlot] = Player();//delete player at all
            else
                players[palyerSlot].isConnected = false;

            if (worldOwner == palyerSlot && gameRunning == false) {
                cout << "-------------------host change--------------------"<< endl;
                worldOwner = FindActiveConnectedPlayer();
                PlayerUpadate(worldOwner);
            }
            return true;
        }
    }

    return   false;// left error
}
//
bool GameWorld::SetPlayerBase(string nick, Vect2 terrLoc) {
    int player = FindPlayer(nick);
    if (player != -1) {
        return players[player].setBase(players, *getChunkForUpadte(terrLoc.X, terrLoc.Y));// chunks[terrLoc.X][terrLoc.Y]
    }
    // player not found
    return false;
}
void GameWorld::ReportEvent(std::string message, Player* pl) {
    if (pl != nullptr) {
        cout << colorize(BLUE) << "To " << pl->nick << " event: " << message<< colorize(NC) << endl;
    }
    else {
        cout << colorize(GREEN) << "To all event: " << message << colorize(NC) << endl;
    }
}
void GameWorld::GameTick() {
    cout << "--------------- Day:"<<day++<<"---------------" << endl;
    
    for (size_t x = 0; x < WorldSize; x++)
        for (size_t y = 0; y < WorldSize; y++) {
            Unit::ProcessUints(Vect2(x,y), *this);
            //process rest
        }

    for (size_t x = 0; x < WorldSize; x++)
        for (size_t y = 0; y < WorldSize; y++) {
            //remove lock from units movment( lock is to prevend dobule processing the same unit)
            Unit::UnlockUpadte(Vect2(x, y), *this);
            if ((chunks[x][y].ToUpadte())) {
                OnChunkUpadte(chunks[x][y]);         
            }
        }
}
void GameWorld::OnChunkUpadte(TerrainChunk & chunk){
    //cout << "chunk" << chunk.Loc.ToString() << " needs update" << endl;
}

void GameWorld::PlayerUpadate(int plId){
    if(plId != -1){
        OnPlayerUpadate(players[plId]);
    } else {
        cout << "wrong player update(player ID = -1)" << endl;
    }
}
void GameWorld::OnPlayerUpadate(Player & pl){
        cout << "player " << pl.nick << " needs update" << endl;
}
bool GameWorld::SendUints(std::string nick,int count, Vect2 terrDes) {
    int player = FindPlayer(nick);
    if (player != -1 && players[player].getPlayerStatus() == Readay) {
        return players[player].SendUnits(*this, count, terrDes);// chunks[terrLoc.X][terrLoc.Y]
    }
    // player not found
    return false;
}




//------------------------ Player //------------------------
bool Player::setBase(Player * players, TerrainChunk & selectedBase) {
    //base can be selected ony once
    if (base != nullptr)
        return false;
    //start field cannot be the Water
    if (selectedBase.Type == Water)
        return false;
    //terrain own to somebody
    if(selectedBase.TerrainOwner != nullptr)
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
Vect2 Player::getBaseLoc() {
    return base->Loc;
}

bool Player::SendUnits(GameWorld& world,int count, Vect2 dest) {
    Unit u;
    if (base->StcjonaryUnit.sendUnits(count, dest, &u)) {
        base->MovingUnits.push_back(u);
        base->ToUpadte();
        world.ReportEvent("Units send to " + dest.ToString(), this);
        return true;
    }
    return false;
}



