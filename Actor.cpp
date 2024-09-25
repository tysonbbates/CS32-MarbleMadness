#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir)
:GraphObject(imageID, startX, startY, startDir), m_health(hitPoints), m_world(world)
{
    
}
StudentWorld* Actor::getWorld() const{
    return m_world;
}

bool Actor::isAlive() const{
    return m_health > 0;
}
bool Actor::allowsAgentColocation() const{
    return false;
}

Actor* Actor::getItem() const{
    return nullptr;
}

bool Actor::allowsMarble() const{
    return false;
}

bool Actor::countsInFactoryCensus() const{
    return false;
}

bool Actor::stopsPea() const{
    return true;
}

bool Actor::isDamageable() const{
    return false;
}

void Actor::damage(int damageAmt){
    m_health -= damageAmt;
}

bool Actor::bePushedBy(Agent *a, int x, int y){
    return false;
}

bool Actor::isSwallowable() const{
    return false;
}

bool Actor::isStealable() const{
    return false;
}

int Actor::getHitPoints() const{
    return m_health;
}

void Actor::setHitPoints(int amt){
    m_health = amt;
}

bool Actor::tryToBeKilled(int damageAmt){
    m_health -= damageAmt;
    return m_health <= 0;
}



Agent::Agent(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir)
:Actor(world, startX, startY, imageID, hitPoints, startDir)
{
    setVisible(true);
}

bool Agent::canPushMarbles() const{
    return false;
}

bool Agent::isDamageable() const{
    return true;
}

bool Agent::moveIfPossible(){
    switch (getDirection()) {
        case up:
            if(getWorld()->agentColocate(getX(), getY()+1)){
                moveTo(getX(), getY()+1);
                return true;
            }
            return false;
        case left:
            if(getWorld()->agentColocate(getX()-1, getY())){
                moveTo(getX()-1, getY());
                return true;
            }
            return false;
        case down:
            if(getWorld()->agentColocate(getX(), getY()-1)){
                moveTo(getX(), getY()-1);
                return true;
            }
            return false;
        case right:
            if(getWorld()->agentColocate(getX()+1, getY())){
                moveTo(getX()+1, getY());
                return true;
            }
            return false;
        default:
            return false;
    }
}

bool Agent::needsClearShot() const{
    return true;
}


Player::Player(StudentWorld* world, int startX, int startY)
:Agent(world, startX, startY, IID_PLAYER, PLAYER_MAX_HEALTH, right), m_ammo(START_AMMO)
{}

int Player::getAmmo() const{
    return m_ammo;
}

void Player::doSomething(){
    if(!isAlive()) return;
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch (ch){
            case KEY_PRESS_LEFT:
                setDirection(left);
                if(!moveIfPossible())
                    if(getWorld()->pushMarbleFromTo(getX()-1, getY(), getX()-2, getY()))
                        moveTo(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if(!moveIfPossible())
                    if(getWorld()->pushMarbleFromTo(getX()+1, getY(), getX()+2, getY()))
                        moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if(!moveIfPossible())
                    if(getWorld()->pushMarbleFromTo(getX(), getY()+1, getX(), getY()+2))
                        moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                if(!moveIfPossible())
                    if(getWorld()->pushMarbleFromTo(getX(), getY()-1, getX(), getY()-2))
                        moveTo(getX(), getY()-1);
                break;
            case KEY_PRESS_ESCAPE:
                if(tryToBeKilled(PLAYER_MAX_HEALTH))
                   getWorld()->playSound(SOUND_PLAYER_DIE);
                   break;
            case KEY_PRESS_SPACE:
                if(m_ammo > 0){
                    getWorld()->shootPea(getX(), getY(), getDirection());
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_ammo--;
                }
                break;
    }
    }
}

void Player::increaseAmmo(){
    m_ammo += AMMO_TO_ADD;
}

bool Player::canPushMarbles() const{
    return true;
}

int Player::shootingSound() const{
    return SOUND_PLAYER_FIRE;
}

bool Player::needsClearShot() const{
    return false;
}

void Player::damage(int damageAmt){
    if(tryToBeKilled(damageAmt))
        getWorld()->playSound(SOUND_PLAYER_DIE);
    else
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
}

void Player::restoreHealth(){
    setHitPoints(PLAYER_MAX_HEALTH);
}

Wall::Wall(StudentWorld* world, int startX, int startY)
:Actor(world, startX, startY, IID_WALL, CONST_HEALTH, none)
{
    setVisible(true);
}

void Wall::doSomething(){}

Pit::Pit(StudentWorld* world, int startX, int startY)
:Actor(world, startX, startY, IID_PIT, CONST_HEALTH, none)
{
    setVisible(true);
}

bool Pit::allowsMarble() const{
    return true;
}

bool Pit::stopsPea() const{
    return false;
}

void Pit::doSomething(){
    if(isAlive())
        if(getWorld()->swallowMarble(getX(), getY()))
            damage(CONST_HEALTH);
}

//PICKUPABLE ITEMS
//----------------
PickupableItem::PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score)
:Actor(world, startX, startY, imageID, CONST_HEALTH, none), m_scoreToAdd(score)
{
    setVisible(true);
}

int PickupableItem::getScoreToAdd() const{
    return m_scoreToAdd;
}

bool PickupableItem::allowsAgentColocation() const{
    return true;
}

bool PickupableItem::stopsPea() const{
    return false;
}

void PickupableItem::doSomething(){
    if(!isAlive()) return;
    if(getWorld()->playerColocated(getX(), getY())){
        getWorld()->increaseScore(m_scoreToAdd);
        addEffect();
        damage(CONST_HEALTH);
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

Crystal::Crystal(StudentWorld* world, int startX, int startY)
:PickupableItem(world, startX, startY, IID_CRYSTAL, CRYSTAL_SCORE)
{}

void Crystal::addEffect() const{
    getWorld()->decCrystals();
}

Goodie::Goodie(StudentWorld* world, int startX, int startY, int imageID, int score)
:PickupableItem(world, startX, startY, imageID, score), m_stolen(false)
{}

bool Goodie::isStealable() const{
    return true;
}

AmmoGoodie::AmmoGoodie(StudentWorld* world, int startX, int startY)
    :Goodie(world, startX, startY, IID_AMMO, AMMO_SCORE)
{}

void AmmoGoodie::addEffect() const{
    getWorld()->getPlayer()->increaseAmmo();
}

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* world, int startX, int startY)
    :Goodie(world, startX, startY, IID_RESTORE_HEALTH, RESTORE_SCORE)
{}

void RestoreHealthGoodie::addEffect() const{
    getWorld()->getPlayer()->restoreHealth();
}

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, int startX, int startY)
:Goodie(world, startX, startY, IID_EXTRA_LIFE, EXTRALIFE_SCORE){}

void ExtraLifeGoodie::addEffect() const{
    getWorld()->incLives();
}

//MARBLES
//-------
Marble::Marble(StudentWorld* world, int startX, int startY)
:Actor(world, startX, startY, IID_MARBLE, MARBLE_HEALTH, none){}

void Marble::doSomething(){}

bool Marble::isDamageable() const{
    return true;
}

bool Marble::isSwallowable() const{
    return true;
}

bool Marble::bePushedBy(Agent *a, int x, int y){
    if(a->canPushMarbles())//pushed by player
        return getWorld()->allowsMarbleColocation(x, y);
    return false;
}

//PEA
//---
Pea::Pea(StudentWorld* world, int startX, int startY, int startDir)
:Actor(world, startX, startY, IID_PEA, CONST_HEALTH, startDir)
{
    setVisible(true);
}

bool Pea::allowsAgentColocation() const{
    return true;
}

bool Pea::stopsPea() const{
    return false;
}

bool Pea::allowsMarble() const{
    return true;
}

void Pea::doSomething(){
    if(!isAlive()) return;
    if(getWorld()->checkPeaSquare(getX(), getY())){ //hit something
        damage(CONST_HEALTH);
    }
    else{
        //moveForward(); <- this function doesn't seem to always work
        switch (getDirection()) {
            case right:
                moveTo(getX()+1, getY());
                break;
            case up:
                moveTo(getX(), getY()+1);
                break;
            case left:
                moveTo(getX()-1, getY());
                break;
            case down:
                moveTo(getX(), getY()-1);
                break;
            default:
                break;
        }
        if(getWorld()->checkPeaSquare(getX(),getY()))
            damage(CONST_HEALTH);
    }
     
}

//EXIT
//-----
Exit::Exit(StudentWorld* world, int startX, int startY)
:Actor(world, startX, startY, IID_EXIT, CONST_HEALTH, none),m_revealed(false)
{
    setVisible(false);
}

bool Exit::stopsPea() const{
    return false;
}

void Exit::doSomething(){
    if((!m_revealed) && getWorld()->getCrystalsRemaining() == 0){
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        m_revealed = true;
        setVisible(true);
    }
    if(m_revealed){
        getWorld()->checkExit(getX(), getY());
    }
}

bool Exit::allowsAgentColocation() const{
    return true;
}

//ROBOT
//----------------------------------
Robot::Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir)
:Agent(world, startX, startY, imageID, hitPoints, startDir), m_score(score), m_tickCount(0)
{
    m_restTicks = (28 - getWorld()->getLevel()) / 4;
    if (m_restTicks < 3)
        m_restTicks = 3;
}

bool Robot::isShootingRobot() const{
    return true;
}

void Robot::damage(int damageAmt){
    if(tryToBeKilled(damageAmt))
        getWorld()->playSound(SOUND_ROBOT_DIE);
    else
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
}

void Robot::doSomething(){
    if(!isAlive()) return;
    m_tickCount++;
    if((m_tickCount-1) % m_restTicks != 0) return;
    m_tickCount = 1;
    if(isShootingRobot() && getWorld()->takeClearShot(getX(), getY(), getDirection())){
        getWorld()->playSound(SOUND_ENEMY_FIRE);
        return;
    }
    doAction();
}

void Robot::doAction(){
}

int Robot::shootingSound() const{
    return SOUND_ENEMY_FIRE;
}

bool Robot::isStealingRobot() const{
    return true;
}

    //RAGEBOT
    //---------
RageBot::RageBot(StudentWorld* world, int startX, int startY, int startDir)
:Robot(world, startX, startY, IID_RAGEBOT, RAGEBOT_HEALTH, RAGEBOT_SCORE, startDir)
{}

void RageBot::doAction(){
    if(!moveIfPossible()){
        switch(getDirection()){
            case (left):
                setDirection(right);
                break;
            case (right):
                setDirection(left);
                break;
            case (up):
                setDirection(down);
                break;
            case (down):
                setDirection(up);
                break;
            default:
                break;
        }
    }
}

bool RageBot::isStealingRobot() const{
    return false;
}

// THIEFBOT FACTORIES
//--------------------
ThiefBotFactory::ThiefBotFactory(StudentWorld* world, int startX, int startY, int type)
:Actor(world, startX, startY, IID_ROBOT_FACTORY, CONST_HEALTH, none), product(type)
{
    setVisible(true);
}



void ThiefBotFactory::doSomething(){
    if(getWorld()->conductCensus(getX(),getY()) < CENSUS_MAX && randInt(1, 50) == 1){
        getWorld()->createThief(getX(), getY(), product);
    }
}


//THIEFBOTS
//---------
ThiefBot::ThiefBot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score)
:Robot(world, startX, startY, imageID, hitPoints, score, right),item(nullptr), distanceTraveled(0)
{
    distanceBeforeTurning = randInt(1, 6);
}

bool ThiefBot::isShootingRobot() const{
    return true;
}

void ThiefBot::damage(int damageAmt){
    
    if(tryToBeKilled(damageAmt)){
        if(item != nullptr){
            //IMPLEMENT - move item
        }
            
    }
}

void ThiefBot::doAction(){
    if(item == nullptr){
        item = getWorld()->stealIfPossible(getX(), getY());
        if(item != nullptr) return; //stolen
    }
    if(distanceTraveled < distanceBeforeTurning && moveIfPossible()){
            distanceTraveled++;
            return;
    }
    //Couldn't move or have to switch directions
    distanceTraveled = 0;
    if(getWorld()->allDirectionsBlocked(getX(), getY())){
        setDirection(randInt(0, 3)*90);
        return;
    }
    //there is an open direction
    bool notMoved = true;
    while(notMoved){
        setDirection(randInt(0, 3)*90);
        notMoved = !moveIfPossible();
        if(!notMoved){
            distanceBeforeTurning = randInt(1, 6);
            distanceTraveled = 1;
        }
    }
}

Actor* ThiefBot::getItem() const{
    return item;
}


bool ThiefBot::countsInFactoryCensus() const{
    return true;
}



//      REGULAR
RegularThiefBot::RegularThiefBot(StudentWorld* world, int startX, int startY)
:ThiefBot(world, startX, startY, IID_THIEFBOT, THIEFBOT_HEALTH, THIEFBOT_SCORE)
{
    
}

bool RegularThiefBot::isShootingRobot() const{
    return true;
}


//       MEAN
MeanThiefBot::MeanThiefBot(StudentWorld* world, int startX, int startY)
:ThiefBot(world, startX, startY, IID_MEAN_THIEFBOT, MEANTHIEFBOT_HEALTH, MEANTHIEFBOT_SCORE){
    
}

