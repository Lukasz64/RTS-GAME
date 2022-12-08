﻿// LandOfBlood.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include "Utils.h"
#include "GameWorld.h"

using namespace std;
/*
TODO list
    GameLogic:
        + Gerate Map
        + Gerate Resources
        + Player State Conotrol
        + Units Movement
        + Units Interraction
        + Supoort selecting olny changed terrains
        + Units send interraction provided(from base)
        - Units send intteraction from terrain to another
        - Proper fight handling(cannot atttack base of antoher player if any toher terory exist)
        + Proper room owner hdling when player dysconnects
        - Calaculating units move cost
        - Calcualating all units food cost by day
        - Support structures on terrains
        - Build/Upgrade strucutre(counitng cost) Inteeaction
        - Support structures actions
    Server:
        -Support client hadling (join/register/left)
        -Support variable parser(making pacckage)
        -Support rpc(remote procdres control)
        -Supoort contorl commnads
    Client:
        -Supports connectio/disconnetion form server 
        -Supports rpc like srver
        -display and upadte resources bar
        -display and upadte map (units nad structs)
        -display interface to conotrl game
    Integation:
        -registrtion protoocol(pass nick before join)
        -sendig map upadte(worki ok on srver and client)
        -sendig resource upadte(worki ok on srver and client)
        -full control player by grafical intrafve via rpc 
*/

int main()
{
    GameWorld game("Game room",123434);
    game.PrintMap();

    game.JoinPlayer("Luki");
    game.ShowGameOwner();
    game.JoinPlayer("Mati");
    game.LeftPlayer("Luki");
    //game.JoinPlayer("Mati");
    game.SetPlayerBase("Mati",Vect2(2,0));
   // game.LeftPlayer("Mati");
    game.JoinPlayer("Luki");
    game.SetPlayerBase("Luki", Vect2(3, 0));
    game.SendUints("Luki", 26, Vect2(8, 0));

    //game.JoinPlayer("Mati");
    game.PrintPlayes();

   
    game.GameTick();
    cout << game.SendUints("Mati",50,Vect2(8,0)) << endl;
    game.GameTick();
    game.GameTick();
    game.ShowGameOwner();

    for (size_t i = 0; i < 20; i++)
        game.GameTick();
    
    

    

    while (true)
    {

        string command = "";
        cout << "dbg>";
        cin >> command;
        if(command == "c"){
            int x, y;
            cin >> x >> y;
            game.getChunk(x, y).NaturalRes.PrintResources();
        } else if(command == "t"){
            int t=0;
            cin >> t;
            for (size_t i = 0; i < 20; i++)
                game.GameTick();
        } else if(command == "su"){
            string nick = "";
            int x,y,c;
            cin >> nick >> x >> y >> c;
            cout << game.SendUints(nick,c,Vect2(x,y));
        }
        else if(command == "j"){
            string nick = "";
            cout << game.JoinPlayer(nick);
        }else if(command == "l"){
            string nick = "";
            cin >> nick ;
            cout << game.JoinPlayer(nick);
        }else if(command == "sb"){
            string nick = "";
            int x,y;
            cin >> nick >> x >> y ;
            cout << game.SetPlayerBase(nick,Vect2(x,y));
        } else if(command == "p"){
        
             game.PrintPlayes();
        }

    }
}
