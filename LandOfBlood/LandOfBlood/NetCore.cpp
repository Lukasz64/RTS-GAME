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
#include <string>
#include <sstream>
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

class Client : public EpollHandler {
    int clientDescriptor;
    NetCore * core;
public:
    Client(int fd,NetCore * ncore) : clientDescriptor(fd),core(ncore) {
        epoll_event ee {EPOLLIN|EPOLLRDHUP, {.ptr=this}};
        epoll_ctl(core->getEpoll(), EPOLL_CTL_ADD, clientDescriptor, &ee);
    }
    virtual ~Client(){
        epoll_ctl(core->getEpoll(), EPOLL_CTL_DEL, clientDescriptor, nullptr);
        shutdown(clientDescriptor, SHUT_RDWR);
        close(clientDescriptor);
    }
    int fd() const {return clientDescriptor;}
    virtual void handleEvent(uint32_t events) override {
        if(events & EPOLLIN) {
            char buffer[256];
            ssize_t count = read(clientDescriptor, buffer, 256);
            //recv(sock,buff,size,flas) 
        }
        if(events & ~EPOLLIN){
            remove();
        }
    }
    void write(char * buffer, int count){
        if(count != ::write(clientDescriptor, buffer, count))
            remove();
        
    }
    void remove() {
        stringstream str;
        str << "Remove client " << clientDescriptor;
        ReportInfo(str.str());
        core->remClient(this);
        delete this;
    }
};



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
void NetCore::handleEvent(uint32_t events){
    if(events & EPOLLIN){
            sockaddr_in clientAddr{};
            socklen_t clientAddrSize = sizeof(clientAddr);
            
            auto clientFd = accept(serverFd, (sockaddr*) &clientAddr, &clientAddrSize);
            
            if(clientFd == -1){
                ReportWarning("Accept client failed");
                return;
            }
            stringstream msg;    
            msg << "new connection from:" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "(fd:" << clientFd << ")";   
            ReportInfo(msg.str());    

            clients.insert(new Client(clientFd,this));
        }
    if(events & ~EPOLLIN){
        ReportError("Critical fail of handle EPOLL server shut dow...",true,-2);
    }
}





NetCore::~NetCore()
{
}