#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>


class NetCore
{
private:
    std::string address,port;
    int serverFd;
    int epollFd;
    sockaddr_in serverAddr;
public:
    NetCore(std::string & ip,std::string & port);
    ~NetCore();

    void LaunchServer();
};






