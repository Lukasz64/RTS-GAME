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
Player * GameWorld::getPlayer(int id){
    return &players[id];
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

bool GameWorld::GetRoomState() {
    //lower rqurmients whe game in run
    if (gameRunning) {
        for (size_t x = 0; x < MaxPlayers; x++)
            if (players[x].getPlayerStatus() == Readay)//players[x].getPlayerStatus() == Readay &&
                return true;
    }
    for (size_t x = 0; x < MaxPlayers; x++)
        if (players[x].isConnected)//players[x].getPlayerStatus() == Readay &&
            return true;
    return false;
}


//
bool GameWorld::JoinPlayer(string nick,void * client)
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
            players[palyerSlot].clientIstnace = client;
            players[palyerSlot].slodID = palyerSlot;

            //PlayerUpadate(freeSlot);
            OnPlayerJoin(players[palyerSlot]);
            return true;
        }
    }
    int freeSlot = FindFreePlayerSlot();
    if (freeSlot != -1) {
        players[freeSlot] = Player(nick,client);
        players[freeSlot].isConnected = true;
        players[freeSlot].slodID = freeSlot;

        if(worldOwner == -1)
            worldOwner = freeSlot;

        //PlayerUpadate(freeSlot);
        OnPlayerJoin(players[freeSlot]);
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

bool  GameWorld::AllRedy(){
    return (CountConnected() == MaxPlayers);  
}
int  GameWorld::CountConnected(){
    int wnk=0;
    for (size_t i = 0; i < MaxPlayers; i++)
        if(players[i].getPlayerStatus() == Readay && players[i].isConnected)
            wnk++;
    return wnk;  
}
//
bool GameWorld::SetPlayerBase(string nick, Vect2 terrLoc) {
    int player = FindPlayer(nick);
    if (player != -1) {

        bool res = players[player].SetBase(players, *getChunkForUpadte(terrLoc.X, terrLoc.Y));// chunks[terrLoc.X][terrLoc.Y]

        if (res){
            OnPlayerUpadate(players[player]);
            if(AllRedy()){
                StartGame(players[worldOwner].nick);
            }    
        }

        return res;
    }
    // player not found
    return false;
}
bool GameWorld::StartGame(std::string nick) {
    int player = FindPlayer(nick);
    if (player != -1) {
        //check if owenr, if redy, and connected player more than owner
        if (worldOwner == player && players[player].getPlayerStatus() == Readay && CountConnected() >= 2) {
            ReportEvent("Game begin let win the best!");
            gameRunning = true;
            return true;
        }
    }
    // player not found
    return false;
}

void GameWorld::ReportEvent(std::string message, Player* pl) {
    if (pl != nullptr) {
        cout << colorize(YELLOW) << "To " << pl->nick << " event: " << message<< colorize(NC) << endl;
    }
    else {
        cout << colorize(GREEN) << "To all event: " << message << colorize(NC) << endl;
    }
}
void  GameWorld::ReportDay(int x){

}
void GameWorld::GameTick() {
    if (!gameRunning)
        return;

    ReportDay(day++);
    //cout << "--------------- Day:"<<day++<<"---------------" << endl;

    
    //clear cahe
    for (size_t x = 0; x < MaxPlayers; x++)
        players[x].caheUnitsCount = 0;

    for (size_t x = 0; x < WorldSize; x++)
        for (size_t y = 0; y < WorldSize; y++) {
            Unit::ProcessUints(Vect2(x,y), *this);
            chunks[x][y].TerrainTick();
            //process rest
        }

    int anyRedyPlayer = -1;
    int redyCount = 0;
    //always upadte base
    for(size_t x = 0; x < MaxPlayers; x++)
        if(players[x].getPlayerStatus() == Readay){
            redyCount++;
            
            if (anyRedyPlayer == -1)
                anyRedyPlayer = x;
            
            OnPlayerUpadate(players[x]);
            players[x].BaseUpdate();
            if (players[x].caheUnitsCount == 0) {
                ReportEvent("Player " + players[x].nick + " defeted by starve.");
                players[x].setPlayerDefeated();
                continue;
            }

     }

    if (redyCount == 1) {
        ReportEvent("Player " + players[anyRedyPlayer].nick + " win the game.");
        players[anyRedyPlayer].setPlayerDefeated();
        gameRunning = false;
        return;
    } 
    if (redyCount == 0) {
        ReportEvent("All fail the game.");
        gameRunning = false;
        return;
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
void  GameWorld::OnPlayerJoin(Player & pl){

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
        cout << "player " << pl.nick << " needs update" << " cahe ("<< pl.caheUnitsCount <<")" << endl;
}

//in future units can be send before game is running
bool GameWorld::SendUints(std::string nick,int count, Vect2 terrDes) {
    int player = FindPlayer(nick);
    if (player != -1 && players[player].getPlayerStatus() == Readay) {
        return players[player].SendUnits(*this, count, terrDes);// chunks[terrLoc.X][terrLoc.Y]
    }
    // player not found
    return false;
}

bool GameWorld::BackToBaseUints(std::string nick, int count, Vect2 backTerritory) {
    int player = FindPlayer(nick);
    if (player != -1 && players[player].getPlayerStatus() == Readay) {
        return players[player].BackUnits(*this, count, backTerritory);// chunks[terrLoc.X][terrLoc.Y]
    }
    // player not found
    return false;
}

bool  GameWorld::UpgradeConstruction(std::string nick,int id,Vect2 teritory){
    int player = FindPlayer(nick);
    if (player != -1 && players[player].getPlayerStatus() == Readay) {
        Player * pl = &players[player];
        TerrainChunk * chunk = getChunkForUpadte(teritory.X,teritory.Y);
        //not player teriory
        if(chunk->TerrainOwner != pl)
            return false;
       
        ////palyer base cannot be terated as palce to build
        if (pl->IsPlayerBase(*chunk))
            return false;

        if(chunk->Constructions[id].Upgrade(*pl->getPlayerResources())){
            OnPlayerUpadate(*pl);
            OnChunkUpadte(*chunk);
            return true;
        }

    }
    // player not found
    return false;
}

