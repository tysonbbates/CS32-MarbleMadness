#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
class Agent;

const int START_AMMO = 20;
const int CONST_HEALTH = 1;
const int MARBLE_HEALTH = 10;
const int RAGEBOT_HEALTH = 10;
const int THIEFBOT_HEALTH = 5;
const int MEANTHIEFBOT_HEALTH = 8;
const int AMMO_TO_ADD = 20;
const int CRYSTAL_SCORE = 50;
const int AMMO_SCORE = 100;
const int RESTORE_SCORE = 500;
const int EXTRALIFE_SCORE = 1000;
const int RAGEBOT_SCORE = 100;
const int THIEFBOT_SCORE = 10;
const int MEANTHIEFBOT_SCORE = 20;
const int PEA_DAMAGE = 2;
const int CENSUS_MAX = 3;

const int STARTING_BONUS = 1000;
const int COMPLETION_BONUS = 2000;
const int PLAYER_MAX_HEALTH = 20;
const int PRODUCT_REGULARTHIEF = 0;
const int PRODUCT_MEANTHIEF = 1;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir);
      // Action to perform each tick
    virtual void doSomething() = 0;

      // Is this actor alive?
    bool isAlive() const;
    
      // Mark this actor as dead
    void setDead();
    
      // Reduce this actor's hit points
    void decHitPoints(int amt);
    
      // Get this actor's world
    StudentWorld* getWorld() const;
    
      // Can an agent occupy the same square as this actor?
    virtual bool allowsAgentColocation() const; //base case false
    
      // Can a marble occupy the same square as this actor?
    virtual bool allowsMarble() const; //base case false
    
      // Does this actor count when a factory counts items near it?
    virtual bool countsInFactoryCensus() const; //base case false
    
      // Does this actor stop peas from continuing?
    virtual bool stopsPea() const; //base case true
    
      // Can this actor be damaged by peas?
    virtual bool isDamageable() const; //base case false

      // Cause this Actor to sustain damageAmt hit points of damage.
    virtual void damage(int damageAmt); //base: decrease hitpoints
    
      // Can this actor be pushed by a to location x,y?
    virtual bool bePushedBy(Agent* a, int x, int y); //base case false
    
      // Can this actor be swallowed by a pit?
    virtual bool isSwallowable() const; //base case false
    
      // Can this actor be picked up by a ThiefBot?
    virtual bool isStealable() const; //base case false
    
      // How many hit points does this actor have left?
    virtual int getHitPoints() const; //base case return hp
    
      // Set this actor's hit points to amt.
    virtual void setHitPoints(int amt);
    
      // Make the actor sustain damage.  Return true if this kills the
      // actor, and false otherwise.
    virtual bool tryToBeKilled(int damageAmt);
    virtual Actor* getItem() const;
private:
    int m_health;
    StudentWorld* m_world;
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int startDir);

      // Move to the adjacent square in the direction the agent is facing
      // if it is not blocked, and return true.  Return false if the agent
      // can't move.
    bool moveIfPossible();

      // Return true if this agent can push marbles (which means it's the
      // player).
    virtual bool canPushMarbles() const; //base case false

      // Return true if this agent doesn't shoot unless there's an unobstructed
      // path to the player.
    virtual bool needsClearShot() const;// base case true

      // Return the sound effect ID for a shot from this agent.
    virtual int shootingSound() const = 0;
    virtual bool isDamageable() const; //true
};

class Player : public Agent
{
public:
    Player(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual void damage(int damageAmt);
    virtual bool needsClearShot() const;
    virtual int shootingSound() const;
    virtual bool canPushMarbles() const;

      // Get player's amount of ammunition
    int getAmmo() const;
    
      // Restore player's health to the full amount.
    void restoreHealth();
    
      // Increase player's amount of ammunition.
    void increaseAmmo();
private:
    int m_ammo;
};

class Robot : public Agent
{
public:
    Robot(StudentWorld* world, int startX, int startY, int imageID,
        int hitPoints, int score, int startDir);
    virtual void doSomething();
    virtual void damage(int damageAmt);
    virtual int shootingSound() const;
    virtual void doAction();

      // Does this robot shoot?
    virtual bool isShootingRobot() const; //base case true
    virtual bool isStealingRobot() const; //base case true
private:
    int m_score;
    int m_tickCount;
    int m_restTicks;
};

class RageBot : public Robot
{
public:
    RageBot(StudentWorld* world, int startX, int startY, int startDir);
    virtual void doAction();
    virtual bool isStealingRobot() const;
};

class ThiefBot : public Robot
{
public:
    ThiefBot(StudentWorld* world, int startX, int startY, int imageID,
                         int hitPoints, int score);
    virtual bool countsInFactoryCensus() const;
    virtual void damage(int damageAmt); //drop item possibly
    virtual bool isShootingRobot() const;
    virtual void doAction();
    Actor* getItem() const;
private:
    int distanceBeforeTurning;
    int distanceTraveled;
    Actor* item;
};

class RegularThiefBot : public ThiefBot
{
public:
    RegularThiefBot(StudentWorld* world, int startX, int startY);
    virtual bool isShootingRobot() const;
};

class MeanThiefBot : public ThiefBot
{
public:
    MeanThiefBot(StudentWorld* world, int startX, int startY);
};

class Exit : public Actor
{
public:
    Exit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
    virtual bool stopsPea() const;
private:
    bool m_revealed;
};

class Wall : public Actor
{
public:
    Wall(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
     
};

class Marble : public Actor
{
public:
    Marble(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool isDamageable() const;
    virtual bool isSwallowable() const;
    virtual bool bePushedBy(Agent* a, int x, int y);
};

class Pit : public Actor
{
public:
    Pit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    virtual bool allowsMarble() const;
    virtual bool stopsPea() const;
};

class Pea : public Actor
{
public:
    Pea(StudentWorld* world, int startX, int startY, int startDir);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
    virtual bool stopsPea() const;
    virtual bool allowsMarble() const;
};

class ThiefBotFactory : public Actor
{
public:
    ThiefBotFactory(StudentWorld* world, int startX, int startY, int type);
    virtual void doSomething();
private:
    int product;
    
};

class PickupableItem : public Actor
{
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID,
                            int score);
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
    virtual bool stopsPea() const;
    int getScoreToAdd() const;
    virtual void addEffect() const = 0;
private:
    int m_scoreToAdd;
};

class Crystal : public PickupableItem
{
public:
    Crystal(StudentWorld* world, int startX, int startY);
    virtual void addEffect() const;
};

class Goodie : public PickupableItem
{
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID,
                            int score);
    virtual bool isStealable() const;

private:
    bool m_stolen;
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* world, int startX, int startY);
    virtual void addEffect() const;
};

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(StudentWorld* world, int startX, int startY);
    virtual void addEffect() const;
};

class AmmoGoodie : public Goodie
{
public:
    AmmoGoodie(StudentWorld* world, int startX, int startY);
    virtual void addEffect() const;
};

#endif // ACTOR_H_
