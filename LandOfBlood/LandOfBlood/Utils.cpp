#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <errno.h>
#include <error.h>
#include <fstream>
#include <time.h>

#include "Utils.h"
using namespace std;
//#define DISABLE_COLORING
fstream logFile;

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


void InitLogs(string fileName){
    logFile.open (fileName, std::fstream::out | std::fstream::app);
}


int random(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

/**
* Colorize terminal colors ANSI escape sequences.
*
* @param font font color (-1 to 7), see COLORS enum
* @param back background color (-1 to 7), see COLORS enum
* @param style font style (1==bold, 4==underline)
**/
const string colorize(int font, int back, int style) {
    //static char code[20];
    stringstream str;


    if (font >= 0)
        font += 30;
    else
        font = 0;
    if (back >= 0)
        back += 40;
    else
        back = 0;

    if (back > 0 && style > 0) {
        str << "\033[" << font << ";" << back << ";" << style << "m";

        //sprintf(code, "\033[%d;%d;%dm", font, back, style);
    }
    else if (back > 0) {
        str << "\033[" << font << ";" << back << "m";
    }
    else {
        str << "\033[" << font << "m";
        //sprintf(code, "\033[%dm", font);
    }
    #ifdef DISABLE_COLORING
        return "";
    #else
        return str.str();
    #endif
}




void ReportError(string errmsg, bool isCritical,int retunCode) {
    logFile << "["<< currentDateTime() <<" ERROR]:" << errmsg << endl;
    cout << colorize(RED, BLACK, 1) << "[ERROR]:" << errmsg << colorize(NC) << endl;
    logFile.flush();
    if( isCritical )
        exit(retunCode);

}
void ReportWarning(string warnmsg) {
    logFile << "["<< currentDateTime() <<" WARN]:" << warnmsg << endl;
    cout << colorize(YELLOW, BLACK, 1) << "[WARN]:" << warnmsg << colorize(NC) << endl;
    logFile.flush();
}
void ReportInfo(string infomsg) {
    logFile << "["<< currentDateTime() <<" INFO]:" << infomsg << endl;
    cout << colorize(BLUE, BLACK, 1) << "[INFO]:" << infomsg << colorize(NC) << endl;
    logFile.flush();
}


bool Vect2::CompareValues(Vect2& b) {
    return (b.X == X && b.Y == Y);
}

std::string Vect2::ToString() {
    stringstream str;
    str << "[" << X << "," << Y << "]";
    return str.str();
}