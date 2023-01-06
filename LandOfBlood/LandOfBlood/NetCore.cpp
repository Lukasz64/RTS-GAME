#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <unordered_set>
#include <signal.h>
#include "Utils.h"
#include "NetCore.h"

using namespace std;


uint16_t readPort(string & str){
    char * ptr;
    auto port = strtol(str.c_str(), &ptr, 10);
    if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
        ReportError("Incorrect port provided: " + str);
    return port;
}
void setReuseAddr(int sock){
    const int one = 1;
    int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if(res) 
        ReportError("setsockopt failed",true,-1);
}



NetCore::NetCore(string & ip,string & p)
{
    port = p;
    address = ip;
}
void NetCore::LaunchServer(){
    ReportInfo("Lounch Server at "+address + ":" + port);
    uint16_t p = readPort(port);

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverFd == -1)
        ReportError("Unable to create server socket",true,-1);

    signal(SIGPIPE, SIG_IGN);

    setReuseAddr(serverFd);

    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons((short)p);
    //fix this to bind proper address
    serverAddr.sin_addr = {INADDR_ANY};

    int res = bind(serverFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if(res) 
         ReportError("Unable buind server socket",true,-1);

    res = listen(serverFd, 1);
    if(res)
        ReportError("Srver listen failed",true,-1);

    epollFd = epoll_create1(0);
}


NetCore::~NetCore()
{
}