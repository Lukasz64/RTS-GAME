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
void SendRPC(int sockfd,std::string call,DataContainer & args);
void SendRPC(int sockfd,std::string call);
void SendRPC(int sockfd,std::string call,std::string arg);
void SendRPC(int sockfd,std::string call,Vect2 arg);
void SendRPC(int sockfd,std::string call,int arg);
void ReciveThread(int fd,SafeQueue<RpcCall> & calls);


void SelfTerminalConotrol(bool enable);
void LoadSettings(std::map<std::string, std::string>  & settings, std::string file = "ClientConfig.txt");