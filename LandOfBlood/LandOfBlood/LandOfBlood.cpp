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
    GameWorld game(845945);
    game.PrintMap();

    game.JoinPlayer("Luki");
    game.JoinPlayer("Adid");
    game.LeftPlayer("Adid");
    game.JoinPlayer("Adid");
    game.SetPlayerBase("Adid",Vect2(1,0));
    game.LeftPlayer("Adid");
    game.LeftPlayer("Luki");
    game.PrintPlayes();
    while (true)
    {
        int x, y;
        cin >> x >> y;
        game.getChunk(x, y).NaturalRes.PrintResources();
    }
}

