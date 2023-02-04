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



int main()
{
    SelfTerminalConotrol(true);   
    InitLogs("ClientLogs.txt");
    ReportInfo("Lounch client...");
    LoadSettings(settings);
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


    SendRPC(sockfd,"register","luki");
    SendRPC(sockfd,"add","lukland");
    SendRPC(sockfd,"add","lukland 23");
    SendRPC(sockfd,"list");
    SendRPC(sockfd,"join",(int)0);


    SafeQueue<RpcCall> calls;

    thread readThtread = thread(
         [sockfd,&calls] { 
            ReciveThread(sockfd,calls);
            ReportWarning("Main tgheread closed");
        }
    );

    //SendRPC(sockfd,"rpc-vect",Vect2(12,999));

    //DataContainer rec,box;
    //rec.PushVariable((string)"Test RPC");
    //rec.PushVariable(box);

    //DataContainer t = rec.GetDataContainer(1);

    //vector<uint8_t> data = rec.Serialize();
    //int size = data.size();

   

    /*cout << colorize(GREEN) << "TestSIG:";
    for (size_t i = 0; i < size; i++)
       printf("%X-",(int)data[i]);
    
    cout << endl;*/
    
   
    //write(sockfd,&size,sizeof(size));
    //write(sockfd,data.data(),size);
    //write(sockfd,data.data(),size);

    while(1){
            char in;
            //scanf(" %c",&in);
            in = getchar();
            printf("read:%d\n",(int)in);

           //write(sockfd,data.data(),size);
           //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}