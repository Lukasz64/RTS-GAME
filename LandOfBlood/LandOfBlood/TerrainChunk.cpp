#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>

#include "TerrainChunk.h"

TerrainChunk::TerrainChunk(ChunkType type, Vect2 loc)
{
    Type = type;
    Loc = loc;
    int DataMul = 100000;
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
