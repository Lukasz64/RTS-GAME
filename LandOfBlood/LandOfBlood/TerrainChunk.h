#pragma once
#include "Resource.h"
#include "Utils.h"
//#include "GameWorld.h"

#include <list>



class GameWorld;
class Unit;
class Player;


class Unit
{
    bool    updated = false;
    Player* owner = nullptr;
    int     count = 0;
    int     delay = 0;//delay wtih reachnig next desytnation

    Vect2   currentLoc;
    Vect2   destynationLoc;

    void ProcessUnit(GameWorld& world);
    void GoNewLocation(GameWorld& world, Vect2 newLoc);

public:

    bool Add(Unit& unit);
    bool isOwner(Player* pl);
    bool sendUnits(int count, Vect2 dest, Unit* outUnit);

    void PrintUnit();


    Unit(Player* owner, int count);
    Unit(Vect2 currentL) { currentLoc = currentL; };
    Unit() {};
    ~Unit() {};



    static void ProcessUints(Vect2 terrLoc, GameWorld& world);
    static void UnlockUpadte(Vect2 terrLoc, GameWorld& world);


    //untested !!!
    static Resource CalculateCost(Vect2 statrtLoc, Vect2 destynationLoc, int units, GameWorld& world);


    static  bool  compare(const Unit& lhs,const Unit& rhs)
    {
        Vect2 curr = lhs.currentLoc;
        Vect2 curr2 = rhs.currentLoc;

        Vect2 dest = lhs.destynationLoc;
        Vect2 dest2 = rhs.destynationLoc;


        return (lhs.owner == rhs.owner && lhs.count == rhs.count && curr2.CompareValues(curr) && dest2.CompareValues(dest));
    } ;
    
private:

};




const int TypesCount = 4;
enum ChunkType
{
        Land = 0,
        Water = 1,
        Forest = 2,
        Montain = 3,
};

class TerrainChunk
{
    private:
        bool         needUpade = true;
    public:
        ChunkType    Type;
        Vect2        Loc;
        Resource     NaturalRes;
        Player* TerrainOwner = nullptr;
        Unit         StcjonaryUnit;



        std::list<Unit> MovingUnits;

        bool ToUpadte();
        void OnModifed();


        TerrainChunk(ChunkType type, Vect2 loc);
        TerrainChunk() {};
        ~TerrainChunk() {};

        void GenResoucesAsPlayerBase(Player* owner);

   
};



