#pragma once
#include "Resource.h"
#include "Utils.h"
//#include "GameWorld.h"

#include <list>


class TerrainChunk;
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
    bool Substract(int count);

    bool isOwner(Player* pl);
    bool sendUnits(int count, Vect2 dest, Unit* outUnit);

    void PrintUnit();


    Unit(Player* owner, int count);
    Unit(Vect2 currentL) { currentLoc = currentL; };
    Unit() {};
    ~Unit() {};
    int getCount();


    static void ProcessUints(Vect2 terrLoc, GameWorld& world);
    static void UnlockUpadte(Vect2 terrLoc, GameWorld& world);
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

const int MaxConstr = 3;

class Construction {
    private:
        Resource profit[MaxConstr];
        Resource buildCosts[MaxConstr];
        int      workUnits[MaxConstr];

    public:
        int ConstrID = 0;
        int ConstrLvl = 0;

        Construction() = default;
        Construction(int constrID,Resource cost,Resource profit,int workUnits, int upgrade1Mul,int upgrade2Mul);

        bool CanUpgrade(Resource & res);
        bool Upgrade(Resource & res);
        bool Tick(TerrainChunk & chunk, int & units);

        void PrintInfo();

        ~Construction() = default;

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
        Player     * TerrainOwner = nullptr;
        Unit         StcjonaryUnit;

        Construction Constructions[4] = {
            Construction(1,Resource(100,200,0,0),Resource(0,0,0,400),5,2,10),//farm
            Construction(2,Resource(500,100,500,1000),Resource(0,10,5,0),50,10,100),//mine
            Construction(3,Resource(200,200,20,100),Resource(20,0,0,0),10,5,20),//sawmill
            Construction(4,Resource(10000,5000,4000,10000),Resource(0,10,50,100),50,20,100)//willage
        };


        std::list<Unit> MovingUnits;

        void TerrainTick();
        bool ToUpadte();
        void OnModifed();


        TerrainChunk(ChunkType type, Vect2 loc);
        TerrainChunk() {};
        ~TerrainChunk() {};

        void GenResoucesAsPlayerBase(Player* owner);

   
};



