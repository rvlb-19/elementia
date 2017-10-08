#include "game_libs.h"

std::vector<Item*> itemList;
std::vector<ItemHandler> itemHandlerList;
std::vector<Class*> classArray;
std::vector<Character*> party;

enum gameState {

    endGame, mainMenu, mapScreen, playLevel, mapEditor, charEditor, classEditor
};

enum editorTileSet {

    plainTileSet, forestTileSet, mountainTileSet, EDITOR_TILESET_QNT
};

enum turnState {

    player, cpu
};

volatile int ticks;
void tickCounter () { ticks++; }
END_OF_FUNCTION(tickCounter)

void initializeTestingWeapon() {

    Weapon *newWeapon = new Weapon();
    newWeapon->setItemName("Iron Sword");
    newWeapon->setRange(1);
    newWeapon->setPower(20);
    newWeapon->setWeaponType(Weapon::physical);
    itemList.push_back(newWeapon);

    ItemHandler newWeaponHandler;
    newWeaponHandler.itemName = newWeapon->getItemName();
    itemHandlerList.push_back(newWeaponHandler);
}

Character *initializeTestingChar(Class *characterClass, std::string charName, int gender, int hairColor) {

    Character *newChar = new Character();
    newChar->setHairColor(hairColor);
    newChar->setGender(gender);
    newChar->setCharName(charName);
    newChar->setClass(characterClass);
    newChar->addItem(itemHandlerList[0]);
    itemList[getItemIndex(newChar->getItem(0).itemName)]->useItem(newChar, 0);

    return newChar;
}

Enemy *initializeTestingEnemy(Class *enemyClass, std::string enemyName, int gender, int hairColor, int posI, int posJ) {

    Enemy *newEnemy = new Enemy();
    newEnemy->setHairColor(hairColor);
    newEnemy->setGender(gender);
    newEnemy->setCharName(enemyName);
    newEnemy->setClass(enemyClass);
    newEnemy->addItem(itemHandlerList[0]);
    itemList[getItemIndex(newEnemy->getItem(0).itemName)]->useItem(newEnemy, 0);

    Coordinate enemyPos;
    enemyPos.i = posI;
    enemyPos.j = posJ;

    newEnemy->setPosition(enemyPos);

    return newEnemy;
}

int mainMenuScreen() {

    int gameState = endGame;

    return gameState;
}

#define GRID_INTENSITY_MAX 5

enum charOptions {

    action, item, info, wait, cancel, CHAR_OPTIONS_QNT
};

int playLevelScreen(Level *currentLevel) {

    int gameState = mainMenu, charMoving = -1, currentEnemy, turnState = player, gridIntensity = 2;
    bool exitLoop = false, switchTurn, confirmHub = false, charSelection = true;
    unsigned i;

    BITMAP *screenBuffer = create_bitmap(SCREEN_W, SCREEN_H);

    InteractableObject camera;

    Controller gameController;

    Cursor levelCursor;
    levelCursor.setSprites(load_bitmap("misc/cursor.bmp", NULL));

    Hub selectionHub;
    selectionHub.setSprites(load_bitmap("misc/selection_hub.bmp", NULL));

    Cursor selectionCursor;
    selectionCursor.setSprites(load_bitmap("misc/cursor.bmp", NULL));

    Controller selectionController;

    Hub charInfoHub;
    charInfoHub.setSprites(load_bitmap("misc/char_info_hub.bmp", NULL));

    Hub charOptionsHub;
    charOptionsHub.setSprites(load_bitmap("misc/char_options_hub.bmp", NULL));

    Cursor charOptionsCursor;
    charOptionsCursor.setSprites(load_bitmap("misc/char_options_cursor.bmp", NULL));

    Controller charOptionsController;

    int currentOption = 0;

    Hub partyInfoHub;
    partyInfoHub.setSprites(load_bitmap("misc/party_info_hub.bmp", NULL));

    Controller partyInfoController;

    Character hubChar;

    ticks = 0;

    while(!exitLoop) {

        while(!exitLoop && ticks > 0) {

            gameController.readInput();

            if(gameController.checkKeyState(Controller::keyESC) != 0) {

                exitLoop = true;
                gameState = endGame;
            }

            if(levelCursor.getEnabled()) {

                if(gameController.checkKeyState(Controller::keyUp) != 0) {

                    if(levelCursor.getPosition().i > 0)
                        levelCursor.move(InteractableObject::up);
                }

                if(gameController.checkKeyState(Controller::keyDown) != 0) {

                    if(levelCursor.getPosition().i + 1 < currentLevel->getSize().i)
                        levelCursor.move(InteractableObject::down);
                }

                if(gameController.checkKeyState(Controller::keyLeft) != 0) {

                    if(levelCursor.getPosition().j > 0)
                        levelCursor.move(InteractableObject::left);
                }

                if(gameController.checkKeyState(Controller::keyRight) != 0) {

                    if(levelCursor.getPosition().j + 1 < currentLevel->getSize().j)
                        levelCursor.move(InteractableObject::right);
                }

                if(gameController.checkKeyState(Controller::keyCancel) == 1) {

                    levelCursor.setPressed(false);
                }

                if(gameController.checkKeyState(Controller::keyR) != 0) {

                    if(gridIntensity < GRID_INTENSITY_MAX)
                        gridIntensity++;
                }

                if(gameController.checkKeyState(Controller::keyL) != 0) {

                    if(gridIntensity > 0)
                        gridIntensity--;
                }
            }

            //Camera position

            if((levelCursor.getPosition().i >= 0) && (levelCursor.getPosition().i + 1 <= camera.getPosition().i)) {

                camera.move(InteractableObject::up);
            }

            if((levelCursor.getPosition().i >= camera.getPosition().i + CAMERA_H) && (levelCursor.getPosition().i + 1 <= currentLevel->getSize().i)) {

                camera.move(InteractableObject::down);
            }

            if((levelCursor.getPosition().j >= 0) && (levelCursor.getPosition().j + 1 <= camera.getPosition().j)) {

                camera.move(InteractableObject::left);
            }

            if((levelCursor.getPosition().j >= camera.getPosition().j + CAMERA_W) && (levelCursor.getPosition().j + 1 <= currentLevel->getSize().j)) {

                camera.move(InteractableObject::right);
            }

            draw_sprite(screenBuffer, currentLevel->getBackground(), 0, 0);

            if(!charSelection) {

                switchTurn = true;

                switch(turnState) {

                    case player:
                        for(i = 0 ; i < currentLevel->getPlayableCharQnt() && switchTurn ; i++) {

                            switchTurn = 1 - currentLevel->getCharacter(i)->isPlayable();
                        }
                        break;

                    case cpu:
                        for(i = 0 ; i < currentLevel->getEnemyQnt() && switchTurn ; i++) {

                            switchTurn = 1 - currentLevel->getEnemy(i)->isPlayable();
                        }
                        break;
                }

                if(switchTurn) {

                    if(turnState == player) {

                        currentEnemy = 0;

                        turnState = cpu;

                        for(i = 0 ; i < currentLevel->getEnemyQnt() ; i++) {

                            currentLevel->getEnemy(i)->setPlayable(true);
                        }

                    } else {

                        turnState = player;

                        for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

                            currentLevel->getCharacter(i)->setPlayable(true);
                        }
                    }
                }

                if(turnState == player) {

                    levelCursor.setVisible(true);

                    if(charMoving == -1) {

                        levelCursor.setEnabled(true);
                        charInfoHub.setEnabled(false);

                        currentLevel->resetMap();

                        for(i = 0 ; i < currentLevel->getPlayableCharQnt() && !levelCursor.getPressed() ; i++) {

                            if(currentLevel->getCharacter(i)->isPlayable()) {

                                if((levelCursor.getPosition().i == currentLevel->getCharacter(i)->getPosition().i) &&
                                   (levelCursor.getPosition().j == currentLevel->getCharacter(i)->getPosition().j)) {

                                    currentLevel->getCharacter(i)->changeAnimation(Character::cursorOn);

                                } else {

                                    currentLevel->getCharacter(i)->changeAnimation(Character::cursorOff);
                                }

                            } else {

                                currentLevel->getCharacter(i)->changeAnimation(Character::cursorOff);
                            }

                            if(currentLevel->getCharacter(i)->getAnimation() == Character::cursorOn &&
                               gameController.checkKeyState(Controller::keyAction) == 1) {

                                levelCursor.setPressed(1 - levelCursor.getPressed());
                                gameController.setKeyState(Controller::keyAction, 2);
                            }

                            currentLevel->getCharacter(i)->setSelected(levelCursor.getPressed());

                            if(currentLevel->getCharacter(i)->isSelected()) {

                                currentLevel->getCharacter(i)->changeAnimation(Character::MovingDown);
                            }
                        }

                        //Check to see if we have to draw a path

                        for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

                            if(currentLevel->getCharacter(i)->getAnimation() == Character::cursorOn || currentLevel->getCharacter(i)->isSelected()) {

                                currentLevel->generatePathOptions(currentLevel->getCharacter(i));
                                currentLevel->generateAttackOptions(currentLevel->getCharacter(i));
                                currentLevel->highlightMap(screenBuffer, currentLevel->getCharacter(i));
                                charInfoHub.setEnabled(true);
                                hubChar = *(currentLevel->getCharacter(i));

                                if(currentLevel->getCharacter(i)->isSelected()) {

                                    charInfoHub.setEnabled(false);
                                    currentLevel->setPath(levelCursor.getPosition(), currentLevel->getCharacter(i));
                                    //currentLevel->drawPath(screenBuffer, currentLevel->getCharacter(i)->getPosition(), levelCursor.getPosition());
                                }

                                if((gameController.checkKeyState(Controller::keyAction) == 1) &&
                                   (currentLevel->getCharacter(i)->getFullClassStat(Stat::move) >= currentLevel->getTileValue(levelCursor.getPosition()))) {

                                    if((currentLevel->getOccupied(levelCursor.getPosition()) == Tile::freeTile) ||
                                       ((levelCursor.getPosition().i == currentLevel->getCharacter(i)->getPosition().i) &&
                                       (levelCursor.getPosition().j == currentLevel->getCharacter(i)->getPosition().j))) {

                                        currentLevel->getCharacter(i)->changeMoving(true);
                                        currentLevel->getCharacter(i)->setLastPos(currentLevel->getCharacter(i)->getPosition());
                                        charMoving = i;
                                    }
                                }
                            }
                        }

                        if(!levelCursor.getPressed()) {

                            for(i = 0 ; i < currentLevel->getEnemyQnt() ; i++) {

                                if((levelCursor.getPosition().i == currentLevel->getEnemy(i)->getPosition().i) &&
                                   (levelCursor.getPosition().j == currentLevel->getEnemy(i)->getPosition().j)) {

                                    currentLevel->generatePathOptions(currentLevel->getEnemy(i));
                                    currentLevel->generateAttackOptions(currentLevel->getEnemy(i));
                                    currentLevel->highlightMap(screenBuffer, currentLevel->getEnemy(i));
                                    charInfoHub.setEnabled(true);
                                    hubChar = *(currentLevel->getEnemy(i));
                                }
                            }
                        }

                    } else {

                        levelCursor.setEnabled(false);

                        if((currentLevel->getCharacter(charMoving)->getPosition().i != levelCursor.getPosition().i) ||
                           (currentLevel->getCharacter(charMoving)->getPosition().j != levelCursor.getPosition().j)) {

                            Coordinate target;
                            bool targetFound = false;

                            currentLevel->setPathValue(false, currentLevel->getCharacter(charMoving)->getPosition());

                            if(currentLevel->getCharacter(charMoving)->getPosition().i > 0) {

                                target = currentLevel->getCharacter(charMoving)->getPosition();
                                target.i--;
                                targetFound = currentLevel->getPathValue(target);
                            }

                            if(currentLevel->getCharacter(charMoving)->getPosition().j > 0 && !targetFound) {

                                target = currentLevel->getCharacter(charMoving)->getPosition();
                                target.j--;
                                targetFound = currentLevel->getPathValue(target);
                            }

                            if(currentLevel->getCharacter(charMoving)->getPosition().i < currentLevel->getSize().i - 1 && !targetFound) {

                                target = currentLevel->getCharacter(charMoving)->getPosition();
                                target.i++;
                                targetFound = currentLevel->getPathValue(target);
                            }

                            if(currentLevel->getCharacter(charMoving)->getPosition().j < currentLevel->getSize().j - 1 && !targetFound) {

                                target = currentLevel->getCharacter(charMoving)->getPosition();
                                target.j++;
                                targetFound = currentLevel->getPathValue(target);
                            }

                            if(currentLevel->getPathValue(target)) {

                                if(target.i == currentLevel->getCharacter(charMoving)->getPosition().i - 1) {

                                    currentLevel->getCharacter(charMoving)->move(InteractableObject::up);
                                    currentLevel->getCharacter(charMoving)->changeAnimation(Character::MovingUp);

                                } else if(target.i == currentLevel->getCharacter(charMoving)->getPosition().i + 1) {

                                    currentLevel->getCharacter(charMoving)->move(InteractableObject::down);
                                    currentLevel->getCharacter(charMoving)->changeAnimation(Character::MovingDown);

                                } else if(target.j == currentLevel->getCharacter(charMoving)->getPosition().j - 1) {

                                    currentLevel->getCharacter(charMoving)->move(InteractableObject::left);
                                    currentLevel->getCharacter(charMoving)->changeAnimation(Character::MovingLeft);

                                } else if(target.j == currentLevel->getCharacter(charMoving)->getPosition().j + 1) {

                                    currentLevel->getCharacter(charMoving)->move(InteractableObject::right);
                                    currentLevel->getCharacter(charMoving)->changeAnimation(Character::MovingRight);
                                }
                            }

                        } else {

                            if(currentLevel->getCharacter(charMoving)->getTargetListSize() == 0) {

                                for(i = 0 ; i < currentLevel->getEnemyQnt() ; i++) {

                                    Coordinate enemyPos = currentLevel->getEnemy(i)->getPosition();

                                    int deltaI = currentLevel->getCharacter(charMoving)->getPosition().i - enemyPos.i;
                                    int deltaJ = currentLevel->getCharacter(charMoving)->getPosition().j - enemyPos.j;

                                    if(deltaI < 0) {

                                        deltaI *= -1;
                                    }

                                    if(deltaJ < 0) {

                                        deltaJ *= -1;
                                    }

                                    if(deltaI + deltaJ <= currentLevel->getCharacter(charMoving)->getFullBattleStat(Stat::range)) {

                                        currentLevel->getCharacter(charMoving)->addTarget(currentLevel->getEnemy(i));
                                    }
                                }
                            }

                            charOptionsHub.setEnabled(!partyInfoHub.getEnabled());
                        }
                    }
                }

                if(turnState == cpu) {

                    levelCursor.setVisible(false);
                    levelCursor.setEnabled(false);
                    confirmHub = false;

                    Enemy *selectedEnemy = currentLevel->getEnemy(currentEnemy);

                    if(charMoving == -1) {

                        currentLevel->resetMap();
                        currentLevel->generatePathOptions(selectedEnemy);
                        currentLevel->generateAttackOptions(selectedEnemy);
                        //currentLevel->highlightMap(screenBuffer, selectedEnemy);

                        // Add all possible attack options

                        for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

                            //
                        }

                        //Improve later

                        if(selectedEnemy->getTargetListSize() > 0) {

                        } else {

                            selectedEnemy->setPlayable(false);
                            currentEnemy++;
                        }

                    } else {

                    }
                }
            }

            if(charSelection) {

                levelCursor.setVisible(true);
                levelCursor.setEnabled(true);

                for(i = 0 ; i < currentLevel->getPlayableCharMax() ; i++) {

                    rectfill(screenBuffer, currentLevel->getStartingPos(i).j*TILE_SIZE + 1, currentLevel->getStartingPos(i).i*TILE_SIZE + 1,
                             (currentLevel->getStartingPos(i).j + 1)*TILE_SIZE - 1, (currentLevel->getStartingPos(i).i + 1)*TILE_SIZE - 1,
                             makeacol(0, 0, 255, 100));

                    if((levelCursor.getPosition().i == currentLevel->getStartingPos(i).i) &&
                       (levelCursor.getPosition().j == currentLevel->getStartingPos(i).j)) {

                           levelCursor.setPressed(gameController.checkKeyState(Controller::keyAction) == 1);
                       }
                }

                if(levelCursor.getPressed() && !selectionHub.getEnabled()) {

                    selectionController.setKeyState(Controller::keyAction, 2);

                    Coordinate hubPosition;
                    hubPosition.i = (1 + camera.getPosition().i + CAMERA_H/4);
                    hubPosition.j = (camera.getPosition().j + CAMERA_W/4);
                    selectionHub.setPosition(hubPosition);
                    selectionCursor.setPosition(selectionHub.getPosition());
                    selectionHub.setEnabled(true);
                }

                levelCursor.setEnabled(1 - (selectionHub.getEnabled() || confirmHub));
                levelCursor.setPressed(selectionHub.getEnabled());

                if(!confirmHub) {

                    if(currentLevel->getPlayableCharQnt() > 0) {

                        if((gameController.checkKeyState(Controller::keyAction) == 1) && !selectionHub.getEnabled()) {

                            confirmHub = true;
                            gameController.setKeyState(Controller::keyAction, 2);
                        }
                    }

                } else {

                    if(gameController.checkKeyState(Controller::keyAction) == 1) {

                        charSelection = false;
                        confirmHub = false;
                        gameController.setKeyState(Controller::keyAction, 2);
                    }

                    if(gameController.checkKeyState(Controller::keyCancel) == 1) {

                        confirmHub = false;
                        gameController.setKeyState(Controller::keyCancel, 2);
                    }
                }
            }

            currentLevel->drawGrid(screenBuffer, gridIntensity);

            //Draw cursors

            if(levelCursor.getVisible()) {

                masked_blit(levelCursor.getSprites(), screenBuffer, levelCursor.getPressed()*TILE_SIZE, 0,
                            levelCursor.getPosition().j*TILE_SIZE, levelCursor.getPosition().i*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }

            //Draw sprites

            for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

                Coordinate spriteIndex = currentLevel->getCharacter(i)->getSpriteIndex();

                masked_blit(currentLevel->getCharacter(i)->getSprites(), screenBuffer, spriteIndex.j*SPRITE_SIZE, spriteIndex.i*SPRITE_SIZE,
                            currentLevel->getCharacter(i)->getPosition().j*TILE_SIZE - TILE_SIZE/2,
                            currentLevel->getCharacter(i)->getPosition().i*TILE_SIZE - TILE_SIZE/2, SPRITE_SIZE, SPRITE_SIZE);
            }

            for(i = 0 ; i < currentLevel->getEnemyQnt() ; i++) {

                Coordinate spriteIndex = currentLevel->getEnemy(i)->getSpriteIndex();

                masked_blit(currentLevel->getEnemy(i)->getSprites(), screenBuffer, spriteIndex.j*SPRITE_SIZE, spriteIndex.i*SPRITE_SIZE,
                            currentLevel->getEnemy(i)->getPosition().j*TILE_SIZE - TILE_SIZE/2,
                            currentLevel->getEnemy(i)->getPosition().i*TILE_SIZE - TILE_SIZE/2, SPRITE_SIZE, SPRITE_SIZE);
            }

            if(confirmHub) {

                rectfill(screenBuffer, camera.getPosition().j*TILE_SIZE, camera.getPosition().i*TILE_SIZE,
                         camera.getPosition().j*TILE_SIZE + 80, camera.getPosition().i*TILE_SIZE + 30, makeacol(0, 0, 0, 100));

                textout_ex(screenBuffer, font, "CONFIRM?", camera.getPosition().j*TILE_SIZE + 10,
                           camera.getPosition().i*TILE_SIZE + 10, makeacol(255, 255, 255, 255), -1);
            }

            if(charInfoHub.getEnabled()) {

                Coordinate hubPosition;
                hubPosition.i = camera.getPosition().i;
                hubPosition.j = camera.getPosition().j;

                int hubPositionMod = 0;

                if((hubChar.getPosition().j - camera.getPosition().j < CAMERA_W/2) && (hubChar.getPosition().i - camera.getPosition().i < CAMERA_H/2)) {

                    hubPosition.j += CAMERA_W - (charInfoHub.getSize().j)/TILE_SIZE;
                    hubPositionMod -= (charInfoHub.getSize().j)%TILE_SIZE;
                }

                charInfoHub.setPosition(hubPosition);

                draw_sprite(screenBuffer, charInfoHub.getSprites(),
                            charInfoHub.getPosition().j*TILE_SIZE + hubPositionMod, charInfoHub.getPosition().i*TILE_SIZE);

                masked_blit(hubChar.getSprites(), screenBuffer, 0, 0,
                            charInfoHub.getPosition().j*TILE_SIZE + (charInfoHub.getSize().j/4) + hubPositionMod,
                            charInfoHub.getPosition().i*TILE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

                textprintf_ex(screenBuffer, font, charInfoHub.getPosition().j*TILE_SIZE + hubPositionMod,
                              charInfoHub.getPosition().i*TILE_SIZE + SPRITE_SIZE + (TILE_SIZE/4), makeacol(255,255,255,100), -1,
                              "%s", hubChar.getCharName().c_str());
            }

            if(selectionHub.getEnabled()) {

                selectionController.readInput();

                if(selectionController.checkKeyState(Controller::keyUp) != 0) {

                    if(selectionCursor.getPosition().i > 0)
                       selectionCursor.move(InteractableObject::up);
                }

                if(selectionController.checkKeyState(Controller::keyDown) != 0) {

                    if(selectionCursor.getPosition().i + 1 < CAMERA_H/2)
                        selectionCursor.move(InteractableObject::down);
                }

                if(selectionController.checkKeyState(Controller::keyLeft) != 0) {

                    if(selectionCursor.getPosition().j > 0)
                        selectionCursor.move(InteractableObject::left);
                }

                if(selectionController.checkKeyState(Controller::keyRight) != 0) {

                    if(selectionCursor.getPosition().j + 1 < CAMERA_W/2)
                        selectionCursor.move(InteractableObject::right);
                }

                if(selectionController.checkKeyState(Controller::keyCancel) == 1) {

                    selectionHub.setEnabled(false);
                }

                draw_sprite(screenBuffer, selectionHub.getSprites(), selectionHub.getPosition().j*TILE_SIZE, selectionHub.getPosition().i*TILE_SIZE);

                masked_blit(selectionCursor.getSprites(), screenBuffer, 0, 0, (selectionCursor.getPosition().j)*TILE_SIZE, (selectionCursor.getPosition().i)*TILE_SIZE, TILE_SIZE, TILE_SIZE);

                i = 0;

                for(int k = 0; (k < CAMERA_H/2) && (i < party.size()) ; k++) {

                    for(int j = 0 ; (j < CAMERA_W/2) && (i < party.size()) ; j++, i++) {

                        Coordinate spriteIndex = party[i]->getSpriteIndex();

                        masked_blit(party[i]->getSprites(), screenBuffer, spriteIndex.j*SPRITE_SIZE, spriteIndex.i*SPRITE_SIZE,
                                    (selectionHub.getPosition().j + j - 0.5)*TILE_SIZE, (selectionHub.getPosition().i + k - 0.5)*TILE_SIZE,
                                    SPRITE_SIZE, SPRITE_SIZE);
                    }
                }

                if(selectionController.checkKeyState(Controller::keyAction) == 1) {

                    unsigned selectedChar = (selectionCursor.getPosition().i - selectionHub.getPosition().i)*CAMERA_W/2 + (selectionCursor.getPosition().j - selectionHub.getPosition().j);

                    if(selectedChar >= party.size()) {

                        for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

                            if((currentLevel->getCharacter(i)->getPosition().i == levelCursor.getPosition().i) &&
                               (currentLevel->getCharacter(i)->getPosition().j == levelCursor.getPosition().j)) {

                                   party.push_back(currentLevel->getCharacter(i));
                                   currentLevel->removePlayableChar(i);
                                   currentLevel->setOccupied(Tile::freeTile, levelCursor.getPosition());
                               }
                        }

                    } else {

                        if(currentLevel->getOccupied(levelCursor.getPosition()) == Tile::freeTile) {

                            currentLevel->addPlayableChar(party[selectedChar]);
                            currentLevel->getCharacter(currentLevel->getPlayableCharQnt() - 1)->setPosition(levelCursor.getPosition());
                            party.erase(party.begin() + selectedChar);
                            currentLevel->setOccupied(Tile::playerTile, levelCursor.getPosition());

                        } else {

                            for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

                                if((currentLevel->getCharacter(i)->getPosition().i == levelCursor.getPosition().i) &&
                                   (currentLevel->getCharacter(i)->getPosition().j == levelCursor.getPosition().j)) {

                                       party.push_back(currentLevel->getCharacter(i));
                                       currentLevel->removePlayableChar(i);
                                   }
                            }

                            currentLevel->addPlayableChar(party[selectedChar]);
                            currentLevel->getCharacter(currentLevel->getPlayableCharQnt() - 1)->setPosition(levelCursor.getPosition());
                            party.erase(party.begin() + selectedChar);
                            currentLevel->setOccupied(Tile::playerTile, levelCursor.getPosition());
                        }
                    }

                    selectionHub.setEnabled(false);
                    gameController.setKeyState(Controller::keyAction, 2);
                }
            }

            if(charOptionsHub.getEnabled()) {

                bool enableAction = currentLevel->getCharacter(charMoving)->getTargetListSize() > 0, optionSelected = false;

                charOptionsController.readInput();

                Coordinate hubPosition;
                hubPosition.i = camera.getPosition().i;
                hubPosition.j = camera.getPosition().j;

                charInfoHub.setPosition(hubPosition);

                if(charOptionsController.checkKeyState(Controller::keyUp) != 0) {

                    if(currentOption > 0) {

                        currentOption--;
                        charOptionsCursor.move(InteractableObject::up);
                    }
                }

                if(charOptionsController.checkKeyState(Controller::keyDown) != 0) {

                    if(currentOption < CHAR_OPTIONS_QNT - 1) {

                        currentOption++;
                        charOptionsCursor.move(InteractableObject::down);
                    }
                }

                if(charOptionsController.checkKeyState(Controller::keyAction) == 1) {

                    optionSelected = true;
                }

                if(charOptionsController.checkKeyState(Controller::keyCancel) == 1) {

                    optionSelected = true;
                    currentOption = cancel;
                }

                draw_sprite(screenBuffer, charOptionsHub.getSprites(), charOptionsHub.getPosition().j*TILE_SIZE, charOptionsHub.getPosition().i*TILE_SIZE);

                std::string optionText;
                int color;

                for(i = 0 ; i < CHAR_OPTIONS_QNT ; i++) {

                    if(i == action) {

                        color = 135 + 60*(1 + enableAction);

                    } else {

                        color = 255;
                    }

                    switch(i) {

                        case action:
                            optionText = "Action";
                            break;

                        case item:
                            optionText = "Item";
                            break;

                        case info:
                            optionText = "Info";
                            break;

                        case wait:
                            optionText = "Wait";
                            break;

                        case cancel:
                            optionText = "Cancel";
                            break;
                    }

                    textprintf_ex(screenBuffer, font, charOptionsHub.getPosition().j*TILE_SIZE,
                                  charOptionsHub.getPosition().i*TILE_SIZE + i*TILE_SIZE + (TILE_SIZE/4),
                                  makeacol(color,color,color,100), -1, "%s", optionText.c_str());
                }

                masked_blit(charOptionsCursor.getSprites(), screenBuffer, 0, 0, (charOptionsHub.getPosition().j)*TILE_SIZE,
                            (charOptionsCursor.getPosition().i + charOptionsHub.getPosition().i)*TILE_SIZE, 56, TILE_SIZE);

                if(optionSelected) {

                    switch(currentOption) {

                        case action:
                            break;

                        case item:
                            break;

                        case info:
                            partyInfoHub.setEnabled(true);
                            Coordinate hubPosition;
                            hubPosition.i = 0;
                            hubPosition.j = 0;
                            partyInfoHub.setPosition(hubPosition);
                            break;

                        case wait:
                            currentLevel->getCharacter(charMoving)->changeMoving(false);
                            currentLevel->getCharacter(charMoving)->setPlayable(false);
                            currentLevel->getCharacter(charMoving)->changeAnimation(Character::cursorOff);
                            levelCursor.setPressed(false);
                            currentLevel->setOccupied(Tile::playerTile, currentLevel->getCharacter(charMoving)->getPosition());
                            currentLevel->setOccupied(Tile::freeTile, currentLevel->getCharacter(charMoving)->getLastPos());
                            break;

                        case cancel:
                            currentLevel->getCharacter(charMoving)->setPosition(currentLevel->getCharacter(charMoving)->getLastPos());
                            currentLevel->getCharacter(charMoving)->changeAnimation(Character::MovingDown);
                            break;
                    }

                    if((currentOption == wait) || (currentOption == cancel)) {

                        Coordinate resetPos;
                        resetPos.i = 0;
                        resetPos.j = 0;
                        charOptionsCursor.setPosition(resetPos);

                        unsigned targetListSize = currentLevel->getCharacter(charMoving)->getTargetListSize();

                        for(i = 0 ; i < targetListSize ; i++) {

                            currentLevel->getCharacter(charMoving)->removeTarget(0);
                        }

                        currentOption = 0;
                        charMoving = -1;
                    }

                    charOptionsHub.setEnabled(false);
                }
            }

            if(partyInfoHub.getEnabled()) {

                partyInfoController.readInput();

                draw_sprite(screenBuffer, partyInfoHub.getSprites(), partyInfoHub.getPosition().j*TILE_SIZE, partyInfoHub.getPosition().i*TILE_SIZE);

                masked_blit(hubChar.getSprites(), screenBuffer, 0, 0, partyInfoHub.getPosition().j*TILE_SIZE, partyInfoHub.getPosition().i*TILE_SIZE, SPRITE_SIZE, SPRITE_SIZE);

                textprintf_ex(screenBuffer, font, partyInfoHub.getPosition().j*TILE_SIZE + SPRITE_SIZE, partyInfoHub.getPosition().i*TILE_SIZE + (TILE_SIZE/4),
                              makeacol(255,255,255,100), -1, "%s", hubChar.getCharName().c_str());

                std::string statText;
                Coordinate statTextPos;

                for(i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

                    switch(i) {

                        case (Stat::health):
                            statText = hubChar.getCurrentHealth() + "/";
                            break;

                        case (Stat::strength):
                            statText = "Str ";
                            break;

                        case (Stat::defense):
                            statText = "Def ";
                            break;

                        case (Stat::magic):
                            statText = "Mag ";
                            break;

                        case (Stat::resistance):
                            statText = "Res ";
                            break;

                        case (Stat::dexterity):
                            statText = "Dex ";
                            break;

                        case (Stat::luck):
                            statText = "Lck ";
                            break;

                        case (Stat::speed):
                            statText = "Spd ";
                            break;

                        case (Stat::move):
                            statText = "Mov ";
                            break;
                    }

                    if((i != (Stat::health)) && (i != (Stat::move))) {

                        statTextPos = partyInfoHub.getPosition();
                        statTextPos.i += i - 1;
                        textprintf_ex(screenBuffer, font, statTextPos.j*TILE_SIZE, statTextPos.i*TILE_SIZE + (TILE_SIZE/4) + SPRITE_SIZE,
                                      makeacol(255,255,255,100), -1, "%s%d", statText.c_str(), hubChar.getClassStat(i).baseStat);
                    }
                }

                if(partyInfoController.checkKeyState(Controller::keyCancel) == 1) {

                    partyInfoHub.setEnabled(false);
                    charOptionsController.setKeyState(Controller::keyCancel, 2);
                }
            }

            //Draw buffer

            stretch_blit(screenBuffer, screen, camera.getPosition().j*TILE_SIZE, camera.getPosition().i*TILE_SIZE,
                         CAMERA_W*TILE_SIZE, CAMERA_H*TILE_SIZE, 0, 0, SCREEN_W, SCREEN_H);

            clear(screenBuffer);
            ticks--;
        }
    }

    for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

        party.push_back(currentLevel->getCharacter(i));
    }

    for(i = 0 ; i < currentLevel->getPlayableCharQnt() ; i++) {

        currentLevel->removePlayableChar(i);
    }

    destroy_bitmap(screenBuffer);

    return gameState;
}

#define MAP_EDITOR_TILESET_SIZE_I 7
#define MAP_EDITOR_TILESET_SIZE_J 6

int mapEditorScreen(std::string levelIndex) {

    std::string pathData = "levels/" + levelIndex + "/data.lev";
    std::string pathBMP = "levels/" + levelIndex + "/background.bmp";

    int gameState = mainMenu, editorTileSet = plainTileSet, terrainType, playableCharQnt = 0, i, j;
    bool exitLoop = false, emptyData = false, hubEnabled = false, placeStartingPos = false, posFound = false;
    Coordinate startingPos;
    std::vector<Coordinate> charStartingPos;
    unsigned index;

    FILE *levelData = fopen(pathData.c_str(), "r");
    Coordinate levelSize;
    fscanf(levelData, "%d %d", &levelSize.i, &levelSize.j);

    int **levelMap = new int*[levelSize.i];

    for(i = 0 ; i < levelSize.i ; i++) {

        levelMap[i] = new int[levelSize.j];
    }

    BITMAP *levelBackground = load_bitmap(pathBMP.c_str(), NULL);

    if(levelBackground == NULL) {

        levelBackground = create_bitmap(TILE_SIZE*levelSize.j, TILE_SIZE*levelSize.i);
        clear(levelBackground);
        emptyData = true;
    }

    if(emptyData) {

        for(i = 0 ; i < levelSize.i ; i++) {

            for(j = 0 ; j < levelSize.j ; j++) {

                levelMap[i][j] = -1;
            }
        }

    } else {

        for(i = 0 ; i < levelSize.i ; i++) {

            for(j = 0 ; j < levelSize.j ; j++) {

                fscanf(levelData, "%d", &levelMap[i][j]);
            }
        }

        fscanf(levelData, "%d", &playableCharQnt);

        for(i = 0 ; i < playableCharQnt ; i++) {

            fscanf(levelData, "%d %d", &startingPos.i, &startingPos.j);
            charStartingPos.push_back(startingPos);
        }
    }

    fclose(levelData);

    BITMAP *screenBuffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *plainTiles = load_bitmap("editor/plain_tiles.bmp", NULL);
    BITMAP *forestTiles = load_bitmap("editor/forest_tiles.bmp", NULL);
    BITMAP *mountainTiles = load_bitmap("editor/mountain_tiles.bmp", NULL);
    BITMAP *currentTileSet = NULL;

    InteractableObject camera;

    Controller gameController;

    Cursor levelCursor, tileSetCursor;
    levelCursor.setSprites(load_bitmap("misc/cursor.bmp", NULL));
    tileSetCursor.setSprites(load_bitmap("misc/cursor.bmp", NULL));

    ticks = 0;

    while(!exitLoop) {

        while(!exitLoop && ticks > 0) {

            gameController.readInput();

            switch(editorTileSet) {

                case plainTileSet:
                    currentTileSet = plainTiles;
                    terrainType = Tile::plain;
                    break;

                case forestTileSet:
                    currentTileSet = forestTiles;
                    terrainType = Tile::forest;
                    break;

                case mountainTileSet:
                    currentTileSet = mountainTiles;
                    terrainType = Tile::mountain;
                    break;
            }

            if(gameController.checkKeyState(Controller::keyESC) != 0) {

                exitLoop = true;
                gameState = endGame;
            }

            if(!hubEnabled) {

                if(gameController.checkKeyState(Controller::keyUp) != 0) {

                    if(levelCursor.getPosition().i > 0)
                        levelCursor.move(InteractableObject::up);
                }

                if(gameController.checkKeyState(Controller::keyDown) != 0) {

                    if(levelCursor.getPosition().i + 1 < levelSize.i)
                        levelCursor.move(InteractableObject::down);
                }

                if(gameController.checkKeyState(Controller::keyLeft) != 0) {

                    if(levelCursor.getPosition().j > 0)
                        levelCursor.move(InteractableObject::left);
                }

                if(gameController.checkKeyState(Controller::keyRight) != 0) {

                    if(levelCursor.getPosition().j + 1 < levelSize.j)
                        levelCursor.move(InteractableObject::right);
                }

                if(gameController.checkKeyState(Controller::keyAction) == 1) {

                    if(!placeStartingPos) {

                        hubEnabled = true;
                        gameController.setKeyState(Controller::keyAction, 2);

                    } else {

                        for(index = 0, posFound = false ; index < charStartingPos.size() && posFound == false ; index++) {

                            posFound = (charStartingPos[index].i == levelCursor.getPosition().i &&
                                        charStartingPos[index].j == levelCursor.getPosition().j);
                        }

                        if(posFound) {

                            playableCharQnt--;
                            charStartingPos.erase(charStartingPos.begin() + (index - 1));

                        } else {

                            playableCharQnt++;
                            charStartingPos.push_back(levelCursor.getPosition());
                        }
                    }
                }

                if(gameController.checkKeyState(Controller::keyR) == 1 || gameController.checkKeyState(Controller::keyL) == 1) {

                    placeStartingPos = 1 - placeStartingPos;
                }

            } else {

                if(gameController.checkKeyState(Controller::keyCancel) != 0) {

                    hubEnabled = false;
                }

                if(gameController.checkKeyState(Controller::keyUp) != 0) {

                    if(tileSetCursor.getPosition().i > 0)
                        tileSetCursor.move(InteractableObject::up);
                }

                if(gameController.checkKeyState(Controller::keyDown) != 0) {

                    if(tileSetCursor.getPosition().i + 1 < MAP_EDITOR_TILESET_SIZE_I)
                        tileSetCursor.move(InteractableObject::down);
                }

                if(gameController.checkKeyState(Controller::keyLeft) != 0) {

                    if(tileSetCursor.getPosition().j > 0)
                        tileSetCursor.move(InteractableObject::left);
                }

                if(gameController.checkKeyState(Controller::keyRight) != 0) {

                    if(tileSetCursor.getPosition().j + 1 < MAP_EDITOR_TILESET_SIZE_J)
                        tileSetCursor.move(InteractableObject::right);
                }

                if(gameController.checkKeyState(Controller::keyAction) == 1) {

                    levelMap[levelCursor.getPosition().i][levelCursor.getPosition().j] = terrainType;

                    blit(currentTileSet, levelBackground, tileSetCursor.getPosition().j*TILE_SIZE, tileSetCursor.getPosition().i*TILE_SIZE,
                         levelCursor.getPosition().j*TILE_SIZE, levelCursor.getPosition().i*TILE_SIZE, TILE_SIZE, TILE_SIZE);

                    gameController.setKeyState(Controller::keyAction, 2);
                    hubEnabled = false;
                }

                if(gameController.checkKeyState(Controller::keyL) != 0) {

                    editorTileSet--;

                    if(editorTileSet == -1) {

                        editorTileSet = EDITOR_TILESET_QNT - 1;
                    }
                }

                if(gameController.checkKeyState(Controller::keyR) != 0) {

                    editorTileSet++;

                    if(editorTileSet == EDITOR_TILESET_QNT) {

                        editorTileSet = 0;
                    }
                }
            }

            //Camera position

            if((levelCursor.getPosition().i >= 0) && (levelCursor.getPosition().i + 1 <= camera.getPosition().i)) {

                camera.move(InteractableObject::up);
            }

            if((levelCursor.getPosition().i >= camera.getPosition().i + CAMERA_H) && (levelCursor.getPosition().i + 1 <= levelSize.i)) {

                camera.move(InteractableObject::down);
            }

            if((levelCursor.getPosition().j >= 0) && (levelCursor.getPosition().j + 1 <= camera.getPosition().j)) {

                camera.move(InteractableObject::left);
            }

            if((levelCursor.getPosition().j >= camera.getPosition().j + CAMERA_W) && (levelCursor.getPosition().j + 1 <= levelSize.j)) {

                camera.move(InteractableObject::right);
            }

            draw_sprite(screenBuffer, levelBackground, 0, 0);

            if(placeStartingPos) {

                for(i = 0 ; i < playableCharQnt ; i++) {

                    startingPos = charStartingPos[i];
                    rectfill(screenBuffer, startingPos.j*TILE_SIZE, startingPos.i*TILE_SIZE,
                             (startingPos.j + 1)*TILE_SIZE - 1, (startingPos.i + 1)*TILE_SIZE - 1, makeacol(0, 0, 255, 100));
                }
            }

            masked_blit(levelCursor.getSprites(), screenBuffer, placeStartingPos*TILE_SIZE, 0,
                        levelCursor.getPosition().j*TILE_SIZE, levelCursor.getPosition().i*TILE_SIZE, TILE_SIZE, TILE_SIZE);

            if(hubEnabled) {

                rectfill(screenBuffer, (camera.getPosition().j + 6)*TILE_SIZE, (camera.getPosition().i + 3)*TILE_SIZE,
                         (camera.getPosition().j + 14)*TILE_SIZE, (camera.getPosition().i + 12)*TILE_SIZE, makeacol(255, 255, 255, 255));

                blit(currentTileSet, screenBuffer, 0, 0, (camera.getPosition().j + 7)*TILE_SIZE, (camera.getPosition().i + 4)*TILE_SIZE,
                     MAP_EDITOR_TILESET_SIZE_J*TILE_SIZE, MAP_EDITOR_TILESET_SIZE_I*TILE_SIZE);

                masked_blit(tileSetCursor.getSprites(), screenBuffer, tileSetCursor.getPressed()*TILE_SIZE, 0,
                           (camera.getPosition().j + tileSetCursor.getPosition().j + 7)*TILE_SIZE,
                           (camera.getPosition().i + tileSetCursor.getPosition().i + 4)*TILE_SIZE, TILE_SIZE, TILE_SIZE);
            }

            //Draw buffer

            stretch_blit(screenBuffer, screen, camera.getPosition().j*TILE_SIZE, camera.getPosition().i*TILE_SIZE,
                         CAMERA_W*TILE_SIZE, CAMERA_H*TILE_SIZE, 0, 0, SCREEN_W, SCREEN_H);

            clear(screenBuffer);
            ticks--;
        }
    }

    save_bitmap(pathBMP.c_str(), levelBackground, NULL);

    levelData = fopen(pathData.c_str(), "w");

    fprintf(levelData, "%d %d\n", levelSize.i, levelSize.j);

    for(i = 0 ; i < levelSize.i ; i++) {

        for(j = 0 ; j < levelSize.j ; j++) {

            fprintf(levelData, " %2d", levelMap[i][j]);
        }

        fprintf(levelData, "\n");
    }

    fprintf(levelData, "%d\n", playableCharQnt);

    for(i = 0 ; i < playableCharQnt ; i++) {

        startingPos = charStartingPos[i];
        fprintf(levelData, "%d %d\n", startingPos.i, startingPos.j);
    }

    fclose(levelData);

    destroy_bitmap(levelBackground);
    destroy_bitmap(plainTiles);
    destroy_bitmap(forestTiles);
    destroy_bitmap(mountainTiles);
    destroy_bitmap(screenBuffer);

    return gameState;
}

enum classEditorState {

    classNameState, statCapsState,
};

int classEditorScreen(std::string classPath) {

    //change the class file

    std::string classDataPath = "classes/" + classPath + "/data.class";
    std::string classSpritesPath = "classes/" + classPath + "/sprites.bmp";

    int gameState = mainMenu, animationTimer = 0;
    bool exitLoop = false;
    char tempClassName[25];

    //

    std::string className;
    int statCaps[Stat::CLASS_STAT_QNT], terrainMod[Tile::TERRAIN_QNT], occupationMod[Tile::TILE_OCCUPIED_QNT], maxLevel;
    BITMAP *classSprites = load_bitmap(classSpritesPath.c_str(), NULL);

    //

    Controller gameController;

    FILE *classData = fopen(classDataPath.c_str(), "r");

    if(classData != NULL) {

        fscanf(classData, "%s", tempClassName);

        className = tempClassName;

        for(int i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

            fscanf(classData, "%d", &statCaps[i]);
        }

        for(int i = 0 ; i < Tile::TERRAIN_QNT ; i++) {

            fscanf(classData, "%d", &terrainMod[i]);
        }

        for(int i = 0 ; i < Tile::TILE_OCCUPIED_QNT ; i++) {

            fscanf(classData, "%d", &occupationMod[i]);
        }

        fscanf(classData, "%d", &maxLevel);

        fclose(classData);

    } else {

        className = "CLASS";

        for(int i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

            statCaps[i] = 0;
        }

        for(int i = 0 ; i < Tile::TERRAIN_QNT ; i++) {

            terrainMod[i] = 1;
        }

        for(int i = 0 ; i < Tile::TILE_OCCUPIED_QNT ; i++) {

            occupationMod[i] = 0;
        }

        maxLevel = 1;
    }

    BITMAP *screenBuffer = create_bitmap(SCREEN_W, SCREEN_H);

    ticks = 0;

    while(!exitLoop) {

        while(!exitLoop && ticks > 0) {

            gameController.readInput();

            if(gameController.checkKeyState(Controller::keyESC) != 0) {

                exitLoop = true;
                gameState = endGame;
            }



            clear(screenBuffer);
            ticks--;
        }
    }

    //Write data back into file

    classData = fopen(classDataPath.c_str(), "w");

    fprintf(classData, "%s\n", className.c_str());

    for(int i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

        fprintf(classData, "%d ", statCaps[i]);
    }

    fprintf(classData, "\n");

    for(int i = 0 ; i < Tile::TERRAIN_QNT ; i++) {

        fprintf(classData, "%d ", terrainMod[i]);
    }

    fprintf(classData, "\n");

    for(int i = 0 ; i < Tile::TILE_OCCUPIED_QNT ; i++) {

        fprintf(classData, "%d ", occupationMod[i]);
    }

    fprintf(classData, "\n");
    fprintf(classData, "%d\n", maxLevel);

    fclose(classData);

    //

    destroy_bitmap(classSprites);
    destroy_bitmap(screenBuffer);

    return gameState;
}

inline void init() {

	allegro_init();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);

	set_alpha_blender();
	drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);

	install_timer();
	install_keyboard();
	install_mouse();
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);

	ticks = 0;
	LOCK_FUNCTION(tickCounter);
	LOCK_VARIABLE(ticks);
	install_int_ex(tickCounter, BPS_TO_TIMER(7));
}

inline void deinit() {

	clear_keybuf();
	allegro_exit();
}

int main() {

	init();

	srand(time(NULL));

	std::vector<Level*> levelArray;
	levelArray.push_back(new Level("1"));

    classArray.push_back(new Class("lord"));

    initializeTestingWeapon();

	party.push_back(initializeTestingChar(classArray[0], "Char1", Character::male, Character::green));
    party.push_back(initializeTestingChar(classArray[0], "Char2", Character::male, Character::blue));
    party.push_back(initializeTestingChar(classArray[0], "Char3", Character::male, Character::green));

    levelArray[0]->addEnemy(initializeTestingEnemy(classArray[0], "Enemy1", Character::male, Character::red, 10, 11));
    levelArray[0]->setOccupied(Tile::enemyTile, levelArray[0]->getEnemy(0)->getPosition());

    levelArray[0]->addEnemy(initializeTestingEnemy(classArray[0], "Enemy2", Character::male, Character::white, 8, 7));
    levelArray[0]->setOccupied(Tile::enemyTile, levelArray[0]->getEnemy(1)->getPosition());

	int gameState = playLevel, nextLevel = 0;

	while(gameState != endGame) {

	    switch(gameState) {

            case mainMenu:
                gameState = mainMenuScreen();
                break;

            case mapScreen:
                break;

            case playLevel:
                gameState = playLevelScreen(levelArray[nextLevel]);
                break;

            case mapEditor:
                gameState = mapEditorScreen("1");
                break;

            case classEditor:
                gameState = classEditorScreen("lord");
                break;
	    }
	}

    for(unsigned i = 0 ; i < levelArray.size() ; i++) {

        delete levelArray[i];
    }

    for(unsigned i = 0 ; i < classArray.size() ; i++) {

        delete classArray[i];
    }

    for(unsigned i = 0 ; i < party.size() ; i++) {

        delete party[i];
    }

	deinit();
	return 0;
}
END_OF_MAIN()
