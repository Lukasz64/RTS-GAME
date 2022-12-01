#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include "Utils.h"
using namespace std;


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

    return str.str();
}