#include "Actor.h"
#include "StudentWorld.h"

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

/*     -----     A C T O R     -----     */

Actor::Actor(StudentWorld* myWorld, int imageID, double startX, double startY, Direction dir, int depth)
: GraphObject(imageID, startX, startY, dir, depth)
{
    m_alive = true;
    m_world = myWorld;
}

DamageableObjects::DamageableObjects(StudentWorld* myWorld, int imageID, double x, double y, Direction dir, int depth)
: Actor(myWorld, imageID, x, y, dir, depth)
{}

HPObjects::HPObjects(StudentWorld* myWorld, int imageID, double x, double y, Direction dir, int depth)
: Actor(myWorld, imageID, x, y, dir, depth)
{}


/*     -----     S O C R A T E S     -----     */

Socrates::Socrates(StudentWorld* myWorld)
: HPObjects(myWorld, IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 0)
{
    setHealth(100);
    m_spray = 20;
    m_flame = 5;
}

void Socrates::doSomething(){
   
    int moveX;
    int moveY;
    int positionalAngle;
    
    if(!isAlive()){
        return;
    }
    
    int ch = KEY_PRESS_LEFT;
    if(getWorld()->getKey(ch)){
        switch (ch) {
            case KEY_PRESS_LEFT:
                // move counterclockwise
                positionalAngle = getDirection() + 185;
                setDirection(positionalAngle); // (128, 185) --> new polar angle
                moveX = 128+((VIEW_WIDTH/2) * cos(getWorld()->convertToRadians(positionalAngle)));
                moveY = 128+((VIEW_HEIGHT/2) * sin(getWorld()->convertToRadians(positionalAngle)));
                
                moveTo(moveX, moveY);
                setDirection(positionalAngle+180);
                break;
                
            case KEY_PRESS_RIGHT:
                // move clockwise
                positionalAngle = getDirection() + 175;
                setDirection(positionalAngle);
                moveX = 128+((VIEW_WIDTH/2) * cos(getWorld()->convertToRadians(positionalAngle)));
                moveY = 128+((VIEW_HEIGHT/2) * sin(getWorld()->convertToRadians(positionalAngle)));

                moveTo(moveX, moveY);
                setDirection(positionalAngle+180);
                break;
                
            case KEY_PRESS_SPACE:
                if (m_spray > 1) {
                    getWorld()->addSpray();
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    m_spray--;
                }
                break;
                
            case KEY_PRESS_ENTER:
                if(m_flame > 1){
                    getWorld()->addFlame();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_flame--;
                }
                break;
                
            default:
                break;
        }
    }
    else{
        if(m_spray < 20){
            m_spray++;
        }
    }
}


/*     -----     D I R T      -----     */

Dirt::Dirt(StudentWorld* myWorld, double x, double y)
: DamageableObjects(myWorld, IID_DIRT, x, y, 0, 1)
{}



/*     -----     G O O D I E     -----     */
Goodie::Goodie(StudentWorld* myWorld, int imageID, double x, double y)
: DamageableObjects(myWorld, imageID, x, y, 0, 1)
{
    m_lifetime = max(randInt(0, 300 - 10 * myWorld->getLevel() - 1), 50);
    m_ticksPassed = 0;
}

void Goodie::doSomething(){
    if(!isAlive()){
        return;
    }
    
    double sx;
    double sy;
    getWorld()->socratesLocation(sx, sy);
    if(getWorld()->distanceFormula(getX(), getY(), sx, sy) <= SPRITE_WIDTH){
        getWorld()->increaseScore(pointsToAdd());
        setDead();
        if(pointsToAdd() > 0){
            getWorld()->playSound(SOUND_GOT_GOODIE);
        }
        pickUp();
        return;
    }
    
    if(getTicksPassed() > getLifetime()){
        setDead();
    }
    
    incrementTick();
}


/*     -----     R E S T O R E H E A L T H G O O D I E     -----     */
RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* myWorld, double x, double y)
: Goodie(myWorld, IID_RESTORE_HEALTH_GOODIE, x, y)
{}

void RestoreHealthGoodie::pickUp(){
     getWorld()->restoreFullHealth();
}

/*     -----     F L A M E T H R O W E R G O O D I E     -----     */
FlamethrowerGoodie::FlamethrowerGoodie(StudentWorld* myWorld, double x, double y)
:Goodie(myWorld, IID_FLAME_THROWER_GOODIE, x, y)
{}

void FlamethrowerGoodie::pickUp(){
    getWorld()->giveSocratesFlames();
}

/*     -----     E X T R A L I F E G O O D I E     -----     */
ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* myWorld, double x, double y)
:Goodie(myWorld, IID_EXTRA_LIFE_GOODIE, x, y)
{}

void ExtraLifeGoodie::pickUp(){
    getWorld()->incLives();
}

/*     -----     F U N G U S     -----     */
Fungus::Fungus(StudentWorld* myWorld, double x, double y)
:Goodie(myWorld, IID_FUNGUS, x, y)
{}

void Fungus::pickUp(){
    getWorld()->damageSocrates(-20);
}

/*     -----     P R O J E C T I L E     -----     */
Projectile::Projectile(StudentWorld* myWorld, int imageID, double x, double y, int dir)
:Actor(myWorld, imageID, x, y, dir, 1)
{
    m_distanceMoved = 0;
}

void Projectile::doSomething(){
    if(!isAlive()){
        return ;
    }
    if(getWorld()->checkOverlapAndDamage(getX(), getY(), true, getDamage())){
        setDead();
        return;
    }
    moveAngle(getDirection(), SPRITE_WIDTH);
    m_distanceMoved += SPRITE_WIDTH;
    if(m_distanceMoved > getMaxDistance()){
        setDead();
        
    }
}

/*     -----     S P R A Y     -----     */
Spray::Spray(StudentWorld* myWorld, double x, double y, int dir)
:Projectile(myWorld, IID_SPRAY, x, y, dir)
{}


/*     -----     F L A M E     -----     */
Flame::Flame(StudentWorld* myWorld, double x, double y, int dir)
:Projectile(myWorld, IID_FLAME, x, y, dir)
{}

/*     -----     F O O D     -----     */
Food::Food(StudentWorld* myWorld, double x, double y)
:Actor(myWorld, IID_FOOD, x, y, 90, 1)
{}

/*     -----     P I T     -----     */
Pit::Pit(StudentWorld* myWorld, double x, double y)
:Actor(myWorld, IID_PIT, x, y, 0, 1)
{
    m_rSalmonella = 5;
    m_aSalmonella = 3;
    m_eColi = 2;
}

void Pit::doSomething(){
    if(m_eColi == 0 && m_rSalmonella == 0 && m_aSalmonella == 0){
        setDead();
        
    }
    else{
        int rand = randInt(1, 50);
        if(rand == 1){
            
            bool bacteriaHasNotBeenAdded = true;
            while (bacteriaHasNotBeenAdded) {
                int randBacteria = randInt(1, 3);
                if(randBacteria == 1 && m_eColi > 0){
                    getWorld()->addEColi(getX(), getY());
                    m_eColi--;
                    getWorld()->playSound(SOUND_BACTERIUM_BORN);
                    bacteriaHasNotBeenAdded = false;
                    break;
                }
                if(randBacteria == 2 && m_aSalmonella > 0){
                    getWorld()->addAggressiveSalmonella(getX(), getY());
                    m_aSalmonella--;
                    getWorld()->playSound(SOUND_BACTERIUM_BORN);
                    bacteriaHasNotBeenAdded = false;
                    break;
                }
                if(randBacteria == 3 && m_rSalmonella > 0){
                    getWorld()->addSalmonella(getX(), getY());
                    m_rSalmonella--;
                    getWorld()->playSound(SOUND_BACTERIUM_BORN);
                    bacteriaHasNotBeenAdded = false;
                    break;
                }
                
                if(m_eColi == 0 && m_rSalmonella == 0 && m_aSalmonella == 0){
                    bacteriaHasNotBeenAdded = false;
                }
            }
        }
    }
}

/*     -----      B A C T E R I A     -----      */
Bacteria::Bacteria(StudentWorld* myWorld, int imageID, double x, double y)
:HPObjects(myWorld, imageID, x, y, 90, 0)
{
    m_planDistance = 0;
    m_foodEaten = 0;
}

bool Bacteria::canMoveForward(double j){
    double xMove;
    double yMove;
    for(int i = 1; i <= j; i++){
        xMove = i*cos(getWorld()->convertToRadians(getDirection()));
        yMove = i*sin(getWorld()->convertToRadians(getDirection()));
        
        if(getWorld()->checkMovementOverlap(getX()+xMove, getY()+yMove)){
            return false;
        }
    }
    return true;
}

void Bacteria::doSomething(){
     if(!isAlive()){
        return;
    }
    bool here = false;
    
    // STEP 2 --> UNIQUE TO AGGRESSIVE
    double sx;
    double sy;
    getWorld()->socratesLocation(sx, sy);
    
    if(followSocrates()){
        if(getWorld()->distanceFormula(getX(), getY(), sx, sy) <= 72){
            here = true;
            
            double opp = sy - getY();
            double adj = sx - getX();
            
            double socDir = getWorld()->convertToDegrees(atan2(opp, adj));
            setDirection(socDir);
            
            if(canMoveForward(3)){
                moveAngle(getDirection(), 3);
            }
        }
    }
    
    // STEP 3
    
    if(getWorld()->distanceFormula(getX(), getY(), sx, sy) <= SPRITE_WIDTH){
        getWorld()->damageSocrates(getDamage());
        if(here) { return; }
    }
    
    // STEP 4
    else if(getFoodEaten() == 3){
        double newX = getX();
        double newY = getY();
    
        if(getX() < VIEW_WIDTH/2){
            newX += SPRITE_WIDTH/2;
        }
        else if(getX() > VIEW_WIDTH/2){
            newX -= SPRITE_WIDTH/2;
        }
        
        if(getY() < VIEW_HEIGHT/2){
            newY += SPRITE_WIDTH/2;
        }
        else if(getY() > VIEW_HEIGHT/2){
            newY -= SPRITE_WIDTH/2;
        }
        
        add(newX, newY);
        getWorld()->incrementBacteriaLeft(1);
        setFoodEaten(0);
        
        if(here) { return; }
    }
    
    // STEP 5
    else{
        if(getWorld()->eatFood(getX(), getY())){
            incrementFoodEaten(1);
        }
        if(here){ return; }
    }
    
    getWorld()->socratesLocation(sx, sy);
    double x = getX();
    double y = getY();
    double distanceToSocrates = getWorld()->distanceFormula(x, y, sx, sy);
    if((!findsFood()) && distanceToSocrates <= 256){
        
        double opp = sy - getY();
        double adj = sx - getX();
        
        double theta = getWorld()->convertToDegrees(atan2(opp, adj));
        setDirection(theta);
        
        for(int i = 0; i < 10; i++){
            if(canMoveForward(2)){
                moveAngle(theta, 2);
                return;
            }
            theta += 10;
            if(theta >= 360){
                theta -= 360;
            }
        }
    }
    // STEP 6
    if(getPlanDistance() > 0){
        incrementPlanDistance(-1);
        if(!canMoveForward(3)){
            int rand = randInt(0, 359);
            setDirection(rand);
            setPlanDistance(10);
            return;
        }
        else{
            moveAngle(getDirection(), 3);
        }
    }
    
    // STEP 7
    else if(findsFood()){
        double xFood = -1;
        double yFood = -1;
        if(!getWorld()->findClosestFood(xFood, yFood)){
            setDirection(randInt(0, 359));
            setPlanDistance(10);
            return;
        }
        else{
            double opp = yFood - getY();
            double adj = xFood - getX();
            
            double foodDir = getWorld()->convertToDegrees(atan2(opp, adj));
            setDirection(foodDir);
            
            if(!canMoveForward(3)){
                setDirection(randInt(0, 359));
                setPlanDistance(10);
                return;
            }
            else{
                moveAngle(getDirection(), 3);
            }
        }
    }
}

/*     -----     S A L M O N E L L A     -----     */

Salmonella::Salmonella(StudentWorld* myWorld, double x, double y)
: Bacteria(myWorld, IID_SALMONELLA, x, y)
{
    setHealth(4);
}

void Salmonella::add(double x, double y){
    getWorld()->addSalmonella(x, y);
}


void Salmonella::playDeathSound(){
    getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

void Salmonella::playHurtSound(){
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

/*     -----     A G G R E S S I V E S A L M O N E L L A     -----     */

AggressiveSalmonella::AggressiveSalmonella(StudentWorld* myWorld, double x, double y)
: Salmonella(myWorld, x, y)
{
    setHealth(10);
}

void AggressiveSalmonella::add(double x, double y){
    getWorld()->addAggressiveSalmonella(x, y);
}


/*     -----     E C O L I      -----     */
EColi::EColi(StudentWorld* myWorld, double x, double y)
: Bacteria(myWorld, IID_ECOLI, x, y)
{
    setHealth(5);
}

void EColi::add(double x, double y){
    getWorld()->addEColi(x, y);
}


void EColi::playDeathSound(){
    getWorld()->playSound(SOUND_ECOLI_DIE);
}

void EColi::playHurtSound(){
    getWorld()->playSound(SOUND_ECOLI_HURT);
}
