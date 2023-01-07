#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unordered_set>

class EpollHandler {
    public:
    virtual ~EpollHandler(){}
    virtual void handleEvent(uint32_t events) = 0;
};
class Client;

class NetCore : public EpollHandler
{
private:
    std::unordered_set<Client*> clients;
    
    std::string address,port;
    sockaddr_in serverAddr;
    int serverFd;
    int epollFd;
      
public:
    NetCore(std::string & ip,std::string & port);
    ~NetCore();

    int getEpoll(){ return epollFd; }
    void remClient(Client * client){ clients.erase(client);}

    void LaunchServer();
    virtual void handleEvent(uint32_t events) override;
};






