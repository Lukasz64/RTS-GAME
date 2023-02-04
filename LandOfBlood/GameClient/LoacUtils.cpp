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
#include <chrono>
#include <thread>
#include <termios.h>    

#include "main.h"
using namespace std;

void SelfTerminalConotrol(bool enable){
    static struct termios newt;
    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr( STDIN_FILENO, &newt);
    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    // also disavle echo 
    if(enable)
        newt.c_lflag &= ~(ICANON | ECHO); 
    else
        newt.c_lflag |= (ICANON | ECHO);          
    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
}

void LoadSettings(std::map<string, string>  & settings, std::string file){
    ifstream settingsFile(file);
    while (!settingsFile.eof())
    {
        string key,val;
        settingsFile >> key >> val;
        settings[key] = val;
        cout << colorize(YELLOW)  << key <<":" << colorize(MAGENTA) << val << colorize(NC)<< endl;
    }
}


