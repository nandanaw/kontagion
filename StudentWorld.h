#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"

#include <string>
#include <list>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    int getBacteriaLeft(){ return bacteriaLeft; }
    void restoreFullHealth(){ socrates->setHealth(100); }
    void giveSocratesFlames(){ socrates->giveFlames(); }
    void incrementBacteriaLeft(int x){ bacteriaLeft += x; }
    
    double convertToRadians(double x);
    double convertToDegrees(double x);
    double distanceFormula(double x1, double y1, double x2, double y2);
    double distanceFromCenter(double x, double y);
    
    
    void socratesLocation(double& x, double& y);
    void damageSocrates(int x);
    void addFood(double x, double y);
    void addSpray();
    void addFlame();
    void addSalmonella(double x, double y);
    void addAggressiveSalmonella(double x, double y);
    void addEColi(double x, double y);
   
    
    bool checkOverlapAndDamage(double x, double y, bool damage, int howMuch);
    bool checkMovementOverlap(double x, double y);
    bool eatFood(double x, double y);
    bool findClosestFood(double& x, double& y);

    virtual ~StudentWorld();

private:
    std::list<Actor*> actors;
    Socrates* socrates;
    int numFood;
    int numDirtPiles;
    int numPits;
    int bacteriaLeft;
    
    void removeDeadActors();
    bool isValidLocation(double x, double y, bool isOnPerim, bool canBlockBac);
    void generateValidLocation(double& x, double& y, bool isOnPerim, bool canBlockBac);
    
    
    
};

#endif // STUDENTWORLD_H_
