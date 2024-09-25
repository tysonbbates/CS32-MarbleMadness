#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

using namespace std;



// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual int init();
  virtual int move();
  virtual void cleanUp();
    bool agentColocate(int x, int y) const;
    bool swallowMarble(int x, int y);
    bool playerColocated(int x, int y) const;
    int getCrystalsRemaining() const;
    void decCrystals();
    Player* getPlayer() const;
    bool allowsMarbleColocation(int x, int y) const;
    bool pushMarbleFromTo(int initX, int initY, int endX, int endY);
    void checkExit(int x, int y);
    bool takeClearShot(int x, int y, int direction);
    bool checkPeaSquare(int x, int y);
    void shootPea(int x, int y, int direction);
    int conductCensus(int x, int y) const;
    void createThief(int x, int y, int product);
    bool allDirectionsBlocked(int x, int y) const;
    Actor* stealIfPossible(int x, int y);
private:
    int loadALevel();
    string createStatusText();
    
    bool levelComplete;
    int numCrystals;
    vector<Actor*> actorPtrs;
    unsigned int bonus;
    Player* player;
    
};

#endif // STUDENTWORLD_H_
