#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <unordered_set>
#include <signal.h>
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

uint16_t readPort(string & str){
    char * ptr;
    auto port = strtol(str.c_str(), &ptr, 10);
    if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
        ReportError("Incorrect port provided: " + str);
    return port;
}

void SendRPC(int sockfd,string call,DataContainer & args){
    DataContainer rpcFrame;
    rpcFrame.PushVariable((string)call);
    rpcFrame.PushVariable(args);
   
    vector<uint8_t> data = rpcFrame.Serialize();
    int size = data.size();

    write(sockfd,&size,sizeof(size));
    write(sockfd,data.data(),size);
}
void SendRPC(int sockfd,string call){
    DataContainer args;
    SendRPC(sockfd,call,args);
}
void SendRPC(int sockfd,string call,string arg){
    DataContainer args;
    args.PushVariable((string)arg);
    SendRPC(sockfd,call,args);
}
void SendRPC(int sockfd,string call,Vect2 arg){
    DataContainer args;
    args.PushVariable(arg);
    SendRPC(sockfd,call,args);
}
void SendRPC(int sockfd,string call,int arg){
    DataContainer args;
    args.PushVariable(arg);
    SendRPC(sockfd,call,args);
}
void SendRPC(int sockfd,string call,int arg,Vect2 arg2){
    DataContainer args;
    args.PushVariable(arg);
    args.PushVariable(arg2);
    SendRPC(sockfd,call,args);
}



const int ClientReciveBuffer = 64 * 1024; //reach buffer 64KB


void ReciveThread(int fd,SafeQueue<RpcCall> & calls){
    uint8_t reciveBuffer[ClientReciveBuffer];
    size_t  recivedDataSize = 0;      
    size_t  expectedPacket = 0;
    while (1)
    {
            int recived = read(fd, &reciveBuffer[recivedDataSize], !expectedPacket  ? sizeof(int) : (expectedPacket - recivedDataSize));
            if(recived <= 0) {
                ReportWarning("Connection fail");
                RpcCall rcall {
                    "TCP-CONN-LOST"                    
                };
                calls.push(rcall);
                return;  //finies thread              
            }
            
            
            recivedDataSize += recived;

            cout << colorize( YELLOW ) << "Read:"<< recivedDataSize << endl;
            
            if(!expectedPacket && recivedDataSize >= sizeof(int)){
                recivedDataSize = 0;
                expectedPacket = *(int *)reciveBuffer;
                cout << colorize( GREEN ) << "Incomig pocket !" << endl;
                cout << colorize( YELLOW ) << "SET:"<< expectedPacket << endl;
                if(expectedPacket > ClientReciveBuffer){
                    ReportError("To lareg packet-exception");
                    expectedPacket = 0;
                }

            }  else if(expectedPacket && recivedDataSize >= expectedPacket){

                DataContainer call(reciveBuffer,expectedPacket);
                RpcCall rcall {
                    call.GetString(0),
                    call.GetDataContainer(1)
                };

                cout << colorize(GREEN) << "call->"<< rcall.rpcName << endl;

                calls.push(rcall);
                expectedPacket = 0;
                recivedDataSize = 0;
            }
    }
}
