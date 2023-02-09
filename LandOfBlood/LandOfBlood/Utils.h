#ifndef UTILS_H // include guard
#define UTILS_H

#include <string>
int random(int min, int max);

struct Vect2
{
        int X;
        int Y;
        Vect2() {
            X = 0;
            Y = 0;
        }
        Vect2(int x, int y) {
            X = x;
            Y = y;
        }
        bool CompareValues(Vect2& b);
        std::string ToString();
 };


enum COLORS {
        NC = -1,
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
 };

const std::string colorize(int font, int back = -1, int style = -1);

void InitLogs(std::string fileName);
void ReportError(std::string errmsg, bool isCritical = false,int retunCode = -1);
void ReportWarning(std::string warnmsg);
void ReportInfo(std::string infomsg);

#endif