#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <list>
#include "TerrainChunk.h"

using namespace std;

 Construction::Construction(Resource p,Resource c,int un, int up1,int up2){
    int r[3] = {1, up1, up2};
    for (size_t i = 0; i < MaxConstr; i++)
    {
        cout << "------------ const test --------------------"<< endl;
        profit[i] = Resource(p,r[i]);
        buildCosts[i] = Resource(c,r[i]);
        workUnits[i] = un * r[i];

        profit[i].PrintResources();
        buildCosts[i].PrintResources();
        cout << colorize(YELLOW) << workUnits[i] << endl;
    }

    //gold is not expected on 1 lvl of construction
    profit[0].setResource(Gold,0);
    buildCosts[0].setResource(Gold,0);

    profit[0].PrintResources();
    buildCosts[0].PrintResources();

 }
 bool Construction::CanUpgrade(Resource & res){
    if(ConstrID == MaxConstr)
        return false;
    return res.canSubstract(buildCosts[ConstrID]);
 }
 bool Construction::Upgrade(Resource & res){
    if(CanUpgrade(res) == false)
        return false;
    res.subResource(buildCosts[ConstrID]);
    ConstrID++;
    return true;
 }
 