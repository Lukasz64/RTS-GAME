#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string>
#include <unordered_set>
#include <thread>         // std::thread
#include "SafeQueue.h"
#include "Container.h"

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

class NetCore : public EpollHandler
{
private:
    std::unordered_set<Client*> clients;
    
    std::string address,port;
    sockaddr_in serverAddr;
    epoll_event eventHandle;

    std::thread  pollThread;
    std::thread  mainServerThread;

    //SafeEleemnt<epoll_event> event;

    int serverFd;
    int epollFd;


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






