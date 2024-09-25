#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::loadALevel(){
    ostringstream levelFile;
    levelFile.fill('0');
    levelFile << "level" << setw(2) << getLevel() << ".txt";
    string curLevel = levelFile.str();
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level:: load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
    numCrystals = 0;
    for(int x = 0; x < VIEW_WIDTH; x++){
        for(int y = 0; y < VIEW_HEIGHT; y++){
            switch (lev.getContentsOf(x, y)) {
                case (Level::player):
                    player = new Player(this, x ,y);
                    break;
                case (Level::wall):
                    actorPtrs.push_back(new Wall(this, x, y));
                    break;
                case (Level::pit):
                    actorPtrs.push_back(new Pit(this, x ,y));
                    break;
                case (Level::crystal):
                    numCrystals++;
                    actorPtrs.push_back(new Crystal(this, x ,y));
                    break;
                case (Level::ammo):
                    actorPtrs.push_back(new AmmoGoodie(this, x, y));
                    break;
                case (Level::marble):
                    actorPtrs.push_back(new Marble(this, x, y));
                    break;
                case (Level::restore_health):
                    actorPtrs.push_back(new RestoreHealthGoodie(this, x , y));
                    break;
                case (Level::exit):
                    actorPtrs.push_back(new Exit(this, x, y));
                    break;
                case (Level::vert_ragebot):
                    actorPtrs.push_back(new RageBot(this, x, y, 270));
                    break;
                case (Level::horiz_ragebot):
                    actorPtrs.push_back(new RageBot(this, x, y, 0));
                    break;
                case (Level::thiefbot_factory):
                    actorPtrs.push_back(new ThiefBotFactory(this, x, y, PRODUCT_REGULARTHIEF));
                    break;
                case (Level::mean_thiefbot_factory):
                    actorPtrs.push_back(new ThiefBotFactory(this, x, y, PRODUCT_MEANTHIEF));
                    break;
                default:
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

string StudentWorld::createStatusText(){
    int score = getScore();
    int level = getLevel();
    int livesLeft = getLives();
    ostringstream statusText;
    statusText.fill('0');
    statusText << "Score: " << setw(7) << score;
    statusText << "  Level: " << setw(2) << level;
    statusText.fill(' ');
    statusText << "  Lives: " << setw(2) << livesLeft;
    statusText << "  Health: " << setw(3) <<(100.0*player->getHitPoints())/PLAYER_MAX_HEALTH <<"%";
    statusText << "  Ammo: " << setw(3) << player->getAmmo();
    statusText << "  Bonus: " << setw(4) << bonus;
    return statusText.str();
}


int StudentWorld::init()
{
    if(loadALevel() == GWSTATUS_LEVEL_ERROR) return GWSTATUS_LEVEL_ERROR;
    
    bonus = STARTING_BONUS;
    levelComplete = false;
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    setGameStatText(createStatusText());
    //EVERYONE doSomething()
    player->doSomething();
    for(int p = 0; p != actorPtrs.size(); p++){
        if((actorPtrs[p])->isAlive())
            (actorPtrs[p])->doSomething();
    }
    //CHECK IF LEVEL COMPLETED
    if(levelComplete){
        increaseScore(bonus + COMPLETION_BONUS);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    //CHECK FOR DEATHS
    if(!player->isAlive()){
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end();){
        if(!(*p)->isAlive() && (*p)->isVisible()){
            if((*p)->countsInFactoryCensus()){
                Actor* goodie = (*p)->getItem();
                goodie->moveTo((*p)->getX(), (*p)->getY());
                goodie->setVisible(true);
                goodie->setHitPoints(CONST_HEALTH);
            }
            delete *p;
            p = actorPtrs.erase(p);
        }
        else
            p++;
    }
    
    
    
    if(bonus>0) bonus--;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete player;
    for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end();){
        delete (*p);
        p = actorPtrs.erase(p);
    }
}

bool StudentWorld::agentColocate(int x, int y) const{
    if(player->getX() == x && player->getY() == y) return false;
    for(vector<Actor*>::const_iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
        if((*p)->getX() == x && (*p)->getY() == y){
            if((*p)->allowsAgentColocation() == false) return false;
        }
    }
    return true;
}

bool StudentWorld::swallowMarble(int x, int y){
    for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
        if((*p)->getX() == x && (*p)->getY() == y){
            if((*p)->isSwallowable() == true){
                (*p)->damage((*p)->getHitPoints());
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::playerColocated(int x, int y) const{
    return (player->getX() == x && player->getY() == y);
}

int StudentWorld::getCrystalsRemaining() const{
    return numCrystals;
}

void StudentWorld::decCrystals(){
    numCrystals--;
}

Player* StudentWorld::getPlayer() const{
    return player;
}

bool StudentWorld::allowsMarbleColocation(int x, int y) const{
    for(vector<Actor*>::const_iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++)
        if((*p)->getX() == x && (*p)->getY() == y && (*p)->isAlive())
            if((*p)->allowsMarble() == false)
                return false;
    return true;
}

bool StudentWorld::pushMarbleFromTo(int initX, int initY, int endX, int endY){
    for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++)
        if((*p)->getX() == initX && (*p)->getY() == initY &&
           (*p)->bePushedBy(player, endX, endY)){
            (*p)->moveTo(endX, endY);
            return true;
        }
    return false;
}

void StudentWorld::checkExit(int x, int y){
    if(player->getX() == x && player->getY() == y){
        levelComplete = true;
    }
}

bool StudentWorld::takeClearShot(int x,int y, int direction)
{
    int playerX = player->getX();
    int playerY = player->getY();
    switch(direction){
        case(0): //right
            if(playerY == y && playerX > x){
                for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
                    if(((*p)->getX() > x && (*p)->getX() < playerX) &&
                       ((*p)->getY() == y && (*p)->stopsPea())){
                        return false;
                    }
                }
                shootPea(x, y, direction);
                return true;
            }
            return false;
        case (90): //up
            if(playerX == x && playerY > y){
                for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
                    if(((*p)->getY() > y && (*p)->getY() < playerY) &&
                       ((*p)->getX() == x && (*p)->stopsPea())){
                        return false;
                    }
                }
                shootPea(x, y, direction);
                return true;
            }
            return false;
        case (180): //left
            if(playerY == y && playerX < x){
                for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
                    if(((*p)->getX() < x && (*p)->getX() > playerX) &&
                       ((*p)->getY() == y && (*p)->stopsPea())){
                        return false;
                    }
                }
                shootPea(x, y, direction);
                return true;
            }
            return false;
        case (270): //down
            if(playerX == x && playerY < y){
                for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
                    if(((*p)->getY() < y && (*p)->getY() > playerY) &&
                       ((*p)->getX() == x && (*p)->stopsPea())){
                        return false;
                    }
                }
                shootPea(x, y, direction);
                return true;
            }
            return false;
        default:
            return false;
    }
}

bool StudentWorld::checkPeaSquare(int x, int y){
    if(player->getX() == x && player->getY() == y){
        if(player->tryToBeKilled(PEA_DAMAGE))
            playSound(SOUND_PLAYER_DIE);
        else
            playSound(SOUND_PLAYER_IMPACT);
        cerr << "HIT PLAYER" << endl;
        return true;
    }
    
    bool peaStopped = false;
    for(vector<Actor*>::iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
        if((*p)->getX() == x && (*p)->getY() == y){
            if((*p)->stopsPea()){
                peaStopped = true;
                if((*p)->isDamageable()){
                    if((*p)->isSwallowable())
                        (*p)->damage(PEA_DAMAGE);
                    else{
                        if((*p)->tryToBeKilled(PEA_DAMAGE))
                            playSound(SOUND_ROBOT_DIE);
                        else
                            playSound(SOUND_ROBOT_IMPACT);}
                    return true;
                }
                
            }
        }
    }
    return peaStopped;
}

void StudentWorld::shootPea(int x, int y, int direction){
    switch (direction) {
        case 0:
            actorPtrs.push_back(new Pea(this, x+1, y, direction));
            break;
        case 90:
            actorPtrs.push_back(new Pea(this, x, y+1, direction));
            break;
        case 180:
            actorPtrs.push_back(new Pea(this, x-1, y, direction));
            break;
        case 270:
            actorPtrs.push_back(new Pea(this, x, y-1, direction));
            break;
        default:
            break;
    }
}

int StudentWorld::conductCensus(int x, int y) const{
    int count = 0;
    for(vector<Actor*>::const_iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
        if((*p)->countsInFactoryCensus() &&
           ((((*p)->getX() - x) < 3 && x -(*p)->getX() < 3) &&
            (((*p)->getY() - y) < 3 && y - (*p)->getY() < 3))){
            if((*p)->getX()-x == 0 && (*p)->getY() == 0){ //ThiefBot on factory
                return CENSUS_MAX;
            }
            count++;
        }
    }
    return count;
}

void StudentWorld::createThief(int x, int y, int product){
    playSound(SOUND_ROBOT_BORN);
    if(product == PRODUCT_REGULARTHIEF)
        actorPtrs.push_back(new RegularThiefBot(this, x, y));
    else if (product == PRODUCT_MEANTHIEF)
        actorPtrs.push_back(new MeanThiefBot(this, x, y));
}

bool StudentWorld::allDirectionsBlocked(int x, int y) const{
    bool up = false;
    bool left =false;
    bool right = false;
    bool down = false;
    for(vector<Actor*>::const_iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
        if((*p)->getX() == x+1 && (*p)->getY() == y && !(*p)->allowsAgentColocation())
            right = true;
        else if ((*p)->getX() == x && (*p)->getY() == y+1 && !(*p)->allowsAgentColocation())
            up = true;
        else if((*p)->getX() == x-1 && (*p)->getY() == y && !(*p)->allowsAgentColocation())
            left = true;
        else if((*p)->getX() == x && (*p)->getY() == y-1 && !(*p)->allowsAgentColocation())
            down = true;
    }
    return (right && up && left && down);
}

Actor* StudentWorld::stealIfPossible(int x, int y){
    for(vector<Actor*>::const_iterator p = actorPtrs.begin(); p != actorPtrs.end(); p++){
        if((*p)->getX() == x && (*p)->getY() == y && (*p)->isStealable() && (*p)->isAlive()){
            if(randInt(1, 10) == 1){
                (*p)->damage(CONST_HEALTH);
                (*p)->setVisible(false);
                playSound(SOUND_ROBOT_MUNCH);
                return *p;
            }
        }
    }
    return nullptr;
}
