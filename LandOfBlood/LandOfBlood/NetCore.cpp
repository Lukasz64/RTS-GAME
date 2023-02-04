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
enum ClienMode{
    Unregistred = -2,
    InLobby = -1
}; 

class Client : public EpollHandler {
    int clientDescriptor;
    //integiry with server
    NetCore * core;
    //epoll object
    epoll_event ee;

    //data reciev state
    uint8_t reciveBuffer[ClientReciveBuffer];
    size_t  recivedDataSize = 0;      
    size_t  expectedPacket = 0;
public:
    string cilentName = "";
    //Unregistred need send his nick!
    //InLobby no room selected
    int    roomID = Unregistred;

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
    void SendRPC(string call,DataContainer & args){
        DataContainer rpcFrame;
        rpcFrame.PushVariable((string)call);
        rpcFrame.PushVariable(args);

        vector<uint8_t> data = rpcFrame.Serialize();
        int size = data.size();

        write((char *)&size,sizeof(size));
        write((char *)data.data(),size);
    }
    void SendRPC(string call,string arg){
        DataContainer args;
        args.PushVariable((string)arg);
        SendRPC(call,args);
    }
    void SendRPC(string call,Vect2 arg){
        DataContainer args;
        args.PushVariable(arg);
        SendRPC(call,args);
    }
    void SendRPC(string call,int arg){
        DataContainer args;
        args.PushVariable(arg);
        SendRPC(call,args);
    }
    void remove() {    
        DataContainer args;
        args.PushVariable((string)cilentName);
        args.PushVariable(roomID);
        args.PushVariable(clientDescriptor);

        RpcCall rcall {
            nullptr,// this specail casll not use 
            "Exit",
             args
        };
        core->clientRequests.push(rcall);    
        
        
        stringstream str;
        str << "Remove client " << clientDescriptor;
        ReportInfo(str.str());
        core->remClient(this);//?
        delete this;
        //delete this;
    }
    /*void ForceRemove() {
        remove();   
        delete this;
    }
    */
    /*void delteClient() {
        
    }*/
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
    serverAddr.sin_addr = {INADDR_ANY};

    if(inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0)
        ReportError("Fail to read given ip adress:" +  address,true,-1);

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
void NetCore::ProvessEvent( RpcCall & event ){
    //cout << event.rpcName << "|" << (event.rpcName == "register") <<"|"<< event.rpcName.length()<<"|"<<(int)event.rpcName[8] <<endl;
    //most frequent call is cations in clients
    if(event.rpcName == "Exit"){
       string nick = event.container.GetString(0);
       int room = event.container.GetInt(1);
       
       if(room >= 0)
         rooms[room].LeftPlayer(nick);
       return;     
    }
    
    if(event.sender->roomID >= 0){
        rooms[event.sender->roomID].ProvessEvent(event);        
        return;
    } 
    
    if(event.sender->roomID == Unregistred){
        if(event.rpcName == "register"){
            string nick = event.container.GetString(0);                    
            //empty nick not accepted
            if(nick == "")
                return;
            for (Client * i : clients)
                if(i->cilentName == nick){// kick player if tray use the same login as used on server
                    stringstream str;
                    str << "Client: " << event.sender->fd() << " try to connect with the same nick as logged user("<<nick << ")";
                    ReportWarning(str.str());
                    event.sender->SendRPC("ErrMsg", (string)"Fail to login by this name, user currentyl connected");
                    event.sender->remove(); 
                    return;
                }
            
            //succesufl refister
            event.sender->SendRPC("Login",(string)"OK");
            event.sender->roomID = InLobby;
            event.sender->cilentName = nick;
            ReportInfo("User "+ nick + " join the lobby");
        }
        return;
    }
    
    if(event.sender->roomID == InLobby){
        if(event.rpcName == "list"){
            event.sender->SendRPC("Rooms.Cnt",rooms.size());
            for (int i = 0; i < rooms.size(); i++)
            {
               DataContainer container;
               container.PushVariable(i);
               container.PushVariable((string)rooms[i].name);
               event.sender->SendRPC("Rooms.Room",container);
            }  
            return;
        }
        if(event.rpcName == "add"){
            string roomName = event.container.GetString(0);   
            if(roomName == "")
                return;
            GameRoom room(roomName);
            room.JoinPlayer(event.sender->cilentName);
            event.sender->roomID = rooms.size();
            rooms.push_back(room);
            return;
        }
        if(event.rpcName == "join"){
            int roomID = event.container.GetInt(0);   
            if(roomID >= rooms.size())
                return;
        
            if(rooms[roomID].JoinPlayer(event.sender->cilentName,event.sender));
                event.sender->roomID = roomID;
          
            return;
        }
        //time mampppig and iterface               
    }
}


void NetCore::MainThread(){ 
    while (1)
    {
        if(!clientRequests.isEmpty()){         
            RpcCall proccsedEvent = clientRequests.pop();
            ProvessEvent(proccsedEvent);           
        }
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



/*
        bool         SetPlayerBase(std::string nick, Vect2 terrLoc);
        bool         StartGame(std::string nick);

        bool         SendUints(std::string nick, int count, Vect2 terrDes);
        bool         BackToBaseUints(std::string nick, int count, Vect2 backTerritory);
        bool         UpgradeConstruction(std::string nick,int id,Vect2 teritory);

*/
// game actions
void GameRoom::ProvessEvent(RpcCall & call){
    string nick = call.sender->cilentName;
    
    if(call.rpcName == "base"){
        SetPlayerBase(nick,call.container.GetVect2(0));
    } else if(call.rpcName == "start"){
        StartGame(nick);
    } else if(call.rpcName == "send"){
        SendUints(nick,call.container.GetInt(0),call.container.GetVect2(1));
    } else if(call.rpcName == "back"){
        BackToBaseUints(nick,call.container.GetInt(0),call.container.GetVect2(1));
    } else if(call.rpcName == "upgrade"){
        UpgradeConstruction(nick,call.container.GetInt(0),call.container.GetVect2(1));
    } 
}