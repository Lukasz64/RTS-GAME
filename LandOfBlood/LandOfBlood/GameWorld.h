#pragma once
#include "Player.h"
#include "Utils.h"
#include "TerrainChunk.h"

#include <string>
const int WorldSize = 12;
const int MaxPlayers = 8;


class GameWorld
{
    private:
        TerrainChunk chunks[WorldSize][WorldSize];
        Player       players[MaxPlayers];
        int          day = 0;

        int FindPlayer(std::string nick);
        int FindFreePlayerSlot();
        int FindActiveConnectedPlayer();

    public:
        GameWorld(unsigned int seed = 12);
        ~GameWorld();

        TerrainChunk  getChunk(int x, int y);
        TerrainChunk* getChunkForUpadte(int x, int y, bool modfiMark = true);

        bool         JoinPlayer(std::string nick);
        bool         LeftPlayer(std::string nick);
        bool         SetPlayerBase(std::string nick, Vect2 terrLoc);
        bool         SendUints(std::string nick, int count, Vect2 terrDes);

        //if nullptr to all players
        void         ReportEvent(std::string message, Player* pl = nullptr);
        void         GameTick();


        void PrintMap();
        void PrintPlayes();

};

