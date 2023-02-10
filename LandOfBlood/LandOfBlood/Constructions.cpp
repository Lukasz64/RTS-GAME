#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <list>
#include "TerrainChunk.h"
#include "Player.h"

using namespace std;

 Construction::Construction(int id,Resource c,Resource p,int un, int up1,int up2){
    int r[3] = {1, up1, up2};
    for (size_t i = 0; i < MaxConstr; i++)
    {
        //cout << "------------ const test --------------------"<< endl;
        profit[i] = Resource(p,r[i]);
        buildCosts[i] = Resource(c,r[i]);
        workUnits[i] = un * r[i];

        //profit[i].PrintResources();
        //buildCosts[i].PrintResources();
        //cout << colorize(YELLOW) << workUnits[i] << endl;
    }

    //gold is not expected on 1 lvl of construction
    profit[0].setResource(Gold,0);
    buildCosts[0].setResource(Gold,0);

    //profit[0].PrintResources();
    //buildCosts[0].PrintResources();

    ConstrID = id;
 }
 bool Construction::CanUpgrade(Resource & res){
    if(ConstrLvl== MaxConstr)
        return false;
    return res.canSubstract(buildCosts[ConstrLvl]);
 }
 bool Construction::Upgrade(Resource & res){
    if(CanUpgrade(res) == false)
        return false;
    res.subResource(buildCosts[ConstrLvl]);
    ConstrLvl++;
    return true;
 }

 bool Construction::Tick(TerrainChunk & chunk,int & units){
    //no construcion and action
    if(ConstrLvl == 0)
        return false;

    if(chunk.TerrainOwner == nullptr)
        return false;

    // if not plaing than skip
    if(chunk.TerrainOwner->getPlayerStatus() != Readay)
        return false;

    int unitCost = workUnits[ConstrLvl - 1]; 
    // to mouch requried
    if(unitCost > units)
        return false;
    
    Resource maxProfit = profit[ConstrLvl - 1];
    Resource prof = Resource(
        random(maxProfit.getResource(Wood)  / 2, maxProfit.getResource(Wood)),
        random(maxProfit.getResource(Stone) / 2, maxProfit.getResource(Stone)),
        random(maxProfit.getResource(Gold)  / 2, maxProfit.getResource(Gold)),
        random(maxProfit.getResource(Food)  / 2, maxProfit.getResource(Food))
    );
    //in future  changing cost in lvl
    Resource digCost = prof.GetDevidedCopy(10);

    //resources are not enought
    if(!chunk.NaturalRes.canSubstract(digCost))
        return false;
    
    chunk.NaturalRes.subResource(digCost);
    units -= unitCost;
    

    //
    if(ConstrID == 4){
        if(random(0,8) == 1){
            //this is specjal type this is willge
            Unit u(chunk.TerrainOwner,random(1,prof.getResource((ResourceType)ConstrLvl)));
            chunk.StcjonaryUnit.Add(u);
        }
    } 
      else
        chunk.TerrainOwner->getPlayerResources()->addResource(prof);

    //    units -= workUnits[ConstrLvl - 1];       
    return true;
 }
 void  Construction::PrintInfo() {
     //cout << colorize(YELLOW) << "----------------------------------" << endl;
     string str[] = { "farm","mine","sawmill","willage" };
     
     cout << colorize(GREEN) << "Name:" << colorize(WHITE) << str[ConstrID-1] << colorize(RED) << "( Lvl:"<< ConstrLvl << " )" << colorize(NC) << endl;

     //cout << colorize(YELLOW) << "----------------------------------" << colorize(NC)<< endl;

 }

 int Construction::getCurrentNededUnits(){
    if(ConstrLvl == 0)
      return 0;
      
    return workUnits[ConstrLvl - 1];
 }
 