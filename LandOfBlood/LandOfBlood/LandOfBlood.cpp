// LandOfBlood.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <vector>
#include "Utils.h"
#include "GameWorld.h"
#include "Container.h"

using namespace std;
/*
TODO list
    GameLogic(compleated):
        + Generate Map
        + Generate Resources
        + Player State Conotrol
        + Units Movement
        + Units Interraction
        + Supoort selecting olny changed terrains
        + Units send interraction provided(from base)
        + Units send intteraction from terrain to another
        + Proper fight handling(cannot atttack base of antoher player if any toher terory exist)
        + Proper room owner hdling when player disconnects
        + Calaculating units move cost
        + Calcualating all units food cost by day
        + Support cost of structures
        + Support structures on terrains
        + Build/Upgrade strucutre(counitng cost) Interraction
        + Support structures actions
        + Support game end's 
    Server:
        +Support Container wrtie int/string/vect2
        +Support Container read int/string/vect2
        +Support Container nastyfiaction(conatiner in container)
        -Support GameLogis Thread instacing
        -Support GameLogic Queue coomuniaction
        -Support GameLogis Feedback to users
        -Support RPC(remote procedure control)
        -Support client hadling (join/register/left)
        -Support variable parser(making pacckage)
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
    
}

