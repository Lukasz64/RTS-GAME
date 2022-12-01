// LandOfBlood.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include "Utils.h"
#include "GameWorld.h"

using namespace std;



int main()
{
    GameWorld game(32345);
    game.PrintMap();

    game.JoinPlayer("Luki");
    game.JoinPlayer("Adid");
    game.LeftPlayer("Adid");
    game.PrintPlayes();
    while (true)
    {
        int x, y;
        cin >> x >> y;
        game.getChunk(x, y).NaturalRes.PrintResources();
    }
}

