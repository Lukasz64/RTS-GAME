#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <list>

#include "GameWorld.h"
#include "Utils.h"
#include "TerrainChunk.h"
using namespace std;


const int DataMul = 100000;


TerrainChunk::TerrainChunk(ChunkType type, Vect2 loc)
{
    Type = type;
    Loc = loc;
   
    // gerate Terrain resources
    int ReachRate = random(500, 1000) * DataMul;
    int MediumRate = random(50, 90) * DataMul;
    int LowRate = random(10, 30) * DataMul;
    int UltraLowRate = random(3, 10) * DataMul;
    int SymblicRate = random(0, 3) * DataMul;
    int Missing = 0;

    switch (type)
    {
    case Land:
        NaturalRes = Resource(
            MediumRate,    //wood 
            LowRate,       //stone 
            Missing,       //gold
            ReachRate      //food
        );
        break;
    case Water:
        NaturalRes = Resource(
            UltraLowRate,     //wood 
            LowRate,          //stone 
            SymblicRate,      //gold
            MediumRate        //food
        );
        break;
    case Forest:
        NaturalRes = Resource(
            ReachRate,       //wood 
            LowRate,         //stone 
            Missing,         //gold
            MediumRate       //food
        );
        break;
    case Montain:
        NaturalRes = Resource(
            UltraLowRate,       //wood 
            ReachRate,          //stone 
            LowRate,            //gold
            Missing             //food
        );
        break;
    default:
        break;
    }
}

void TerrainChunk::GenResoucesAsPlayerBase(Player* owner) {
    TerrainOwner = owner;
    StcjonaryUnit = Unit(owner, 100);
    NaturalRes = Resource(
        1000,       //wood 
        1000,       //stone 
        0,          //gold
        100000      //food
    );
}


bool TerrainChunk::ToUpadte() {
    bool lastState = needUpade;
    needUpade = false;
    return lastState;
}

void TerrainChunk::OnModifed(){
    needUpade = true;
}
void TerrainChunk::ConstructionsTick(){
    //ooptmize
    if(TerrainOwner == nullptr)
        return;

    //process all structres
    for (int i = 0, units = StcjonaryUnit.getCount(); i < 4 && units > 0; i++)
        if(Constructions[i].Tick(*this,units))
            needUpade = true;
}

bool operator==(const Unit& lhs, const Unit& rhs)
{
    return Unit::compare(lhs, rhs);
};
// ------------------------------------ Unit ------------------------------------


bool Unit::Add(Unit& unit) {
    if (unit.owner == owner) {
        count += unit.count;
        return true;
    }
    return false;
}

//units
Unit::Unit(Player* own, int cnt)
{
    owner = own;
    count = cnt;
    currentLoc = own->getBaseLoc();
}
bool Unit::isOwner(Player* pl) {
    return (owner == pl);
}

void Unit::GoNewLocation(GameWorld& world, Vect2 newLoc) {

    TerrainChunk* cuurrentChunk = world.getChunkForUpadte(currentLoc.X, currentLoc.Y);
    TerrainChunk* newChunk = world.getChunkForUpadte(newLoc.X, newLoc.Y);

    cuurrentChunk->MovingUnits.remove(*this);
    
    currentLoc = newLoc;
    
    if (newChunk->Type == Water)
        delay=2;
    

    //reach destynation
    if ((currentLoc.CompareValues(destynationLoc))) {
        // add units
        if (newChunk->StcjonaryUnit.Add(*this)) {
            world.ReportEvent("Units reach destination" + newChunk->Loc.ToString(), owner);
        }
        else { // explore or fight
            //exploration
            if (newChunk->StcjonaryUnit.owner == nullptr) {
                if (newChunk->Type == Water) {
                    //for now is banned
                    world.ReportEvent("Units cannot explored woter and back" + newChunk->Loc.ToString(), owner);
                    destynationLoc = owner->getBaseLoc();
                }
                else {
                    newChunk->TerrainOwner = owner;
                    newChunk->StcjonaryUnit.owner = owner;
                    newChunk->StcjonaryUnit.count = count;
                    world.ReportEvent("Units successfully explored terrain" + newChunk->Loc.ToString(), owner);
                    return;
                }
            }
            else {// fight
                //succes
                if (newChunk->StcjonaryUnit.count * 2 < count) {
                    
                    // if teriory is player base it has different rules
                    if (newChunk->TerrainOwner->IsPlayerBase(*newChunk)) {
                        //if has connt be dedefted
                        if (newChunk->TerrainOwner->HasAnyOtherTeriory(world)) {
                            destynationLoc = owner->getBaseLoc();
                            world.ReportEvent("Your base territory defended" + newChunk->Loc.ToString() + ", on attack by " + owner->nick, newChunk->TerrainOwner);
                            world.ReportEvent("Units fail take over " + newChunk->Loc.ToString() + " becouse this is player base and has another teriories", owner);
                            newChunk->MovingUnits.push_back(*this);
                            return;
                        }
                        else // player failed
                        {
                            newChunk->TerrainOwner->setPlayerDefeated();
                            world.ReportEvent("Player " + newChunk->TerrainOwner->nick + " has been defeated by " + owner->nick);
                            world.OnPlayerUpadate(*newChunk->TerrainOwner);
                        }
                    }
                    
                    world.ReportEvent("Your territory " + newChunk->Loc.ToString() + ", has been defeated by " + owner->nick, newChunk->TerrainOwner);
                    world.ReportEvent("Units take over terrain" + newChunk->Loc.ToString(), owner);

                    newChunk->TerrainOwner = owner;
                    newChunk->StcjonaryUnit.owner = owner;
                    //takig unis from other player(max half)
                    newChunk->StcjonaryUnit.count = count + random(1, newChunk->StcjonaryUnit.count / 2);
                    return;
                }
                else { // defend
                    destynationLoc = owner->getBaseLoc();
                    count = random(count / 3, count / 2);

                    world.ReportEvent("Your territory defended" + newChunk->Loc.ToString() + ", on attack by " + owner->nick, newChunk->TerrainOwner);
                    world.ReportEvent("Units fail take over terrain" + newChunk->Loc.ToString(), owner);
                }
            }

            // 
        }
    }
    newChunk->MovingUnits.push_back(*this);
}
void Unit::ProcessUnit(GameWorld& world) {
    //this unit has been upadted this ture
    if (updated)
        return;
    
    if (owner == nullptr) {
        ReportError("Unint move that don't have owner");
        return;
    }
    //don't nesesry any move
    if (currentLoc.CompareValues(destynationLoc))
        return;

    updated = true;

    //delay on move
    if (delay > 0) {
        TerrainChunk* cuurrentChunk = world.getChunkForUpadte(currentLoc.X, currentLoc.Y);
        //becouse our instace is copy we need modfi this in list
        cuurrentChunk->MovingUnits.remove(*this);
        delay--;
        cuurrentChunk->MovingUnits.push_back(*this);
        return;
    }

    if (currentLoc.X < destynationLoc.X) {
        GoNewLocation(world, Vect2(currentLoc.X + 1, currentLoc.Y));
        return;
    }
    if (currentLoc.X > destynationLoc.X) {
        GoNewLocation(world, Vect2(currentLoc.X - 1, currentLoc.Y));
        return;
    }

    if (currentLoc.Y < destynationLoc.Y){
        GoNewLocation(world, Vect2(currentLoc.X, currentLoc.Y + 1));
        return;
    }
    if (currentLoc.Y > destynationLoc.Y) {
        GoNewLocation(world, Vect2(currentLoc.X, currentLoc.Y - 1));
        return;
    }
}
void Unit::ProcessUints(Vect2 terrLoc, GameWorld& world) {
    for (Unit n : world.getChunk(terrLoc.X, terrLoc.Y).MovingUnits) {
        //cout << colorize(RED) << "[" << terrLoc.ToString() << "]" << n.owner->nick << n.currentLoc.ToString() << endl;
        n.ProcessUnit(world);
    }
}
void Unit::UnlockUpadte(Vect2 terrLoc, GameWorld& world) {
    list<Unit> units = world.getChunk(terrLoc.X, terrLoc.Y).MovingUnits;

    for (auto i = units.begin(); i != units.end(); ++i)
        i->updated = false;

    world.getChunk(terrLoc.X, terrLoc.Y).MovingUnits = units;

    TerrainChunk* r = world.getChunkForUpadte(terrLoc.X, terrLoc.Y, (units.size() != 0));
    r->MovingUnits = units;
    //if (units.size() == 0)
      //  r->ToUpadte();//reduce modfication if not nesesry


    return;
}

bool Unit::sendUnits(int c, Vect2 dest,Unit * outUnit) {
    if (count >= c) {
        *outUnit = Unit(owner, c);
        
        outUnit->currentLoc = currentLoc;
        outUnit->destynationLoc = dest;
        count -= c;
        return true;
    }
    return false;
}
/*
    mebe smoeday:
        add lower cost if teriory is own by player 
        change doom units movement
*/
Resource  Unit::CalculateCost(Vect2 statrtLoc, Vect2 destynationLoc,int units, GameWorld& world) {
    Resource cost;
    Vect2 currentLoc = statrtLoc;
    int foodMul = 2 * units, stoneMul = 20;
    //woodM  = units * 4;

    while (!currentLoc.CompareValues(destynationLoc))
    {     
        if (currentLoc.X < destynationLoc.X) {
            currentLoc = Vect2(currentLoc.X + 1, currentLoc.Y);
        } else if (currentLoc.X > destynationLoc.X) {
            currentLoc = Vect2(currentLoc.X - 1, currentLoc.Y);
        } else if (currentLoc.Y < destynationLoc.Y) {
            currentLoc = Vect2(currentLoc.X, currentLoc.Y + 1);
        }else if (currentLoc.Y > destynationLoc.Y) {
            currentLoc = Vect2(currentLoc.X, currentLoc.Y - 1);
        }
        
        TerrainChunk chunk = world.getChunk(currentLoc.X, currentLoc.Y);
        if (chunk.Type == Water)
            cost.addResource(Resource(0, stoneMul, 0, foodMul * 2));
         else{
            cost.addResource(Resource(0, stoneMul, 0, foodMul));
        }
        //world.ReportEvent("on "+ currentLoc.ToString());
    }
    return cost;
}
void Unit::PrintUnit() {
    if (owner == nullptr) {
        cout << colorize(RED) << "No owner" << colorize(NC) << endl;
        return;
    }
    cout << colorize(YELLOW) << "Owner:" << colorize(GREEN) << owner->nick << endl;
    cout << colorize(YELLOW) << "Units count:" << colorize(GREEN) << count << colorize(NC) << endl;
}
int Unit::getCount(){
    return count;
}


