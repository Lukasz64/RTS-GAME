#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <unordered_set>
#include <cstdio>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <vector>
#include <iostream> 
#include <fstream>
#include <cstring>
#include <map>
#include <chrono>
#include <thread>
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include "main.h"
using namespace std;

const int TypesCount = 4;
enum ChunkType
{
        Land = 0,
        Water = 1,
        Forest = 2,
        Montain = 3,
};
enum Keys {
   Tylda = 96,
   Up = 65,
   Down = 66,
   Right = 67,
   Left = 68,
   Enter = 10,
   Space = 32
};

/*
 //farm
 //mine
 //sawmill
 //willage
*/
const int WorldSize = 12;
struct ChunkInfo {
      ChunkType TerrainType = Land;
      int Owner = -1;
      int StructsLevel[4]={0};
      bool StructsActive[4]={0};
      bool StructsUpgrade[4]={0};

      int Resources[5]={0};
      //one unit
      int Unit = -1;


      string getChunkName(){
         string names[4] = {"Land","Water","Forest","Montain"};
         return names[(int)TerrainType];
      }
      string getStructName(int x){
         string names[4] = {"Farm","Mine","Sawmill","Willage"};
         return names[x];
      }
      string getResourceName(int x){
         string names[5] = { "Units","Wood","Stone","Gold","Food" };
         return names[x];
      }

      void DrawTerrain(bool selected){
         COLORS cc[] = { YELLOW,BLUE,GREEN,BLACK };
         COLORS cc2[] = { RED,CYAN,MAGENTA,WHITE };
    
         int chunk = (int)TerrainType;
         char sel1 = ' ',sel2 = ' ';

         COLORS chc = cc[chunk];
         COLORS tc;
         COLORS uc = chc;
         if(Unit >= 0){
            uc = cc2[Unit];
         }
         if(Owner >= 0){
            sel1 = '[';
            sel2 = ']';
            tc = cc2[Owner];
         } 
         if(selected) {
            tc = WHITE; 
            sel1 = '}';
            sel2 = '{';
         }

         cout << colorize(tc,chc,1) << sel1 << colorize(uc,chc) << ((Unit >= 0) ? '@' : ' ') << colorize(tc,(chc)) << sel2 << colorize(NC);
      }


      void DrawRes(){
            COLORS cc[] = { CYAN,RED,WHITE,YELLOW,GREEN };
             for (int i = 0; i < 5; i++)
             {
                cout <<colorize(cc[i],BLACK,1)<< getResourceName(i) << ":"<< Resources[i] << "  " << colorize(NC); 
             }
      }

      void Read(DataContainer cont) {
         int inc = 0;

         TerrainType = (ChunkType)cont.GetInt(inc++);
         Owner = cont.GetInt(inc++);
         Unit  = cont.GetInt(inc++);
         //cout << "owner:" << Owner << endl;
         for (int i = 0; i < 4; i++)
            StructsLevel[i] = cont.GetInt(inc++);
         for (int i = 0; i < 5; i++)
            Resources[i] = cont.GetInt(inc++);
         
         for (int i = 0; i < 4; i++){
            StructsActive[i] = cont.GetBool(inc++);
            StructsUpgrade[i] = cont.GetBool(inc++);
         }
      }
      void ReadSingle(DataContainer cont) {
         int inc = 1;

         Unit  = cont.GetInt(inc++);
         TerrainType = (ChunkType)cont.GetInt(inc++);
         Owner = cont.GetInt(inc++);
         //cout << "owner:" << Owner << endl;
         for (int i = 0; i < 4; i++)
            StructsLevel[i] = cont.GetInt(inc++);
         for (int i = 0; i < 5; i++)
            Resources[i] = cont.GetInt(inc++);

         for (int i = 0; i < 4; i++){
            StructsActive[i] = cont.GetBool(inc++);
            StructsUpgrade[i] = cont.GetBool(inc++);
         }
      }

      void DrawInfo(){
         COLORS cc[] = { CYAN,RED,WHITE,YELLOW,GREEN };  
         COLORS cc2[] = { GREEN,RED,YELLOW,WHITE };   

         MoveCursor(Vect2(37,3));
         cout << colorize(WHITE,BLACK) << "Terrain info:";
         for (int i = 0; i < 5; i++)
         {
            MoveCursor(Vect2(37,4+i));   
            cout <<colorize(cc[i],BLACK,1)<< getResourceName(i) << ":\t"<< Resources[i] << colorize(NC); 
         }

         for (int i = 0; i < 4; i++)
         {
            MoveCursor(Vect2(37,4+6+i));   
            cout <<colorize(cc2[i],BLACK,1)<< getStructName(i) << ":"<< StructsLevel[i] <<colorize(WHITE,BLACK,1) << "\t(";
            if(StructsActive[i])
               cout << colorize(GREEN,BLACK,1) << "ON";
            else
               cout << colorize(RED,BLACK,1) << "OFF";

            cout << colorize(WHITE,BLACK,1) << ",";

            if(StructsUpgrade[i])
               cout << colorize(GREEN,BLACK,1) << "CAN UPGRADE";
            else
               cout << colorize(RED,BLACK,1) << "CAN'T UPGRADE";


            cout << colorize(WHITE,BLACK,1) << ")"<< colorize(NC); 
         }  
      }
};
struct Map {
   ChunkInfo chunks[WorldSize][WorldSize];
   Vect2 plBase;
   int ownerID = -1;

   void ReadMap(DataContainer & cont){
         plBase = cont.GetVect2(0);
         ownerID = cont.GetInt(1);
         //cout << ownerID << endl;

         int idx = 2;
         for (int y  = 0; y < WorldSize; y++){
            for (int x  = 0; x < WorldSize; x++){
               chunks[x][y].Read(cont.GetDataContainer(idx++));
            }
         }
   }
   void RefresChunk(DataContainer & cont){
    Vect2 vect = cont.GetVect2(0);
    
    chunks[vect.X][vect.Y].ReadSingle(cont);
    //cout <<"Chunk udpate !"<< vect.ToString()<<"++++++"<<chunks[vect.X][vect.Y].Unit << endl;
    //must be fest
    if(chunks[vect.X][vect.Y].Owner == ownerID && mode(Vect2(0,0)) == 1){
        plBase = vect;
    }
   }

   void DrawMap(Vect2 selected){
      for (int y  = 0; y < WorldSize; y++){
         chunks[0][y].TerrainType = (ChunkType)(y % 4);
         for (int x  = 0; x < WorldSize; x++)
            chunks[x][y].DrawTerrain((selected.X == x && selected.Y == y));
         cout << endl;
      }     
   }
   void DrawResources(){
        if(mode(Vect2(0,0)) != 1){
                chunks[plBase.X][plBase.Y].DrawRes();
        }
   } 


   //select
   //explore
   //devolop
   //attack
   
   int mode(Vect2 selected){
        if(selected.CompareValues(plBase))
            return 0;
        if(plBase.X == -1 && plBase.Y == -1){
            if(chunks[selected.X][selected.Y].Owner != -1)
               return 0;
            return 1;
        }
        if(chunks[selected.X][selected.Y].Owner == -1)
            return 2;
        if(chunks[selected.X][selected.Y].Owner == ownerID)
            return 3;
        return 4;
   }
};


const int maxMessages = 5;
struct ClientGame
{
    Map map;
    Vect2 t; 
    int units = 50; 
   
    string messages[maxMessages];
    int start = 0;
    string modes[5] = {
      "--no actions--\n",
      "Select base(enter)\n",
      "Explore 50 units (enter)\n",
      "Upgrade Farm(F)\t\tUpgrade Mine(M)\nUpgrade Sawmill(S)\tUpgrade Village(V)\nget/back %d units(G/B)\n",
      "Attack %d units(enter)\n"
    };  

   void ChunkSelectorUptade(Keys code){
        if (code == Up)t.Y--;
        else if (code == Down)t.Y++;
        else if (code == Left)t.X--;
        else if (code == Right)t.X++;

        if (t.Y < 0)t.Y = WorldSize - 1;
        if (t.Y >= WorldSize)t.Y = 0;
        if (t.X < 0)t.X = WorldSize - 1;
        if (t.X >= WorldSize)t.X = 0;
   }

   void OnKey(Keys code){
        ChunkSelectorUptade(code);

        if (code == Space)
            SendRPC("start");

        if (code == Tylda)
        {
            SelfTerminalConotrol(false);
            exit(1);
        }
        int mode = map.mode(t);
        if (mode == 1)
        { // select
            if (code == Enter)
               SendRPC("base", t);
        }
        else if (mode == 2 || mode == 4)
        { // (explore/attack)
            if (code == Enter)
               SendRPC("send", units, t);
        }
        else if (mode == 3)
        { // devolop
            if (code == 'f')
               SendRPC("upgrade", 0, t);
            else if (code == 'm')
               SendRPC("upgrade", 1, t);
            else if (code == 's')
               SendRPC("upgrade", 2, t);
            else if (code == 'v')
               SendRPC("upgrade", 3, t);
            else if (code == 'g')
               SendRPC("send", units, t);
            else if (code == 'b')
               SendRPC("back", units, t);
        }

        if (code == '=')
            units += 5;
        if (code == '-' && units > 0)
            units -= 5;
   }
   
   void ProcessClientEvent(RpcCall & call){
      if(call.rpcName == "KeyPress"){
            Keys code = (Keys)call.container.GetInt(0);
            OnKey(code);
      }   
      else if(call.rpcName == "MapSync")
                  map.ReadMap(call.container);   
      else if(call.rpcName == "Chunk")
                  map.RefresChunk(call.container);            
      else if(call.rpcName == "msg"){
         string m = call.container.GetString(0);
         messages[start++] = m;
         if(start >= maxMessages)start = 0;
      }
   }
   
   void DrawGUI(){
      system("clear");
      int mode = map.mode(t);

      map.DrawResources();
      cout << endl;
      cout << t.ToString() << endl;

      map.DrawMap(t);
      printf("Exit(~)\t\t\tUints %d(+/-)\n", units);
      

      printf(modes[mode].c_str(), units, units);

      if (mode == 3)
         map.chunks[t.X][t.Y].DrawInfo();

      MoveCursor(Vect2(0, 20));
      for (size_t i = 0; i < maxMessages; i++)
      {
         int realIdx = (i + start) % maxMessages;
         if (messages[realIdx] != "")
               cout << messages[realIdx] << endl;
      }
   }
};

void ReadServerList(SafeQueue<RpcCall> & calls,vector<string>& servers){
   servers.clear();
   SendRPC("list");// request server to 
   int cnt = INT32_MAX;
    
   while(cnt){
        if(!calls.isEmpty()){
            RpcCall call = calls.pop();
            if(call.rpcName == "Rooms.Cnt"){
                cnt = call.container.GetInt(0);
            }
            if(call.rpcName == "Rooms.Room"){
                cnt--;
                servers.push_back(call.container.GetString(0));
            }
        }
        usleep(1000);
   }
}
int TryJoinRoom(SafeQueue<RpcCall> & calls,int id,vector<string>& servers){
   SendRPC("join", (int)id);
   while (isConnected){      
        if (!calls.isEmpty()){
            RpcCall call = calls.pop();
            if (call.rpcName == "STATUS"){
                string stat = call.container.GetString(0);
                if (stat == "OK"){
                   ReportInfo("Joinig room:" + servers[id]);
                   return 1;
                } else {
                   ReportWarning("Fail to join room");
                   return -1;
                }
            } else if(call.rpcName == "TCP-CONN-LOST"){    
            }else {
                ReportError("Critical error unexpeted answer from server");
                return -1;
            }
        }
        usleep(1000);
  }
  return -2;
}

void GameMenu(SafeQueue<RpcCall> & calls){
    vector<string> servers;
    
    while (isConnected){   
      
      ReadServerList(calls,servers);
      //print list
      system("clear");
      for (size_t i = 0; i < servers.size(); i++)
         cout << colorize(GREEN) << "(" << i << ")" << colorize(WHITE) << servers[i] << endl;

      cout << "Commands" << endl << "join <id>"<<endl << "new <room name>"<< endl<< "ref"<< endl;

      //
      while(isConnected){
         string command;
         cin >> command;
         if(command == "join"){
               int id =0;
               cin >> id;
               if(id < 0 || id >= servers.size()){
                  ReportWarning("Wrong id");
                  continue;
               }
               int res = TryJoinRoom(calls,id,servers);
               if(res == 1 || res == -2) return;
         } else if(command == "new") {
               cin >> command;
               //if coonectionton not lost this is isntat connection to room(no way to fail)
               SendRPC("add",command);
               return;
         } else if(command == "ref"){
            break;
         }else {
            ReportError("Unkow command: " + command);
         }
      }
   }
}

int GameMain(string playernick,int sockfd,SafeQueue<RpcCall> & calls){
    
    SendRPC("register",playernick);
    GameMenu(calls);

    SelfTerminalConotrol(true); 
    ClientGame client;

   //keys reciving thread
   thread keyboradThtread = thread(
         [&calls] { 
            while (isConnected)
            {
               char in = getchar();
               DataContainer container;
               container.PushVariable((int)in);

               RpcCall call {
                  "KeyPress",
                  container
               };
               calls.push(call);
            }           
        }
   );

    while(isConnected){           
            //int mode = map.mode(t);
            bool needUpdate = false;

            while(!calls.isEmpty()){
               RpcCall call = calls.pop();
              
               if(call.rpcName == "TCP-CONN-LOST"){
                  ReportError("Connection lost, pres any key to reconnect");
                  keyboradThtread.join();
                  return -1;
               }             
               needUpdate = true;
               client.ProcessClientEvent(call);
            }
            if(needUpdate){
               client.DrawGUI();
            }
            usleep(1000*1000/10);//10 frames
    }
    keyboradThtread.join();
    return -1;
}