#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    numFood =  min(5 * getLevel(), 25);
    numDirtPiles = max(180 - 20 * getLevel(), 20);
    numPits = getLevel();
    bacteriaLeft = 10*getLevel();
    socrates = nullptr;
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    bacteriaLeft = 10*getLevel();
    // adding socrates
       socrates = new Socrates(this);
    
    double randX;
    double randY;
    // adding pits
    for(int i = 0; i < getLevel(); i++){
        generateValidLocation(randX, randY, false, false);
        actors.push_back(new Pit(this, randX, randY));
    }
    // adding food
    for(int i = 0; i < numFood; i++){
        generateValidLocation(randX, randY, false, false);
        actors.push_back(new Food(this, randX, randY));
    }
    
    // adding dirt
    for(int i = 0; i < numDirtPiles; i++){
        generateValidLocation(randX, randY, false, true);
        actors.push_back(new Dirt(this, randX, randY));
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() // things that are done during one tick
{
    ostringstream oscore;
    ostringstream olevel;
    ostringstream olives;
    ostringstream ohealth;
    ostringstream osprays;
    ostringstream oflames;
    /*
     Score: 004500 Level: 4 Lives: 3 Health: 82 Sprays: 16 Flames: 4
     */
    oscore.fill('0');
    oscore << setw(6) << getScore();
    olevel << setw(2) << getLevel();
    olives << setw(1) << getLives();
    ohealth << setw(3) << socrates->getHP();
    osprays << setw(2) << socrates->getSpray();
    oflames << setw(2) << socrates->getFlame();
    
    string score = oscore.str();
    string level = olevel.str();
    string lives = olives.str();
    string health = ohealth.str();
    string sprays = osprays.str();
    string flames = oflames.str();
    
    setGameStatText("Score " + score + "  Level: " + level + "  Lives: " + lives + "  Health: " + health + "  Sprays: " + sprays + "  Flames: " + flames);
    
    socrates->doSomething();
    
    list<Actor*>::iterator it;
    for(it = actors.begin(); it != actors.end(); it++){
        Actor* cur = *it;
        if(cur->isAlive()){
            cur->doSomething();
        }
        
        if(!socrates->isAlive()){
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        
        
        if(getBacteriaLeft() == 0){
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    removeDeadActors();
    
    int chanceFungus = max(510 - getLevel() * 10, 200);
    int chanceGoodie = max(510 - getLevel() * 10, 250);
     
    int randFungus = randInt(0, chanceFungus-1);
     int randGoodie = randInt(0, chanceGoodie-1);
     double randX;
     double randY;
     int randPercentage = randInt(0, 10);
     
    if(randFungus == 0){
        generateValidLocation(randX, randY, true, false);
        actors.push_back(new Fungus(this, randX, randY));
    }
    if(randGoodie == 0){
         generateValidLocation(randX, randY, true, false);
         if(randPercentage <= 1){
             actors.push_back(new ExtraLifeGoodie(this, randX, randY));
         }
         if(randPercentage > 1 && randPercentage <= 4){
             actors.push_back(new FlamethrowerGoodie(this, randX, randY));
         }
         if(randPercentage > 4 && randPercentage <= 10){
             actors.push_back(new RestoreHealthGoodie(this, randX, randY));
         }
     }
     
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // deleting socrates
    if(socrates != nullptr){
        delete socrates;
        socrates = nullptr;
    }
    // deleting all dirt piles/other actors
    if(!actors.empty()){
        list<Actor*>::iterator it;
        for(it = actors.begin(); it != actors.end(); it++){
            delete *it;
            actors.erase(it);
        }
    }
}

void StudentWorld::removeDeadActors(){
    list<Actor*>::iterator it;
    for(it = actors.begin(); it != actors.end(); it++){
        Actor* cur = *it;
        if(!cur->isAlive()){
            delete *it;
            actors.erase(it);
        }
    }
}


bool StudentWorld::isValidLocation(double x, double y, bool isOnPerim, bool canBlockBac){
    
    list<Actor*>::iterator it;
    for(it = actors.begin(); it != actors.end(); it++){
        Actor* cur = *it;
        if(distanceFormula(x, y, cur->getX(), cur->getY()) <= SPRITE_WIDTH){
            if((!canBlockBac) || (canBlockBac && !cur->blocksBacteria())){
                return false;
            }
        }
    }
    if(isOnPerim){
        if(distanceFromCenter(x, y) == VIEW_RADIUS){
            return true;
        }
        return false;
    }
    
    if(distanceFromCenter(x, y) > 120){
        return false;
    }
    
    return true;
}

void StudentWorld::generateValidLocation(double& x, double& y, bool isOnPerim, bool canBlockBac){
    do{
        x = randInt(0, VIEW_WIDTH);
        y = randInt(0, VIEW_HEIGHT);
    } while (!isValidLocation(x, y, isOnPerim, canBlockBac));
}

double StudentWorld::distanceFormula(double x1, double y1, double x2, double y2){
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}

double StudentWorld::distanceFromCenter(double x, double y){
    return sqrt(pow(x-128, 2) + pow(y-128, 2));
}

void StudentWorld::socratesLocation(double &x, double& y){
    x = socrates->getX();
    y = socrates->getY();
}

void StudentWorld::damageSocrates(int x){
    socrates->incrementHealth(x);
    if(socrates->getHP() > 0){
        playSound(SOUND_PLAYER_HURT);
    }
    else if(socrates->getHP() <= 0){
        socrates->setDead();
        playSound(SOUND_PLAYER_DIE);
    }
}

double StudentWorld::convertToRadians(double x){
    return x*M_PI/180;
}

double StudentWorld::convertToDegrees(double x){
    return x*180/M_PI;
}


void StudentWorld::addFood(double x, double y){
    actors.push_back(new Food(this, x, y));
}

void StudentWorld::addSpray(){
    int xPos;
    int yPos;
    
    
    xPos = 128+((VIEW_WIDTH/2)*cos(convertToRadians(socrates->getDirection()+180)));
    yPos = 128+((VIEW_HEIGHT/2)*sin(convertToRadians(socrates->getDirection()+180)));
    actors.push_back(new Spray(this, xPos, yPos, socrates->getDirection()));
}

void StudentWorld::addFlame(){
    for(int i = 0; i < 16; i++){
        actors.push_back(new Flame(this, socrates->getX(), socrates->getY(), socrates->getDirection()+(i*22)));
    }
}

void StudentWorld::addSalmonella(double x, double y){
    actors.push_back(new Salmonella(this, x, y));
}

void StudentWorld::addAggressiveSalmonella(double x, double y){
    actors.push_back(new AggressiveSalmonella(this, x, y));
}

void StudentWorld::addEColi(double x, double y){
    actors.push_back(new EColi(this, x, y));
}

bool StudentWorld::checkOverlapAndDamage(double x, double y, bool damage, int howMuch){
    list<Actor*>::iterator it;
    
    for (it = actors.begin(); it != actors.end(); it++) {
        Actor* cur = *it;
        if((cur->isDamageable() || cur->hasHP()) && distanceFormula(x, y, cur->getX(), cur->getY()) <= SPRITE_WIDTH){
            if(damage && cur->isAlive()){
                if(cur->isDamageable()){
                    cur->setDead();
                }
                else if(cur->hasHP()){
                    cur->incrementHealth(-1*howMuch);
                    bool alive = cur->isAlive();
                     if(!alive){
                        cur->setDead();
                        cur->playDeathSound();
                         increaseScore(100);
                         if(randInt(1, 2) == 1){
                             addFood(cur->getX(), cur->getY());
                         }
                        bacteriaLeft--;
                    }
                     else{
                         cur->playHurtSound();
                     }
                }
            }
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkMovementOverlap(double x, double y){
    list<Actor*>::iterator it;
    for(it = actors.begin(); it != actors.end(); it++){
        Actor* cur = *it;
        if (cur->blocksBacteria() && (distanceFormula(x, y, cur->getX(), cur->getY()) <= SPRITE_WIDTH/2 || distanceFromCenter(x, y) >= VIEW_RADIUS)) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::eatFood(double x, double y){
    list<Actor*>::iterator it;
    for(it = actors.begin(); it != actors.end(); it++){
        Actor* cur = *it;
        if(cur->canBeEaten() && distanceFormula(x, y, cur->getX(), cur->getY()) <= SPRITE_WIDTH){
            cur->setDead();
            return true;
        }
    }
    return false;
}



bool StudentWorld::findClosestFood(double& x, double& y){
    list<Actor*>::iterator it;
    double minDistance = -1;
    for (it = actors.begin(); it != actors.end(); it++) {
        Actor* cur = *it;
        if(cur->canBeEaten()){
            if((minDistance == -1 || distanceFormula(x, y, cur->getX(), cur->getY()) < minDistance) && distanceFormula(x, y, cur->getX(), cur->getY()) <= 128){
                minDistance = distanceFormula(x, y, cur->getX(), cur->getY());
                x = cur->getX();
                y = cur->getY();
            }
        }
    }
    
    if(minDistance == -1){
        return false;
    }
    return true;
}
