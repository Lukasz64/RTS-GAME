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
   Enter = 10
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
      int Structs[4]={0};
      int Resources[5]={0};
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

         if(Owner >= 0){
            sel1 = '[';
            sel2 = ']';
            tc = cc2[Owner];
         } 
         if(selected) {
            tc = MAGENTA; 
            sel1 = '}';
            sel2 = '{';
         }

         cout << colorize(tc,chc,1) << sel1 << colorize(WHITE,chc) << ' ' << colorize(tc,(chc)) << sel2 << colorize(NC);
      }

      void DrawRes(){
            COLORS cc[] = { CYAN,RED,WHITE,YELLOW,GREEN };
             for (int i = 0; i < 5; i++)
             {
                cout <<colorize(cc[i],BLACK,1)<< getResourceName(i) << ":"<< Resources[i] << "  " << colorize(NC); 
             }
      }

      void Read(DataContainer cont) {
         TerrainType = (ChunkType)cont.GetInt(0);
         Owner = cont.GetInt(1);
         Unit  = cont.GetInt(2);
         for (int i = 0; i < 4; i++)
            Structs[i] = cont.GetInt(3+i);
         for (int i = 0; i < 5; i++)
            Resources[i] = cont.GetInt(7+i);
      }
      void ReadSingle(DataContainer cont) {
         Unit  = cont.GetInt(1);
         TerrainType = (ChunkType)cont.GetInt(2);
         Owner = cont.GetInt(3);
         cout << "owner:" << Owner << endl;
         for (int i = 0; i < 4; i++)
            Structs[i] = cont.GetInt(4+i);
         for (int i = 0; i < 5; i++)
            Resources[i] = cont.GetInt(8+i);
      }

};



struct Map {
   ChunkInfo chunks[WorldSize][WorldSize];
   Vect2 plBase;
   int ownerID = -1;

   void ReadMap(DataContainer & cont){
         plBase = cont.GetVect2(0);
         ownerID = cont.GetInt(1);

         int idx = 2;
         for (int y  = 0; y < WorldSize; y++){
            for (int x  = 0; x < WorldSize; x++){
               chunks[x][y].Read(cont.GetDataContainer(idx++));
            }
         }
   }
   void RefresChunk(DataContainer & cont){
    Vect2 vect = cont.GetVect2(0);
    cout <<"Chunk udpate !"<< vect.ToString() << endl;
    chunks[vect.X][vect.Y].ReadSingle(cont);
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
        if(plBase.X == -1 && plBase.Y == -1)
            return 1;
        if(chunks[selected.X][selected.Y].Owner == -1)
            return 2;
        if(chunks[selected.X][selected.Y].Owner == ownerID)
            return 3;
        return 4;
   }
};

void GameMenu(int sockfd,SafeQueue<RpcCall> & calls){
    vector<string> servers;
    SendRPC(sockfd,"list");// request server to 
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
    }
    for (size_t i = 0; i < servers.size(); i++)
    {
        cout << colorize(GREEN) << "(" << i << ")" << colorize(WHITE) << servers[i] << endl;
    }
    cout << "Commands" << endl << "join <id>"<<endl << "new <room name>"<< endl;
    while(1){
        string command;
        cin >> command;
        if(command == "join"){
            int id =0;
            cin >> id;
            if(id < 0 || id >= servers.size()){
                ReportWarning("Wrong id");
                continue;
            }
            SendRPC(sockfd,"join",(int)id);
             while(1){
                if(!calls.isEmpty()){
                    RpcCall call = calls.pop();
                    if(call.rpcName == "STATUS"){
                        string stat = call.container.GetString(0);
                        if(stat == "OK") {
                            ReportInfo("Joinig room:"+servers[id]);
                            return;
                        }if(stat == "MapSync"){// this bcouse of hierachy of objects
                        
                        }else {
                            ReportWarning("Fail to join room");
                        }
                    } else {
                        ReportError("Critical error unexpeted answer from server");
                        break;
                    }
                }
                usleep(1000/5);
             }


        } else if(command == "new") {
            cin >> command;
            //if coonectionton not lost this is isntat connection to room(no way to fail)
            SendRPC(sockfd,"add",command);
            return;
        }
    }

}


int GameMain(string playernick,int sockfd,SafeQueue<RpcCall> & calls){
    
    SendRPC(sockfd,"register",playernick);
    GameMenu(sockfd,calls);

    Map map;
    Vect2 t(0,0);  
    
    SelfTerminalConotrol(true); 

   //keys reciving thread
    thread keyboradThtread = thread(
         [&calls] { 
            while (1)
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


    while(1){           
            while(!calls.isEmpty()){
               RpcCall call = calls.pop();
               system("clear");
               if(call.rpcName == "TCP-CONN-LOST"){
                  return -1;
               }
               
               if(call.rpcName == "MapSync"){
                  map.ReadMap(call.container);
               }
               if(call.rpcName == "Chunk"){
                  map.RefresChunk(call.container);
               }
               

           
               

               if(call.rpcName == "KeyPress"){
                  Keys code = (Keys)call.container.GetInt(0);
                  
                  if(code == Up)   t.Y--;
                  else if(code == Down) t.Y++;
                  else if(code == Left) t.X--;
                  else if(code == Right)t.X++;

                  if(t.Y < 0) t.Y = WorldSize - 1;
                  if(t.Y >= WorldSize) t.Y = 0;

                  if(t.X < 0) t.X = WorldSize - 1;
                  if(t.X >= WorldSize) t.X = 0;


                  if(code == Tylda){
                     SelfTerminalConotrol(false);
                     exit(1);
                  }

                  if(map.mode(t) == 1){
                        if(code == Enter){
                            SendRPC(sockfd,"base",t);
                        }
                  }

                  cout << code;
               }

               map.DrawResources(); cout << endl;
               cout << t.ToString()<< endl;
            
               


         
               map.DrawMap(t);
               if(map.mode(t) == 1){
                    cout << "Select base(enter)"<<endl;
               }
            }
            usleep(1000/60);
            //printf("read:%d\n",(int)in);

           //write(sockfd,data.data(),size);
           //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}