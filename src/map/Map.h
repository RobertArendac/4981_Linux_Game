#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../game/GameManager.h"

using namespace std;

// Tile Size
static constexpr int T_SIZE = 150;

// Map width/height
static constexpr int M_WIDTH = 100;
static constexpr int M_HEIGHT = 100;

// Base width/height
static constexpr int BASE_SIZE = 7;

// Max number of walls and spawn points
static constexpr int MAX_WALLS = 60;
static constexpr int MAX_SPAWNPOINTS = 16;

static constexpr char CONCRETE_START    = '0';
static constexpr char CONCRETE_M        = '1';
static constexpr char WALL              = '2';
static constexpr char ZOMBIE_SPAWN      = '3';
static constexpr char SHOP_SPOT         = '4';
static constexpr char BASE_START        = '5';
static constexpr char BASE              = '6';
static constexpr char FLOOR             = '7';
static constexpr char WALL_START        = '8';


struct MapPoint {
public:
    int x;
    int y;
};

struct MapWall {
public:
    int x;
    int y;
    int width;
    int height;
};

class Map {
public:
    Map(string file);
    int loadFileData();
    void genWalls(const array<MapPoint, MAX_WALLS> wallStart);
    void mapLoadToGame();
    void printData();

    string getFile() { return fname; };
    void setFile(const string f) { fname = f; };


    // Data needed from map file
    // char mapdata[M_HEIGHT][M_WIDTH];
    array< array<char, M_WIDTH>, M_HEIGHT> mapdata;
    //zombie spawn point count
    int zombieSpawnCount;
    // Wall start point count
    int wallStartCount;
    // Shop count;
    int shopCount;
    // Base Start Point
    struct MapPoint base;
    // Array of zombieSpawn points
    struct MapPoint zombieSpawn[MAX_SPAWNPOINTS];
    // Array of shop points
    struct MapPoint shops[6];
    // Boolean Array for AI Nodes
    // bool AIMap[M_HEIGHT][M_WIDTH];
    array<array<bool, M_WIDTH>, M_HEIGHT> AIMap;
    // Vector of walls with pixel position and dimensions.
    vector<MapWall> walls;

private:
    string fname;
};


#endif
