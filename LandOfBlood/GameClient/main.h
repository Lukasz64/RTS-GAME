#include "LIBS/SafeQueue.h"
#include "LIBS/Utils.h"
#include "LIBS/Container.h"
#include <string>
#include <map>

struct RpcCall {
    std::string   rpcName;
    DataContainer container;
};

uint16_t readPort(std::string & str);
void SendRPC(std::string call,DataContainer & args);
void SendRPC(std::string call);
void SendRPC(std::string call,std::string arg);
void SendRPC(std::string call,Vect2 arg);
void SendRPC(std::string call,int arg);
void SendRPC(std::string call,int arg,Vect2 arg2);
void ReciveThread(int fd,SafeQueue<RpcCall> & calls);
void WaitUntilRedy();

void SelfTerminalConotrol(bool enable);
void MoveCursor(Vect2 vect);
void LoadSettings(std::map<std::string, std::string>  & settings, std::string file = "ClientConfig.txt");

int GameMain(std::string playernick,int sockfd,SafeQueue<RpcCall> & calls);

extern volatile bool isConnected;