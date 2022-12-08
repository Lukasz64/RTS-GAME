#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>

#include "Resource.h"
#include "Utils.h"


using namespace std;

void Resource::setResource(ResourceType id, int value) {
    resource[id] = value;
}
int Resource::getResource(ResourceType id) {
    return resource[id];
}
Resource::Resource(int wood, int stone, int gold, int food) {
    setResource(Wood, wood);
    setResource(Stone, stone);
    setResource(Gold, gold);
    setResource(Food, food);
}
void Resource::PrintResources() {
    string names[] = { "Wood","Stone","Gold","Food" };
    for (size_t i = 0; i < 4; i++)
    {
        cout << colorize(i + 1) << names[i] << ":" << getResource((ResourceType)i) << colorize(NC) << endl;

    }
}