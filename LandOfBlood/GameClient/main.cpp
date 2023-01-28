#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <unordered_set>
#include <signal.h>
#include <cstdio>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <vector>
#include <iostream> 
#include <fstream>
#include <cstring>
#include <map>
#include <chrono>
#include <thread>

#include "LIBS/Utils.h"
#include "LIBS/Container.h"


using namespace std;
std::map<string, string> settings = {
        { "adress", "127.0.01" },
        { "port", "1701" },
        { "nick", "luki" },
};
uint16_t readPort(string & str){
    char * ptr;
    auto port = strtol(str.c_str(), &ptr, 10);
    if(*ptr!=0 || port<1 || (port>((1<<16)-1))) 
        ReportError("Incorrect port provided: " + str);
    return port;
}

int main()
{
     InitLogs("ClientLogs.txt");
     ReportInfo("Lounch client...");

    ifstream settingsFile("ClientConfig.txt");
    while (!settingsFile.eof())
    {
        string key,val;
        settingsFile >> key >> val;
        settings[key] = val;
        cout << colorize(YELLOW)  << key <<":" << colorize(MAGENTA) << val << colorize(NC)<< endl;
    }
    ReportInfo("Load settings ok");

     //int clfd = 
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(readPort(settings["port"]));   

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        ReportError("Fail to create socket",true);

    if(inet_pton(AF_INET, settings["adress"].c_str(), &serv_addr.sin_addr) <= 0)
         ReportError("Fail to read given ip adress:" +  settings["adress"],true);
    
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
       ReportError("Fail to connect to " + settings["adress"] + ":" + settings["port"],true);

    DataContainer rec,box;
    rec.PushVariable((string)"Test RPC");
    rec.PushVariable(box);

    DataContainer t = rec.GetDataContainer(1);

    vector<uint8_t> data = rec.Serialize();
    int size = data.size();

   

    /*cout << colorize(GREEN) << "TestSIG:";
    for (size_t i = 0; i < size; i++)
       printf("%X-",(int)data[i]);
    
    cout << endl;*/
    
   
    write(sockfd,&size,sizeof(size));
    write(sockfd,data.data(),size);
  
    while(1){
           write(sockfd,data.data(),size);
           std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}