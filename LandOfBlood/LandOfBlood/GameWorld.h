#pragma once
#include "Player.h"
#include "TerrainChunk.h"

#include <string>
const int WorldSize = 12;
const int MaxPlayers = 8;

const int CostFoodMul = 1;

class GameWorld
{
    private:
        TerrainChunk chunks[WorldSize][WorldSize];
        Player       players[MaxPlayers];
        int          day = 0;
        
        bool         gameRunning = false;   
        int          worldOwner = -1;
        std::string  name = "";

        int FindPlayer(std::string nick);
        int FindFreePlayerSlot();
        int FindActiveConnectedPlayer();

    public:
        GameWorld(std::string worldName,unsigned int seed = 12);
        ~GameWorld();

        TerrainChunk  getChunk(int x, int y);
        TerrainChunk* getChunkForUpadte(int x, int y, bool modfiMark = true);

        bool         JoinPlayer(std::string nick);
        bool         LeftPlayer(std::string nick);
        bool         SetPlayerBase(std::string nick, Vect2 terrLoc);
        bool         StartGame(std::string nick);

        bool         SendUints(std::string nick, int count, Vect2 terrDes);
        bool         BackToBaseUints(std::string nick, int count, Vect2 backTerritory);
        bool         UpgradeConstruction(std::string nick,int id,Vect2 teritory);
        

        //ok in use(true) or to delete(false)
        bool         GetRoomState();


        //if nullptr to all players
        void         OnChunkUpadte(TerrainChunk & chunk);
        void         PlayerUpadate(int plID);
        void         OnPlayerUpadate(Player & pl);
        void         ReportEvent(std::string message, Player* pl = nullptr);
        void         GameTick();


        void PrintMap();
        void PrintPlayes();
        void ShowGameOwner();
};

