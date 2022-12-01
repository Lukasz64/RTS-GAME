#pragma once
#include "Utils.h"
#include "Resource.h"

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
public:
    ChunkType Type;
    Vect2     Loc;
    Resource  NaturalRes;


    TerrainChunk(ChunkType type, Vect2 loc);
    TerrainChunk() {};


    ~TerrainChunk() {};;

private:

};

