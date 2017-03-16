#include "Player.h"
#include <math.h>

Player::Player(const int32_t& id) : tempBarricadeID(-1), tempTurretID(-1), 
        marine(GameManager::instance()->getMarine(id)) {

}

void Player::handleMouseUpdate(Window& w, const float camX, const float camY) {
    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    const int mouseDeltaX = w.getWidth() / 2 - mouseX;
    const int mouseDeltaY = w.getHeight() / 2 - mouseY;

    marine.setAngle(((atan2(mouseDeltaX, mouseDeltaY)* 180.0)/M_PI) * - 1);

    if (tempBarricadeID > -1) {
        Barricade &tempBarricade = GameManager::instance()->getBarricade(tempBarricadeID);
        tempBarricade.move(marine.getX(), marine.getY(), mouseX + camX, mouseY + camY,
            GameManager::instance()->getCollisionHandler());
    }

    if (tempTurretID > -1) {
        Turret &tempTurret = GameManager::instance()->getTurret(tempTurretID);
        tempTurret.move(marine.getX(), marine.getY(), mouseX + camX, mouseY + camY,
            GameManager::instance()->getCollisionHandler());

        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            if (tempTurret.collisionCheckTurret(marine.getX(), marine.getY(), mouseX + camX, mouseY + camY,
                    GameManager::instance()->getCollisionHandler())) {
                tempTurret.placeTurret();
                tempTurretID = -1;
            }
        }
    }

    //fire weapon on left mouse click
    if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        if(marine.inventory.getCurrent() != nullptr){
            if(marine.inventory.getCurrent()->getFireState()){
                marine.fireWeapon();
            }
        }
    }

}

void Player::handleMouseWheelInput(const SDL_Event *e){
    marine.inventory.scrollCurrent(e->wheel.y);
}

// function to handle mouse-click events
void Player::handlePlacementClick(SDL_Renderer *renderer) {

    if (tempBarricadeID > -1) {
        Barricade &tempBarricade = GameManager::instance()->getBarricade(tempBarricadeID);
        if (tempBarricade.isPlaceable()) {
            tempBarricade.placeBarricade();
            tempBarricadeID = -1;
        }
    }
}

void Player::handleKeyboardInput(const Uint8 *state) {
    float x = 0;
    float y = 0;
    float velocity = marine.getVelocity();

    // Check for move inputs
    if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
        y -= velocity;
    }
    if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {
        y += velocity;
    }
    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
        x -= velocity;
    }
    if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
        x += velocity;
    }

    //Inventory inputs
    if (state[SDL_SCANCODE_1]){
        marine.inventory.switchCurrent(0);
    } else if (state[SDL_SCANCODE_2]){
        marine.inventory.switchCurrent(1);
    } else if (state[SDL_SCANCODE_3]){
        marine.inventory.switchCurrent(2);
    }

    //Weapon input
    if(state[SDL_SCANCODE_R]){
        marine.inventory.getCurrent()->reloadClip();
    }
    if(state[SDL_SCANCODE_E]){
        marine.checkForPickUp();
    }
    if(state[SDL_SCANCODE_I]) {
        marine.inventory.useItem();
    }
    marine.setDY(y);
    marine.setDX(x);
}

void Player::handleTempBarricade(SDL_Renderer *renderer) {
    if(tempBarricadeID < 0) {
        const double angle = marine.getAngle();
        tempBarricadeID = GameManager::instance()->createBarricade(renderer,
            marine.getX() + PLAYER_PLACE_DISTANCE * cos(angle),
            marine.getY() + PLAYER_PLACE_DISTANCE * sin(angle));
    } else {
        GameManager::instance()->deleteBarricade(tempBarricadeID);
        tempBarricadeID = -1;
    }
}


void Player::handleTempTurret(SDL_Renderer *renderer) {
   if(tempTurretID < 0) {
       const double angle = marine.getAngle();
       tempTurretID = GameManager::instance()->createTurret(renderer, 
           marine.getX() + PLAYER_PLACE_DISTANCE * cos(angle),
           marine.getY() + PLAYER_PLACE_DISTANCE * sin(angle));
   } else {
       GameManager::instance()->deleteTurret(tempTurretID);
       tempTurretID = -1;
   }
}
