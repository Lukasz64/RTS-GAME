#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <unordered_set>
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
#include <termios.h>            //termios, TCSANOW, ECHO, ICANON
#include "main.h"

using namespace std;
std::map<string, string> settings = {
        { "adress", "127.0.01" },
        { "port", "1701" },
        { "nick", "luki" },
};

int InitConnetion(){
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(readPort(settings["port"]));   

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
       ReportError("Fail to create socket",true);

    if(inet_pton(AF_INET, settings["adress"].c_str(), &serv_addr.sin_addr) <= 0)
       ReportError("Fail to read given ip adress:" +  settings["adress"],true);  

   if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
         ReportError("Fail to connect to " + settings["adress"] + ":" + settings["port"]); 
         return -1;
    }

    return sockfd;
}

int main()
{
    SelfTerminalConotrol(false);   
    InitLogs("ClientLogs.txt");
    ReportInfo("Lounch client...");
    LoadSettings(settings);
    ReportInfo("Load settings ok");

    SafeQueue<RpcCall> calls;

    cout << "Enter Nick:";
    cin >> settings["nick"];

      
    while(1){       
      int sockfd = InitConnetion();

      //if fail recconeting until sucess
      //connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)
      if( sockfd < 0){
         ReportInfo("Try reconnect ...");
         sleep(5);
         continue;
      }
      
      //simple clear queue
      while (!calls.isEmpty())
         calls.pop();

      //rpc recivng thread
      thread readThtread = thread(
            [sockfd,&calls] { 
               ReciveThread(sockfd,calls);
               ReportWarning("rpc thread closed");
         }
      ); 
      if(WaitUntilRedy() == false){
         readThtread.join();
         continue;
      }

      //game main
      GameMain(settings["nick"],sockfd,calls);

      SelfTerminalConotrol(false);
      shutdown(sockfd,SHUT_RDWR);
      close(sockfd);

      readThtread.join();
    }

}