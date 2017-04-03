/*------------------------------------------------------------------------------
* Source: Map.cpp
*
* Functions:
*       Map(string file);
*       int loadFileData();
*       void genWalls(const struct MapPoint wstart[MAX_WALLS]);
*       string getFile();
*       void setFile(string f);
*       void mapLoadToGame();
*       void printData();
*
* Date: March 23, 2017
*
* Revisions:
* Edited By : Name 2016/JAN/25 - Description
*
* Designer: Jordan Lin
*
* Author: Jordan Lin
*
* Notes:
* Map Class functions used for reading in a map file and loading the map into the game.
------------------------------------------------------------------------------*/
#include "Map.h"

using namespace std;

/**
 * Date: Mar. 23, 2017
 * Author: Jordan Lin
 * Function Interface: Map(string file)
 * Parameters:
 *      file - File name, must be a CSV File
 * Description:
 * Map Object Constructor, creates a map object using the map selected file name.
 */
Map::Map(string file):fname(file) {
    memset(AIMap.data(), 0, AIMap.size() * sizeof(AIMap[0]));
    memset(mapdata.data(), 0,  mapdata.size() * sizeof(mapdata[0]));
}

/**
 * Date: Mar. 23, 2017
 * Author: Jordan Lin
 * Function Interface: int loadFileData()
 * Description:
 * Read's the Map CSV file map data with the appropriate tile IDs
 */
int Map::loadFileData() {
    // Open file stream for selected map file
    ifstream file(fname.c_str());

    if (file.fail()) {
        printf("failed to open file\n");
        return 0;
    }

    // Array of wallstart positions
    array<MapPoint, MAX_WALLS> wallStart;
    // Indices
    int j = 0;
    int i = 0;

    // Character Variable
    char ch;
    while (file >> noskipws >> ch) {
        switch (ch) {
            case ',':
                continue;
            case '\n':
                j = 0;
                i++;
                continue;
            case WALL:              //Wall piece found, update AI Map
                AIMap[i][j] = 1;
                break;
            case WALL_START:        //Label start of wall rectangle
                wallStart[wallStartCount].x = j;
                wallStart[wallStartCount].y = i;
                wallStartCount++;
                AIMap[i][j] = 1;
                break;
            // case CONCRETE_START:    //Start of concrete area
            case ZOMBIE_SPAWN:      //Zombie spawn Points
                zombieSpawn[zombieSpawnCount].x = j;
                zombieSpawn[zombieSpawnCount].y = i;
                zombieSpawnCount++;
                break;
            case SHOP_SPOT:         // Shop spot points
                shops[shopCount].x = j;
                shops[shopCount].y = i;
                shopCount++;
                break;
            case BASE_START:        //Start of base area
                base.x = j;
                base.y = i;
                break;
        }
        mapdata[i][j] = ch;               //Store map file value
        j++;
    }
    genWalls(wallStart);
    // printData();
    return 1;
}

/**
 * Date: Mar. 23, 2017
 * Author: Jordan Lin
 * Function Interface: void genWalls(const struct MapPoint wstart[MAX_WALLS])
 * Parameters:
 *      wstart - Array of wall start MapPoints.
 * Description:
 * Read's the Map CSV file map data with the appropriate tile IDs
 */
void Map::genWalls(const array<MapPoint, MAX_WALLS> wallStart) {
    int nextx;
    int nexty;
    int endx;
    int endy;

    for (int q = 0; q < wallStartCount; q++) {
        int startx = wallStart[q].x;
        int starty = wallStart[q].y;
        nextx = startx + 1;
        nexty = starty + 1;
        while (mapdata[starty][nextx] == WALL) {
            nextx ++;
        }
        endx = nextx;
        while (mapdata[nexty][startx] == WALL) {
            nexty++;
        }
        endy = nexty;
        MapWall temp;
        temp.x = startx * T_SIZE;
        temp.y = starty * T_SIZE;
        temp.width = (endx - startx) * T_SIZE;
        temp.height = (endy - starty) * T_SIZE;
        walls.push_back(temp);
    }
}

/**
 * Date: Mar. 23, 2017
 * Author: Jordan Lin
 * Function Interface: void mapLoadToGame()
 * Description:
 * Creates the wall structures using the GameManager createWall function.
 */
void Map::mapLoadToGame() {
    for (const auto& w : walls) {
        logv("Creating wall");
        GameManager::instance()->createWall(w.x, w.y, w.width, w.height);
    }
}

/**
 * Date: Mar. 23, 2017
 * Author: Jordan Lin
 * Function Interface: string getFile()
 * Parameters:
 * Description:
 * Print Map Data loaded includeing:
 *  - Base Position
 *  - Zombie spawn points
 *  - Shop Positions
 *  - Wall Position/Dimensions
 */
void Map::printData() {
    cout << "MAP DATA" << endl;
    for (int q = 0; q < M_HEIGHT; q++) {
        for (int w = 0; w < M_WIDTH; w++) {
            cout << mapdata[q][w];
        }
        std::cout << std::endl;
    }
    cout << endl;

    cout << "BASE POSITIONS" << endl;
    printf("Position: %d, %d\n", base.x, base.y);
    cout << endl;

    cout << "SPAWN POINTS" << endl;
    for(int i = 0; i < zombieSpawnCount; i++) {
        printf("Spawn Point: %d; Position: %d, %d\n", i, zombieSpawn[i].x, zombieSpawn[i].y);
    }
    cout << endl;

    cout << "SHOP POSITIONS" << endl;
    for(int i = 0; i < shopCount; i++) {
        printf("Shop: %d; Position: %d, %d\n", i, shops[i].x, shops[i].y);
    }

    cout << "AI MAP" << endl;
    for (int q = 0; q < M_HEIGHT; q++) {
        for (int w = 0; w < M_WIDTH; w++) {
            cout << AIMap[q][w];
        }
        std::cout << std::endl;
    }
    cout << endl;

    cout << "WALL DATA" << endl;
    for (int q = 0; q < wallStartCount; q++) {
    printf("Wall: %d;\t Position: %d, %d;\t\t width = %d, height = %d\n", q, walls[q].x / T_SIZE,
            walls[q].y / T_SIZE, walls[q].width / T_SIZE, walls[q].height / T_SIZE);
    }
}
