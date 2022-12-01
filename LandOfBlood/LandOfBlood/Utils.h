#pragma once

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
