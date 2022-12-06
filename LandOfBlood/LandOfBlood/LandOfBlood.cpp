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
    GameWorld game(123434);
    game.PrintMap();

    game.JoinPlayer("Luki");
    game.JoinPlayer("Mati");
    game.LeftPlayer("Mati");
    game.JoinPlayer("Mati");
    game.SetPlayerBase("Mati",Vect2(2,0));
    game.LeftPlayer("Mati");
    //game.LeftPlayer("Luki");
    game.SetPlayerBase("Luki", Vect2(3, 0));
    game.SendUints("Luki", 26, Vect2(8, 0));

    game.JoinPlayer("Mati");
    game.PrintPlayes();

   
    game.GameTick();
    cout << game.SendUints("Mati",50,Vect2(8,0)) << endl;
    game.GameTick();
    game.GameTick();

    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();
    game.GameTick();



    while (true)
    {
        int x, y;
        cin >> x >> y;
        game.getChunk(x, y).NaturalRes.PrintResources();
    }
}

