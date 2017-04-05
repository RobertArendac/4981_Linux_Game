#include "MatchManager.h"

MatchManager::MatchManager() : round(0), spawnTick(0), zombiesToSpawn(0) {

}

void MatchManager::checkMatchState() {
    if (GameManager::instance()->getZombieManager().size() <= 0 && zombiesToSpawn <= 0) {
          newRound();
    }
    if (zombiesToSpawn > 0) {
        spawnZombies();
    }
}

void MatchManager::spawnZombies() {
    const int currentTime = SDL_GetTicks();
    if (currentTime < (spawnTick + ZOMBIE_SPAWN_DELAY)) {
        return;
    }
    spawnTick = currentTime;

    CollisionHandler& ch = GameManager::instance()->getCollisionHandler();
    for (auto& pos : spawnPoints) {
        if (zombiesToSpawn > 0) {
            if (!ch.detectMovementCollision(ch.getQuadTreeEntities(
                    ch.quadtreeMarine,&pos),&pos)
                    || ch.detectMovementCollision(ch.getQuadTreeEntities(ch.quadtreeZombie,&pos),&pos)) {
                GameManager::instance()->createZombie(pos);
                --zombiesToSpawn;
            }
        } else {
           break;
        }
    }
}

/**
* Date: April 5, 2017
*
* Designed: Isaac M.
* Programmed: Robert A.
* Function Interface: void MatchManager::setSpawnPoints(std::vector<MapPoint> points)
* Description:
* Inserts all the spawn points.  Each spawn point is associated with a zombie
*/
void MatchManager::setSpawnPoints(std::vector<MapPoint> points) {
    SDL_Rect temp = {INITVAL, INITVAL, ZOMBIE_WIDTH, ZOMBIE_HEIGHT};
    for (const auto& p : points) {
        temp.x = p.x+5;
        temp.y = p.y+5;
        spawnPoints.emplace_back(0, temp, temp, temp, temp, 100, ZombieState::ZOMBIE_MOVE);
    }
}

void MatchManager::newRound() {
    zombiesToSpawn = (spawnPoints.size() * ++round) * 2;
}
