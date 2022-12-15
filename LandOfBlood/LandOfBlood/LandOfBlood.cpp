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
        -Support Container wrtie int/string/vect2
        -Support Container read int/string/vect2
        -Support Container nastyfiaction(conatiner in container)
        -Support GameLogis Thread instacing
        -Support GameLogic Queue coomuniaction
        -Support GameLogis Feedback to users
        -Support RPC(remote procedure control)
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

enum ContainerType{
    BOOL_ = 0,
    INT_ = 1,
    STRING_ = 2,
    VECT2_ = 3,
    CONTAINER_ = 4
};

struct ContainerRecord
{
    ContainerType type;
    int           lenght;  
    void         *data;
};


class DataContainer
{
private:
    bool modfied = false;
    vector<ContainerRecord> recordContainer;
    vector<uint8_t> dataContainer;
    vector<uint8_t> binaryContainer;

    void addData(ContainerType type,void *data,int lenght);
    void writeBinary(vector<uint8_t> & outBuffer,void *data,int lenght);
public:
    DataContainer(/* args */) = default;
    ~DataContainer() = default;

    void PushVariable(bool var);
    void PushVariable(int var);
    void PushVariable(string var);
    void PushVariable(Vect2 var);
};
void DataContainer::writeBinary(vector<uint8_t> & outBuffer,void *data,int lenght){
    for (size_t i = 0; i < lenght; i++)
        outBuffer.push_back(((uint8_t*)data)[i]);
}
void DataContainer::addData(ContainerType type,void *data,int lenght){
    ContainerRecord rec{type,lenght,(void *)dataContainer.size()};
    recordContainer.push_back(rec);
    writeBinary(binaryContainer,data,lenght);
}
void DataContainer::PushVariable(bool var){
    addData(BOOL_,&var,1);
}
void DataContainer::PushVariable(int var){
    addData(INT_,&var,sizeof(var));
}
void DataContainer::PushVariable(string var){
    addData(STRING_,&var[0],var.length());
}
void DataContainer::PushVariable(Vect2 var){
    addData(VECT2_,&var,sizeof(var));
}





int main()
{
    GameWorld game("Game room",987654321);
    Construction sonstr(1,Resource(1,1,1,1),Resource(2,2,4,4),10,2,5);
    srand(time(NULL));

    game.PrintMap();

    game.JoinPlayer("Luki");
    game.ShowGameOwner();
    game.JoinPlayer("Mati");
    game.LeftPlayer("Luki");
    //game.JoinPlayer("Mati");
    game.SetPlayerBase("Mati",Vect2(2,0));

    cout << game.StartGame("Mati") << endl;

   // game.LeftPlayer("Mati");
    game.JoinPlayer("Luki");
    game.SetPlayerBase("Luki", Vect2(3, 0));
    game.SendUints("Luki", 30, Vect2(8, 0));
    game.SendUints("Luki", 30, Vect2(8, 2));

    //game.JoinPlayer("Mati");
    game.PrintPlayes();

   
    game.GameTick();
    
   // cout << game.SendUints("Mati",50,Vect2(4,0)) << endl;
    
    game.GameTick();
    game.GameTick();
    game.ShowGameOwner();
   
    for (size_t i = 0; i < 20; i++)
        game.GameTick();

    
    cout << game.SendUints("Mati", 100, Vect2(3, 1)) << endl;
    cout << game.BackToBaseUints("Mati", 50, Vect2(4, 0)) << endl;
    for (size_t i = 0; i < 20; i++)
        game.GameTick();
    
    
    Resource res(100, 20, 300, 3000);

    cout << res.canSubstract(Resource(200,0,0,0)) << endl;
    cout << res.canSubstract(Resource(100,0,0,0)) << endl;
    cout << res.canSubstract(Resource(100, 20, 300, 3000)) << endl;
    cout << res.canSubstract(Resource(50, 10, 11, 1000)) << endl;
    cout << res.canSubstract(Resource(50, 555, 11, 1000)) << endl;
    cout << res.canSubstract(Resource(50, 555, 111, 1000)) << endl;
    cout << res.canSubstract(Resource(50, 555, 111, 11000)) << endl;


    cout << res.subResource(Resource(50, 19, 299, 3000)) << endl;
    cout << res.canSubstract(Resource(50, 2, 1, 0)) << endl;
    cout << res.canSubstract(Resource(51, 0, 0, 0)) << endl;


    while (true)
    {

        string command = "";
        cout << "dbg>";
        cin >> command;
        if(command == "chunk"){
            int x, y;
            cin >> x >> y;
            TerrainChunk chunk = game.getChunk(x, y);
            chunk.NaturalRes.PrintResources();
            chunk.StcjonaryUnit.PrintUnit();
            cout << colorize(YELLOW) << "----------------------------------" << endl;
            for (size_t i = 0; i < 4; i++)
                chunk.Constructions[i].PrintInfo();
            cout << colorize(YELLOW) << "----------------------------------" << colorize(NC) << endl;

        } else if(command == "tick"){
            int t=0;
            cin >> t;
            for (size_t i = 0; i < t; i++)
                game.GameTick();
        } else if(command == "send"){
            string nick = "";
            int x,y,c;
            cin >> nick >> x >> y >> c;
            cout << game.SendUints(nick,c,Vect2(x,y)) << endl;
        }
        else if (command == "back") {
            string nick = "";
            int x, y, c;
            cin >> nick >> x >> y >> c;
            cout << game.BackToBaseUints(nick, c, Vect2(x, y)) << endl;
        }
        else if(command == "join"){
            string nick = "";
            cin >> nick;
            cout << game.JoinPlayer(nick) << endl;
        }else if(command == "left"){
            string nick = "";
            cin >> nick ;
            cout << game.LeftPlayer(nick);
        }else if(command == "base"){
            string nick = "";
            int x,y;
            cin >> nick >> x >> y ;
            cout << game.SetPlayerBase(nick, Vect2(x, y)) << endl;
        }
        else if (command == "up") {
            string nick = "";
            int x, y, c;
            cin >> nick >> x >> y >> c;
            cout << game.UpgradeConstruction(nick,c, Vect2(x, y)) << endl;
        }
        else if (command == "player") {
            game.PrintPlayes();
        }
        else if (command == "map") {
            game.PrintMap();
        }
        else if (command == "trace") {
            int x1,x2,y1,y2,c;
            cin >> x1>>y1 >> x2 >> y2 >> c;
            Unit::CalculateCost(Vect2(x1,y1),Vect2(x2,y2),c,game).PrintResources();
            //game.PrintMap();
        }
        else if (command == "help") {
            cout << "chunk x y" << endl;
            cout << "tick n" << endl;
            cout << "send nick x y count" << endl;
            cout << "join nick" << endl;
            cout << "left nick" << endl;
            cout << "base nick x y" << endl;
            cout << "player" << endl;
            cout << "map" << endl;
            cout << "trace x1 y1 x2 y2 count" << endl;
            cout << "up nick x y id" << endl;
        }

    }
}

