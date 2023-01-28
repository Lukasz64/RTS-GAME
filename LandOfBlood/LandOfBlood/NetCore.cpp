#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>

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
    epoll_event ee;

    uint8_t reciveBuffer[ClientReciveBuffer];
    size_t  recivedDataSize = 0;      
    size_t  expectedPacket = 0;
public:
    string cilentName = "";

    Client(int fd,NetCore * ncore) : clientDescriptor(fd),core(ncore) {
        ee.events = EPOLLIN|EPOLLRDHUP;
        ee.data = {.ptr = this};
        epoll_ctl(core->getEpoll(), EPOLL_CTL_ADD, clientDescriptor, &ee);
    }
    virtual ~Client(){
        stringstream msg;    
        msg << "connection(fd:" << clientDescriptor << ") closed";   
        ReportInfo(msg.str()); 
        
        epoll_ctl(core->getEpoll(), EPOLL_CTL_DEL, clientDescriptor, nullptr);
        shutdown(clientDescriptor, SHUT_RDWR);
        close(clientDescriptor);
    }
    int fd() const {return clientDescriptor;}
    virtual void handleEvent(uint32_t events) override {    
        if(events & EPOLLIN) {
            //char buffer[256];

            recivedDataSize += read(clientDescriptor, &reciveBuffer[recivedDataSize], !expectedPacket  ? sizeof(int) : (expectedPacket - recivedDataSize));

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
               /* cout << colorize(GREEN) << "TestSIG:";
                for (size_t i = 0; i < recivedDataSize; i++)
                printf("%X-",(int)reciveBuffer[i]);
                
                cout << endl;*/
                
                DataContainer call(reciveBuffer,expectedPacket);
                RpcCall rcall {
                    this,
                    call.GetString(0),
                    call.GetDataContainer(1)
                };

                core->clientRequests.push(rcall);
                expectedPacket = 0;
                recivedDataSize = 0;
            }
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
    //serverRequests.maxSize = 1;
}
void NetCore::InitalizeServer(){
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

    eventHandle.events = EPOLLIN;
    eventHandle.data = {.ptr = this};

    epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &eventHandle);


}
void NetCore::EpollThread(){
    //handle all eppols
    while (1)
    {
        if(-1 == epoll_wait(epollFd, &eventHandle, 1, -1)) {
            ReportError("epoll_wait fail", true, -5);
        }
        ((EpollHandler*)eventHandle.data.ptr)->handleEvent(eventHandle.events);
        ReportInfo("Epool action");
    }
}
void NetCore::MainThread(){ 
    while (1)
    {
        if(!clientRequests.isEmpty()){
            RpcCall proccsedEvent = clientRequests.pop();
            ReportInfo("Epool execution:" + proccsedEvent.rpcName);         
            
        }
        //ReportInfo("Epool execution");
        //sth is no ok ? or linux breaks
    }
    
}

void NetCore::LaunchServer(){ 
    pollThread = thread(
         [this] { 
            this->EpollThread(); 
            ReportWarning("Main tgheread closed");
        }
    );
    mainServerThread = thread(
        [this]  { 
            this->MainThread(); 
            ReportWarning("Main tgheread closed");
        }
    );
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