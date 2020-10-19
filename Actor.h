#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


/*     ----- A C T O R ----- */

class StudentWorld;

class Actor: public GraphObject{
public:
    Actor(StudentWorld* myWorld, int imageID, double startX, double startY, Direction dir, int depth);
    virtual ~Actor(){}
    
    virtual void doSomething() = 0;

    virtual bool isAlive(){ return m_alive; }
    virtual void playDeathSound(){ return; }
    virtual void playHurtSound(){ return; }
    virtual bool isDamageable(){ return false; }
    virtual bool hasHP(){ return false; }
    virtual bool canBeEaten(){ return false; }
    virtual bool blocksBacteria(){ return false; }
    virtual void incrementHealth(int x){ return; }
    
    void setDead(){ m_alive = false; }
    StudentWorld* getWorld(){ return m_world; }

private:
    bool m_alive;
    StudentWorld* m_world;
};

/*     -----     D A M A G E A B L E O B J E C T S      -----     */
class DamageableObjects: public Actor{
public:
    DamageableObjects(StudentWorld* myWorld, int imageID, double x, double y, Direction dir, int depth);
    virtual ~DamageableObjects(){}
    
    virtual void doSomething() = 0;
    virtual bool isDamageable(){ return true; }
};

/*     -----     H P O B J E C T S     -----     */
class HPObjects: public Actor{
public:
    HPObjects(StudentWorld* myWorld, int imageID, double x, double y, Direction dir, int depth);
    virtual ~HPObjects(){}
    
    virtual void doSomething() = 0;
    virtual bool isAlive(){ return m_hp > 0; }
    virtual bool hasHP(){ return true; }
    
    int getHP(){ return m_hp; }
    void setHealth(int x){ m_hp = x; }
    virtual void incrementHealth(int x){ m_hp += x;}
    
private:
    int m_hp;
};
/*     ----- D I R T -----     */

class Dirt: public DamageableObjects{
public:
    Dirt(StudentWorld* myWorld, double x, double y);
    virtual ~Dirt(){}

    virtual void doSomething(){ return; }
    virtual bool blocksBacteria(){ return true; }
    
    
private:
};

/* -----     S O C R A T E S     -----     */
class Socrates: public HPObjects{
public:
    Socrates(StudentWorld* myWorld);
    virtual ~Socrates(){}

    int getSpray(){ return m_spray; }
    int getFlame(){ return m_flame; }
    void giveFlames(){ m_flame += 5; }
    
    virtual void doSomething();
    

private:
    int m_spray;
    int m_flame;
};

/*     -----     G O O D I E     -----     */

class Goodie: public DamageableObjects{
public:
    Goodie(StudentWorld* myWorld, int imageID, double x, double y);
    int getLifetime(){ return m_lifetime; };
    int getTicksPassed(){ return m_ticksPassed; };
    void incrementTick(){ m_ticksPassed++; };
    virtual void doSomething();
    virtual int pointsToAdd() = 0;
    virtual void pickUp() = 0;

    virtual ~Goodie(){}
private:
    int m_lifetime;
    int m_ticksPassed;
};

/*     -----     R E S T O R E H E A L T H G O O D I E     -----     */
class RestoreHealthGoodie: public Goodie{
public:
    RestoreHealthGoodie(StudentWorld* myWorld, double x, double y);
    virtual int pointsToAdd(){ return 250; }
    virtual void pickUp();
    virtual ~RestoreHealthGoodie(){}
private:
};

/*     -----     F L A M E T H R O W E R G O O D I E     -----     */

class FlamethrowerGoodie: public Goodie{
public:
    FlamethrowerGoodie(StudentWorld* myWorld, double x, double y);
    virtual int pointsToAdd(){ return 300; }
    virtual void pickUp();
    virtual ~FlamethrowerGoodie(){}
private:
};

/*     -----     E X T R A L I F E G O O D I E     -----     */
class ExtraLifeGoodie: public Goodie{
public:
    ExtraLifeGoodie(StudentWorld* myWorld, double x, double y);
    virtual int pointsToAdd(){ return 500; }
    virtual void pickUp();
    virtual ~ExtraLifeGoodie(){}
private:
};

/*     -----     F U N G U S     -----     */
class Fungus: public Goodie{
public:
    Fungus(StudentWorld* myWorld, double x, double y);
    virtual int pointsToAdd(){ return -50; }
    virtual void pickUp();
    virtual ~Fungus(){}
private:
};

/*     -----     P R O J E C T I L E     -----     */
class Projectile: public Actor{
public:
    Projectile(StudentWorld* myWorld, int imageID, double x, double y, int dir);
    virtual void doSomething();
    virtual int getMaxDistance() = 0;
    virtual int getDamage() = 0;
    void incrementDistanceMoved(double x){ m_distanceMoved += x; }
    int getDistanceMoved(){ return m_distanceMoved; }
    virtual ~Projectile(){}
private:
    double m_distanceMoved;
};

/*     -----     S P R A Y     -----     */
class Spray: public Projectile{
public:
    Spray(StudentWorld* myWorld, double x, double y, int dir);
    virtual int getMaxDistance(){ return 112; }
    virtual int getDamage(){ return 2; }
    virtual ~Spray(){}
private:
};

/*     -----     F L A M E     -----     */
class Flame: public Projectile{
public:
    Flame(StudentWorld* myWorld, double x, double y, int dir);
    virtual int getMaxDistance(){ return 32; }
    virtual int getDamage(){ return 5; }
    virtual ~Flame(){}
private:
};

/*     -----     F O O D     -----     */
class Food: public Actor{
public:
    Food(StudentWorld* myWorld, double x, double y);
    virtual void doSomething(){ return; }
    virtual bool canBeEaten(){ return true; }
    virtual ~Food(){}
};


/*     -----     P I T     -----     */
class Pit: public Actor{
public:
    Pit(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual ~Pit(){}
private:
    int m_rSalmonella;
    int m_aSalmonella;
    int m_eColi;
};

/*     -----     B A C T E R I A     -----     */
class Bacteria: public HPObjects{
public:
    Bacteria(StudentWorld* myWorld, int imageID, double x, double y);
    virtual void doSomething();
    int getPlanDistance(){ return m_planDistance; }
    virtual int getDamage() = 0;
    void incrementPlanDistance(double x){ m_planDistance += x; }
    void setPlanDistance(double x){ m_planDistance = x; }
    int getFoodEaten(){ return m_foodEaten; }
    void setFoodEaten(int x){ m_foodEaten = x; }
    void incrementFoodEaten(int x){ m_foodEaten += x; }
    bool canMoveForward(double j);
    virtual bool findsFood(){ return true; }
    virtual bool followSocrates(){ return false; }
    virtual void add(double x, double y) = 0;
    
    virtual ~Bacteria(){}
    
private:
    int m_planDistance;
    int m_foodEaten;
};

/*     -----     S A L M O N E L L A     -----  */

class Salmonella: public Bacteria{
public:
    Salmonella(StudentWorld* myWorld, double x, double y);
   // virtual void doSomething();
    virtual void add(double x, double y);
    virtual int getDamage(){ return -1; }
    virtual void playDeathSound();
    virtual void playHurtSound();
    virtual ~Salmonella(){}
};

/*     -----     A G G R E S S I V E S A L M O N E L L A     -----  */
class AggressiveSalmonella: public Salmonella{
public:
    AggressiveSalmonella(StudentWorld* myWorld, double x, double y);
    //virtual void doSomething();
    virtual void add(double x, double y);
    virtual int getDamage(){ return -2; }
    virtual bool followSocrates(){ return true; }
    
    virtual ~AggressiveSalmonella(){}
};

/*     -----      E C O L I     -----     */
class EColi: public Bacteria{
public:
    EColi(StudentWorld* myWorld, double x, double y);
    virtual void add(double x, double y);
    virtual int getDamage(){ return -4; }
    virtual bool findsFood(){ return false; }
    virtual void playDeathSound();
    virtual void playHurtSound();

    virtual ~EColi(){}
};
#endif // ACTOR_H_
