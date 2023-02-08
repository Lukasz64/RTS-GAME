#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <thread>         // std::thread
#include "SafeQueue.h"
#include "Container.h"
#include "GameWorld.h"


const int ClientReciveBuffer = 8 * 1024; //reach buffer 8KB




class EpollHandler {
    public:
    virtual ~EpollHandler(){}
    virtual void handleEvent(uint32_t events) = 0;
    
};
class Client;

struct RpcCall {
    Client *      sender;
    std::string   rpcName;
    DataContainer container;
};


class GameRoom : public GameWorld {
    public:
    GameRoom(std::string name) : GameWorld(name,rand()){}
    void ProvessEvent(RpcCall & call);
    void OnPlayerUpadate(Player & pl) override;
    void OnChunkUpadte(TerrainChunk & chunk) override; 
    void OnPlayerJoin(Player & pl)override;
    //void         OnChunkUpadte(TerrainChunk & chunk);
};

class NetCore : public EpollHandler
{
private:
    std::unordered_set<Client*> clients;
    std::vector<GameRoom> rooms;
    
    std::string address,port;
    sockaddr_in serverAddr;
    epoll_event eventHandle;

    std::thread  pollThread;
    std::thread  mainServerThread;

    //SafeEleemnt<epoll_event> event;

    int serverFd;
    int epollFd;

    void ProvessEvent(RpcCall & event );

public:

    SafeQueue<RpcCall> clientRequests;

    NetCore(std::string & ip,std::string & port);
    ~NetCore();

    void EpollThread();
    void MainThread();   

    int getEpoll(){ return epollFd; }
    void remClient(Client * client){ clients.erase(client);}

    void InitalizeServer();
    void LaunchServer();
    virtual void handleEvent(uint32_t events) override;
};






