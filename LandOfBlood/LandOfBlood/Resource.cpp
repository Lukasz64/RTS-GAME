#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>

#include "Resource.h"
#include "Utils.h"


using namespace std;

Resource::Resource(int wood, int stone, int gold, int food) {
    setResource(Wood, wood);
    setResource(Stone, stone);
    setResource(Gold, gold);
    setResource(Food, food);
}
Resource::Resource(Resource & res, int mutiplier){
        for (size_t i = 0; i < 4; i++)
            resource[i] = res.resource[i] * mutiplier;
}

void Resource::setResource(ResourceType id, int value) {
    resource[id] = value;
}
int Resource::getResource(ResourceType id) {
    return resource[id];
}

void Resource::PrintResources() {
    string names[] = { "Wood","Stone","Gold","Food" };
    for (size_t i = 0; i < 4; i++)
    {
        cout << colorize(i + 1) << names[i] << ":" << getResource((ResourceType)i) << colorize(NC) << endl;

    }
}
void Resource::addResource(Resource res) {
    for (size_t i = 0; i < 4; i++)
        resource[i] += res.resource[i];
}
bool Resource::canSubstract(Resource res){
    for (size_t i = 0; i < 4; i++)
        if(resource[i] < res.resource[i])
            return false;
    return true;
}
bool Resource::subResource(Resource res){
    if(!canSubstract(res))
        return false;
    for (size_t i = 0; i < 4; i++)
        resource[i] -= res.resource[i];
    return true;
}
Resource Resource::GetDevidedCopy(int devider){
    Resource r(*this,1);
    for(int i=0;i<4;i++)
        r.resource[i] /= devider;
    return r;
}
std::string Resource::ToString() {
    stringstream str;
    str << "(W:" << getResource(Wood) << ",S:" << getResource(Stone) << ",G:"<< getResource(Gold) << ",F:"<< getResource(Food) << ")";
    return str.str();
}