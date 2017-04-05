/*------------------------------------------------------------------------------
* Source: Zombie.cpp    
*
* Functions:
*    
*
* Date: 
*
* Revisions: 
* Edited By : Yiaoping Shu- Style guide
*
* Designer: 
*
* Author: 
*
* Notes:
*  
------------------------------------------------------------------------------*/
#include <math.h>
#include <random>
#include <cassert>
#include <utility>
#include "Node.h"
#include "Zombie.h"
#include "../game/GameManager.h"
#include "../log/log.h"
#include "../map/Map.h"
using namespace std;

Zombie::Zombie(const int32_t id, const SDL_Rect& dest, const SDL_Rect& movementSize, const SDL_Rect& projectileSize,
        const SDL_Rect& damageSize, const int health, const ZombieState state, const int step,
        const ZombieDirection dir, const int frame) : Entity(id, dest, movementSize, projectileSize,
        damageSize), Movable(id, dest, movementSize, projectileSize, damageSize, ZOMBIE_VELOCITY),
        health(health), state(state), step(step), dir(dir), frame(frame) {
    logv("Create Zombie\n");
    inventory.initZombie();
}

Zombie::~Zombie() {
    logv("Destroy Zombie\n");
}

/**
 * Get move direction
 * Fred Yang
 * February 14
 */
ZombieDirection Zombie::getMoveDir() {
    if (frame > 0) {
        return dir;
    }

    string pth = generatePath(Point(getX(),getY()));

    return static_cast<ZombieDirection>(pth.length() > 0 ? stoi(pth.substr(0,1)) : -1);
}

void Zombie::onCollision() {
    // Do nothing for now
}

void Zombie::collidingProjectile(int damage) {
    health -= damage;
    if (health <= 0) {
        GameManager::instance()->deleteZombie(getId());
    }
}

void Zombie::attack() {
    // Do nothing for now
}

void Zombie::die() {
    // Do nothing for now
}

/**
 * Returns if the zombie is moving
 * Robert Arendac
 * March 7
*/
bool Zombie::isMoving() const {
    return (state == ZombieState::ZOMBIE_MOVE);
}

/**
 * Robert Arendac, Fred Yang
 * March 28
 *
 * Zombie detects objects in vicinity.
 * In theory, zombies will only have a movement collision with a target
 * as their pathfinding should walk around obstacles.
 * Return:  0- nothing, 1- zombie, 2- wall, 3- marine,
 *          4- turret, 5- barricade, 6- other objects(base tower)
 *
 * Note:
 * It's important for Zombie to know what exactly the object is.
 * object typeId can be defined in GameManage.h
*/
int Zombie::detectObj() const {
    int objTypeId = 0;
    auto ch = GameManager::instance()->getCollisionHandler();

    if (ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeZombie, this), this)) {
        objTypeId = 1;
    } else if (ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeWall, this), this)) {
        objTypeId = 2;
    } else if (ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeMarine, this), this)) {
        objTypeId = 3;
    } else if (ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeTurret, this), this)) {
        objTypeId = 4;
    } else if (ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeBarricade,this),this)) {
        objTypeId = 5;
    } else if (ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeObj,this),this)) {
        objTypeId = 6;
    }

    return objTypeId;
}

/**
 * overriden move method, preventing zombies from blocking
 * Fred Yang,  Robert Arendac
 * March 15
*/
void Zombie::move(float moveX, float moveY, CollisionHandler& ch){
    ZombieDirection newDir = ZombieDirection::DIR_INVALID;
    ZombieDirection nextDir = ZombieDirection::DIR_INVALID;

    const float oldX = getX();
    const float oldY = getY();

    // Move the Movable left or right
    setX(getX() + moveX);

    if (detectObj() > 0) {
        setX(getX() - moveX);
    }

    // Move the Movable up or down
    setY(getY() + moveY);

    if (detectObj() > 0) {
        setY(getY() - moveY);
    }

    const float curX = getX();
    const float curY = getY();
    const float dist = sqrt((curX - oldX)*(curX - oldX) + (curY - oldY)*(curY - oldY));

    // zombie blocked
    if (dist < BLOCK_THRESHOLD) {
        string pth = getPath();
        size_t found = pth.find_first_not_of('0' + static_cast<int>(dir));

        if (found != string::npos) {
          nextDir = static_cast<ZombieDirection>(stoi(pth.substr(found, 1)));
        }

        // If blocked, searching for better direction
        switch (dir) {
            case ZombieDirection::DIR_R:
                newDir = (nextDir == ZombieDirection::DIR_RD ?
                          ZombieDirection::DIR_D : ZombieDirection::DIR_U);
                break;
            case ZombieDirection::DIR_RD:
                newDir = (nextDir == ZombieDirection::DIR_D ?
                          ZombieDirection::DIR_LD : ZombieDirection::DIR_RU);
                break;
            case ZombieDirection::DIR_D:
                newDir = (nextDir == ZombieDirection::DIR_LD ?
                          ZombieDirection::DIR_L : ZombieDirection::DIR_R);
                break;
            case ZombieDirection::DIR_LD:
                newDir = (nextDir == ZombieDirection::DIR_L ?
                          ZombieDirection::DIR_LU : ZombieDirection::DIR_RD);
                break;
            case ZombieDirection::DIR_L:
                newDir = (nextDir == ZombieDirection::DIR_LU ?
                          ZombieDirection::DIR_U : ZombieDirection::DIR_D);
                break;
            case ZombieDirection::DIR_LU:
                newDir = (nextDir == ZombieDirection::DIR_U ?
                          ZombieDirection::DIR_RU : ZombieDirection::DIR_LD);
                break;
            case ZombieDirection::DIR_U:
                newDir = (nextDir == ZombieDirection::DIR_RU ?
                          ZombieDirection::DIR_R : ZombieDirection::DIR_L);
                break;
            case ZombieDirection::DIR_RU:
                newDir = (nextDir == ZombieDirection::DIR_R ?
                          ZombieDirection::DIR_RD : ZombieDirection::DIR_LU);
                break;
            case ZombieDirection::DIR_INVALID:
                break;
        }

        // set current direction
        setCurDir(newDir);
    }
}

/**
 * Get the direction of the zombie and take a step in the appropriate direction
 * Robert Arendac, Fred Yang
 * March 28
*/
void Zombie::generateMove() {
     // Direction zombie is moving
    const ZombieDirection direction = getMoveDir();

    // detect surroundings
    const int collisionObjId = detectObj();

    // path is empty, prepared to switch state to IDLE
    if (direction == ZombieDirection::DIR_INVALID) {
        if (frame > 0) {
            --frame;
        }

        setState(ZombieState::ZOMBIE_IDLE);

        return;
    }

    // preferrable objects appear in vicinity, prepared to attack
    if (collisionObjId > 0) {
        if (frame > 0) {
            --frame;
        }

        if (collisionObjId > 2) { // base, marine, turret, or barricade
            setState(ZombieState::ZOMBIE_ATTACK);
        }

        return;
    }

    // Each case will set direction and angle based on the next step in the path
    switch (direction) {
        case ZombieDirection::DIR_R:
            setDX(ZOMBIE_VELOCITY);
            setDY(0);
            setAngle(static_cast<double>(ZombieAngles::EAST));
            break;
        case ZombieDirection::DIR_RD:
            setDX(ZOMBIE_VELOCITY);
            setDY(ZOMBIE_VELOCITY);
            setAngle(static_cast<double>(ZombieAngles::SOUTHEAST));
            break;
        case ZombieDirection::DIR_D:
            setDX(0);
            setDY(ZOMBIE_VELOCITY);
            setAngle(static_cast<double>(ZombieAngles::SOUTH));
            break;
        case ZombieDirection::DIR_LD:
            setDX(-ZOMBIE_VELOCITY);
            setDY(ZOMBIE_VELOCITY);
            setAngle(static_cast<double>(ZombieAngles::SOUTHWEST));
            break;
        case ZombieDirection::DIR_L:
            setDX(-ZOMBIE_VELOCITY);
            setDY(0);
            setAngle(static_cast<double>(ZombieAngles::WEST));
            break;
        case ZombieDirection::DIR_LU:
            setDX(-ZOMBIE_VELOCITY);
            setDY(-ZOMBIE_VELOCITY);
            setAngle(static_cast<double>(ZombieAngles::NORTHWEST));
            break;
        case ZombieDirection::DIR_U:
            setDX(0);
            setDY(-ZOMBIE_VELOCITY);
            setAngle(static_cast<double>(ZombieAngles::NORTH));
            break;
        case ZombieDirection::DIR_RU:
            setDX(ZOMBIE_VELOCITY);
            setDY(-ZOMBIE_VELOCITY);
            setAngle(static_cast<double>(ZombieAngles::NORTHEAST));
            break;
        case ZombieDirection::DIR_INVALID:  // Shouldn't ever happens, gets rid of warning
            break;
    }
    zAttack();
    // Frames are used to make sure the zombie doesn't move through the path too quickly/slowly
    if (frame > 0) {
        --frame;
    } else {
        setCurFrame(ZOMBIE_FRAMES);
        ++step;
    }

    setCurDir(direction);
    setState(ZombieState::ZOMBIE_MOVE);
}

/**
 * A* algo generates a string of direction digits.
 * Fred Yang
 * March 15
 */
string Zombie::generatePath(const Point& start) {
    return generatePath(start, Point(MAP_WIDTH / 2, MAP_HEIGHT / 2));
}

/**
 * A* algo generates a string of direction digits.
 * Fred Yang
 * Feb 14
 */
string Zombie::generatePath(const Point& start, const Point& dest) {
    // temp index
    int i, j;

    //Boolean map of obstacles
    auto gameMap = getAIMap();

    // priority queue index
    int index = 0;

    // row & column index
    int curRow, curCol;
    int newRow, newCol;

    // path to be generated
    string path;

    // priority queue
    static array<priority_queue<Node>, 2> pq;

    // reset the node maps
    memset(closedNodes, 0, sizeof(int) * M_WIDTH * M_HEIGHT);
    memset(openNodes, 0, sizeof(int) * M_WIDTH * M_HEIGHT);
    memset(dirMap, 0, sizeof(int) * M_WIDTH * M_HEIGHT);

    const int xNodeStart = static_cast<int> (start.second + TILE_OFFSET) / TILE_SIZE;
    const int yNodeStart = static_cast<int> (start.first + TILE_OFFSET) / TILE_SIZE;
    const int xNodeDest = static_cast<int> (dest.second + TILE_OFFSET) / TILE_SIZE - 1;
    const int yNodeDest = static_cast<int> (dest.first + TILE_OFFSET) / TILE_SIZE - 1;

    // create the start node and push into open list
    Node curNode(xNodeStart, yNodeStart);
    curNode.updatePriority(xNodeDest, yNodeDest);
    pq[index].push(curNode);

    // A* path finding
    while (!pq[index].empty()) {
        // get the current node with the highest priority from open list
        curNode = pq[index].top();

        curRow = curNode.getXPos();
        curCol = curNode.getYPos();

        // remove the node from the open list
        pq[index].pop();

        // mark it on open/close map
        openNodes[curRow][curCol] = 0;
        closedNodes[curRow][curCol] = 1;

        // quit searching when the destination is reached
        if (curRow == xNodeDest && curCol == yNodeDest) {
            // generate the path from destination to start
            // by following the directions
            path = "";
            while (!(curRow == xNodeStart && curCol == yNodeStart)) {
                j = dirMap[curRow][curCol];
                path = static_cast<char>('0' + (j + DIR_CAP / 2) % DIR_CAP) + path;
                curRow += MY[j];
                curCol += MX[j];
            }

            // empty the leftover nodes
            pq[index] = priority_queue<Node>();

            setPath(path);
            return path;
        }

        // traverse neighbors
        for (i = 0; i < DIR_CAP;i++) {
            // neighbor coordinates
            newRow = curRow + MY[i];
            newCol = curCol + MX[i];

            // not evaluated & not outside (bound checking)
            if (!(newRow < 0 || newRow > M_HEIGHT - 1 || newCol < 0 || newCol > M_WIDTH - 1
                || gameMap[newRow][newCol] >= 1 || closedNodes[newRow][newCol] == 1)) {

                // generate a child node
                Node childNode(newRow, newCol, curNode.getLevel(), curNode.getPriority());
                childNode.nextLevel(i);
                childNode.updatePriority(xNodeDest, yNodeDest);

                // if it is not in the open list then add into that
                if (openNodes[newRow][newCol] == 0) {
                    openNodes[newRow][newCol] = childNode.getPriority();
                    pq[index].push(childNode);

                    // update the parent direction info
                    dirMap[newRow][newCol] = (i + DIR_CAP / 2) % DIR_CAP;
                } else if (openNodes[newRow][newCol] > childNode.getPriority()) {
                    // update the priority info
                    openNodes[newRow][newCol] = childNode.getPriority();

                    // update the parent direction info
                    dirMap[newRow][newCol] = (i + DIR_CAP / 2) % DIR_CAP;

                    // use a backup queue to put the best node (with highest priority)
                    // onto the top of the queue, which can be chosen later to build the path.
                    while (!(pq[index].top().getXPos() == newRow &&
                           pq[index].top().getYPos() == newCol)) {
                        pq[1 - index].push(pq[index].top());
                        pq[index].pop();
                    }

                    pq[index].pop();

                    if (pq[index].size() > pq[1 - index].size()) {
                        index = 1 - index;
                    }

                    while (!pq[index].empty()) {
                        pq[1 - index].push(pq[index].top());
                        pq[index].pop();
                    }

                    index = 1 - index;
                    pq[index].push(childNode);
                }
            }
        }
    }

    return ""; // no route found
}

/**
 * Date: Mar. 28, 2017
 * Author: Mark Tattrie
 * Function Interface: void Zombie::zAttack()
 * Description:
 * Calls the zombies current weapon "ZombieHands" to fire
 */
void Zombie::zAttack(){
    Weapon* w = inventory.getCurrent();
    if (w) {
        w->fire(*this);
    } else {
        logv("Zombie Slot Empty\n");
    }
}
