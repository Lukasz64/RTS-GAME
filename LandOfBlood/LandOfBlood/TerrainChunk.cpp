#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <list>

#include "GameWorld.h"
#include "Utils.h"
#include "TerrainChunk.h"
using namespace std;


const int DataMul = 100000;


TerrainChunk::TerrainChunk(ChunkType type, Vect2 loc)
{
    Type = type;
    Loc = loc;
   
    // gerate Terrain resources
    int ReachRate = random(500, 1000) * DataMul;
    int MediumRate = random(50, 90) * DataMul;
    int LowRate = random(10, 30) * DataMul;
    int UltraLowRate = random(3, 10) * DataMul;
    int SymblicRate = random(0, 3) * DataMul;
    int Missing = 0;

    switch (type)
    {
    case Land:
        NaturalRes = Resource(
            MediumRate,    //wood 
            LowRate,       //stone 
            Missing,       //gold
            ReachRate      //food
        );
        break;
    case Water:
        NaturalRes = Resource(
            UltraLowRate,     //wood 
            LowRate,          //stone 
            SymblicRate,      //gold
            MediumRate        //food
        );
        break;
    case Forest:
        NaturalRes = Resource(
            ReachRate,       //wood 
            LowRate,         //stone 
            Missing,         //gold
            MediumRate       //food
        );
        break;
    case Montain:
        NaturalRes = Resource(
            UltraLowRate,       //wood 
            ReachRate,          //stone 
            LowRate,            //gold
            Missing             //food
        );
        break;
    default:
        break;
    }
}

void TerrainChunk::GenResoucesAsPlayerBase(Player* owner) {
    TerrainOwner = owner;
    StcjonaryUnit = Unit(owner, 100);
    NaturalRes = Resource(
        1000,       //wood 
        1000,       //stone 
        0,          //gold
        10000      //food
    );
}


bool TerrainChunk::ToUpadte() {
    bool lastState = needUpade;
    needUpade = false;
    return lastState;
}

void TerrainChunk::OnModifed(){
    needUpade = true;
}
void TerrainChunk::TerrainTick(){
    //ooptmize
    if(TerrainOwner == nullptr || TerrainOwner->getPlayerStatus() != Readay)
        return;
  

    //additonal security chcek(game integierity)
    if (TerrainOwner->IsPlayerBase(*this) == false) {
        //process all structres
        for (int i = 0, units = StcjonaryUnit.getCount(); i < 4 && units > 0; i++)
            if (Constructions[i].Tick(*this, units))
                needUpade = true;
    }
    int cost = StcjonaryUnit.getCount() * CostFoodMul;
    if (TerrainOwner->getPlayerResources()->subResource(Resource(0, 0, 0, cost)) == false) {
        // we cant feed all units
        // eleimente form 0 to 5 units
        StcjonaryUnit.Substract(random(0, 5));
    }
    TerrainOwner->CaheUnitsCount += StcjonaryUnit.getCount();
}


