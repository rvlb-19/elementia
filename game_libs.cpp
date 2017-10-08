#include "game_libs.h"

Controller::Controller() {

    for(int i = 0 ; i < CONTROLLER_KEY_QNT ; i++) {

        this->keyArray[i] = 0;
    }
}

Controller::~Controller() {

}

void Controller::readInput() {

    if(key[KEY_UP]) {

        if(this->keyArray[keyUp] == 0) {

            this->keyArray[keyUp] = 1;

        } else {

            this->keyArray[keyUp] = 2;
        }

    } else {

        this->keyArray[keyUp] = 0;
    }

    if(key[KEY_DOWN]) {

        if(this->keyArray[keyDown] == 0) {

            this->keyArray[keyDown] = 1;

        } else {

            this->keyArray[keyDown] = 2;
        }

    } else {

        this->keyArray[keyDown] = 0;
    }

    if(key[KEY_LEFT]) {

        if(this->keyArray[keyLeft] == 0) {

            this->keyArray[keyLeft] = 1;

        } else {

            this->keyArray[keyLeft] = 2;
        }

    } else {

        this->keyArray[keyLeft] = 0;
    }

    if(key[KEY_RIGHT]) {

        if(this->keyArray[keyRight] == 0) {

            this->keyArray[keyRight] = 1;

        } else {

            this->keyArray[keyRight] = 2;
        }

    } else {

        this->keyArray[keyRight] = 0;
    }

    if(key[KEY_A]) {

        if(this->keyArray[keyAction] == 0) {

            this->keyArray[keyAction] = 1;

        } else {

            this->keyArray[keyAction] = 2;
        }

    } else {

        this->keyArray[keyAction] = 0;
    }

    if(key[KEY_B]) {

        if(this->keyArray[keyCancel] == 0) {

            this->keyArray[keyCancel] = 1;

        } else {

            this->keyArray[keyCancel] = 2;
        }

    } else {

        this->keyArray[keyCancel] = 0;
    }

    if(key[KEY_ESC]) {

        if(this->keyArray[keyESC] == 0) {

            this->keyArray[keyESC] = 1;

        } else {

            this->keyArray[keyESC] = 2;
        }

    } else {

        this->keyArray[keyESC] = 0;
    }

    if(key[KEY_L]) {

        if(this->keyArray[keyL] == 0) {

            this->keyArray[keyL] = 1;

        } else {

            this->keyArray[keyL] = 2;
        }

    } else {

        this->keyArray[keyL] = 0;
    }

    if(key[KEY_R]) {

        if(this->keyArray[keyR] == 0) {

            this->keyArray[keyR] = 1;

        } else {

            this->keyArray[keyR] = 2;
        }

    } else {

        this->keyArray[keyR] = 0;
    }
}

int Controller::checkKeyState(int controllerKey) {

    return this->keyArray[controllerKey];
}

void Controller::setKeyState(int controllerKey, int state) {

    this->keyArray[controllerKey] = state;
}

InteractableObject::InteractableObject() {

    this->position.i = 0;
    this->position.j = 0;

    this->sprites = NULL;
}

InteractableObject::~InteractableObject() {

    if(this->sprites != NULL) {

        destroy_bitmap(this->sprites);
    }
}

Coordinate InteractableObject::getPosition() {

    return this->position;
}

void InteractableObject::setPosition(Coordinate newPos) {

    this->position = newPos;
}

BITMAP *InteractableObject::getSprites() {

    return this->sprites;
}

void InteractableObject::setSprites(BITMAP *sprites) {

    this->sprites = sprites;
}

void InteractableObject::move(int direction) {

    switch(direction) {

        case up:
            this->position.i--;
            break;

        case down:
            this->position.i++;
            break;

        case left:
            this->position.j--;
            break;

        case right:
            this->position.j++;
            break;
    }
}

Cursor::Cursor() : InteractableObject() {

    this->pressed = false;
    this->enabled = false;
    this->visible = false;
}

Cursor::~Cursor() {

}

bool Cursor::getPressed() {

    return this->pressed;
}

void Cursor::setPressed(bool value) {

    this->pressed = value;
}

bool Cursor::getEnabled() {

    return this->enabled;
}

void Cursor::setEnabled(bool value) {

    this->enabled = value;
}

bool Cursor::getVisible() {

    return this->visible;
}

void Cursor::setVisible(bool value) {

    this->visible = value;
}

Hub::Hub() : InteractableObject() {

    this->hubEnabled = false;
}

Hub::~Hub() {

}

bool Hub::getEnabled() {

    return this->hubEnabled;
}

void Hub::setEnabled(bool status) {

    this->hubEnabled = status;
}

Coordinate Hub::getSize() {

    Coordinate hubSize;

    hubSize.i = this->getSprites()->h;
    hubSize.j = this->getSprites()->w;

    return hubSize;
}

Class::Class() {

    this->className = "";
    this->classSprites = NULL;
    this->classElement = -1;
}

Class::Class(std::string classPath) {

    std::string classDataPath = "classes/" + classPath + "/data.class";
    std::string classSpritesPath = "classes/" + classPath + "/sprites.bmp";

    this->classSprites = load_bitmap(classSpritesPath.c_str(), NULL);

    FILE* classData = fopen(classDataPath.c_str(), "r");

    char tempClassName[100];

    fscanf(classData, "%s", tempClassName);

    this->className = tempClassName;

    for(int i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

        fscanf(classData, "%d", &this->statCaps[i]);
    }

    for(int i = 0 ; i < Tile::TERRAIN_QNT ; i++) {

        fscanf(classData, "%d", &this->terrainMod[i]);
    }

    for(int i = 0 ; i < Tile::TILE_OCCUPIED_QNT ; i++) {

        fscanf(classData, "%d", &this->occupationMod[i]);
    }

    fscanf(classData, "%d", &this->maxLevel);

    fclose(classData);
}

Class::~Class() {

    if(this->classSprites != NULL) {

        destroy_bitmap(this->classSprites);
    }
}

void Class::setClassName(std::string className) {

    this->className = className;
}

std::string Class::getClassName() {

    return this->className;
}

void Class::setClassSprites(BITMAP *classSprites) {

    this->classSprites = classSprites;
}

BITMAP *Class::getClassSprites(int hairColor, int gender) {

    Coordinate size;
    size.i = SPRITE_SIZE*Character::ANIMATIONS_STEPS;
    size.j = SPRITE_SIZE*Character::CHAR_ANIMATION_QNT;

    BITMAP *charSprites = create_bitmap(size.j, size.i);
    clear(charSprites);

    blit(this->classSprites, charSprites, size.j*gender, size.i*hairColor, 0, 0, size.j, size.i);

    return charSprites;
}

void Class::setStatCap(int stat, int cap) {

    this->statCaps[stat] = cap;
}

int Class::getStatCap(int stat) {

    return this->statCaps[stat];
}

void Class::setTerrainMod(int terrain, int mod) {

    this->terrainMod[terrain] = mod;
}

int Class::getTerrainMod(int terrain) {

    return this->terrainMod[terrain];
}

void Class::setOccupationMod(int tileOccupied, int mod) {

    this->occupationMod[tileOccupied] = mod;
}

int Class::getOccupationMod(int tileOccupied) {

    return this->occupationMod[tileOccupied];
}

void Class::setMaxLevel(int level) {

    this->maxLevel = level;
}

int Class::getMaxLevel() {

    return this->maxLevel;
}

void Class::setElement(int element) {

    this->classElement = element;
}

int Class::getElement() {

    return this->classElement;
}

Character::Character() : InteractableObject() {

    this->animation = cursorOff;
    this->animationStep = 0;
    this->selected = false;
    this->playable = true;
    this->moving = false;
    this->equippedWeapon = -1;
    this->charName = "";
    this->currentHealth = 0;
}

Character::~Character() {

    this->characterClass = NULL;
}

int Character::getOccupationMod(int tileOccupied) {

    return this->characterClass->getOccupationMod(tileOccupied);
}

void Character::setCharName(std::string charName) {

    this->charName = charName;
}

std::string Character::getCharName() {

    return this->charName;
}

Coordinate Character::getSpriteIndex() {

    Coordinate index;
    index.i = this->animationStep;
    index.j = this->animation;

    this->animationStep == ANIMATIONS_STEPS - 1 ? this->animationStep = 0 : this->animationStep++;

    return index;
}

void Character::setLastPos(Coordinate pos) {

    this->lastPos = pos;
}

Coordinate Character::getLastPos() {

    return this->lastPos;
}

void Character::changeAnimation(int newAnimation) {

    this->animation = newAnimation;
}

int Character::getAnimation() {

    return this->animation;
}

void Character::setSelected(bool status) {

    this->selected = status;
}

bool Character::isSelected() {

    return this->selected;
}

void Character::setPlayable(bool status) {

    this->playable = status;
}

bool Character::isPlayable() {

    return this->playable;
}

void Character::changeMoving(bool status) {

    this->moving = status;
}

bool Character::isMoving() {

    return this->moving;
}

void Character::addItem(ItemHandler newItem) {

    this->inventory.push_back(newItem);
}

ItemHandler Character::getItem(int index) {

    return this->inventory[index];
}

std::vector<ItemHandler> Character::getInventory() {

    return this->inventory;
}

void Character::setClass(Class *characterClass) {

    this->characterClass = characterClass;
    this->setSprites(this->characterClass->getClassSprites(this->hairColor, this->gender));

    Stat move;
    move.baseStat = 5;
    this->setClassStat(move, Stat::move);
}

Class *Character::getClass() {

    return this->characterClass;
}

int Character::getGender() {

    return this->gender;
}

void Character::setGender(int gender) {

    this->gender = gender;
}

int Character::getHairColor() {

    return this->hairColor;
}

void Character::setHairColor(int hairColor) {

    this->hairColor = hairColor;
}

void Character::setClassStat(Stat newStat, int stat) {

    this->stats[stat] = newStat;
}

Stat Character::getClassStat(int stat) {

    return this->stats[stat];
}

int Character::getFullClassStat(int stat) {

    return this->stats[stat].baseStat;
}

void Character::setBattleStat(Stat newStat, int stat) {

    this->battleStats[stat] = newStat;
}

Stat Character::getBattleStat(int stat) {

    return this->battleStats[stat];
}

int Character::getFullBattleStat(int stat) {

    return this->battleStats[stat].baseStat;
}

void Character::setEquippedWeapon(int index) {

    this->equippedWeapon = index;
}

int Character::getEquippedWeapon() {

    return this->equippedWeapon;
}

void Character::addTarget(Character *target) {

    this->targets.push_back(target);
}

void Character::removeTarget(int index) {

    this->targets.erase(this->targets.begin() + index);
}

Character *Character::getTarget(int index) {

    return this->targets[index];
}

unsigned Character::getTargetListSize() {

    return this->targets.size();
}

void Character::setCurrentHealth(int health) {

    this->currentHealth = health;
}

int Character::getCurrentHealth() {

    return this->currentHealth;
}

Enemy::Enemy() : Character() {

}

Enemy::~Enemy() {

}

int Enemy::getOccupationMod(int tileOccupied) {

    if(tileOccupied == Tile::enemyTile) {

        tileOccupied = Tile::playerTile;

    } else if(tileOccupied == Tile::playerTile) {

        tileOccupied = Tile::enemyTile;
    }

    return this->getClass()->getOccupationMod(tileOccupied);
}

Item::Item() {

    this->itemName = "";
    this->uses = 0;
}

Item::Item(std::string itemName, int uses) {

    this->itemName = itemName;
    this->uses = uses;
}

Item::~Item() {

}

void Item::setItemName(std::string itemName) {

    this->itemName = itemName;
}

std::string Item::getItemName() {

    return this->itemName;
}

void Item::setUses(int uses) {

    this->uses = uses;
}

int Item::getUses() {

    return this->uses;
}

Weapon::Weapon() : Item() {

    this->range = 0;
    this->power = 0;
    this->weaponType = -1;
}

Weapon::~Weapon() {

}

int Weapon::getWeaponType() {

    return this->weaponType;
}

void Weapon::setWeaponType(int weaponType) {

    this->weaponType = weaponType;
}

void Weapon::setRange(int range) {

    this->range = range;
}

int Weapon::getRange() {

    return this->range;
}

void Weapon::setPower(int power) {

    this->power = power;
}

int Weapon::getPower() {

    return this->power;
}

void Weapon::useItem(Character *user, int index) {

    if(user->getEquippedWeapon() != -1) {

        for(unsigned i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

        }

        Stat range = user->getBattleStat(Stat::range);
        range.baseStat -= this->range;

        user->setBattleStat(range, Stat::range);
    }

    user->setEquippedWeapon(index);

    if(user->getEquippedWeapon() != -1) {

        for(unsigned i = 0 ; i < Stat::CLASS_STAT_QNT ; i++) {

        }

        Stat stat = (this->weaponType == Weapon::physical ? user->getClassStat(Stat::strength) : user->getClassStat(Stat::magic));

        Stat attack;
        attack.baseStat = stat.baseStat + this->power;
        user->setBattleStat(attack, Stat::attack);

        Stat range;
        range.baseStat = this->range;
        user->setBattleStat(range, Stat::range);
    }
}

Level::Level() {

    this->levelSize.i = 0;
    this->levelSize.j = 0;

    this->levelBackground = NULL;

    this->levelMap = NULL;
}

Level::Level(std::string levelIndex) {

    std::string levelDataPath = "levels/" + levelIndex + "/data.lev";
    std::string levelBackgroundPath = "levels/" + levelIndex + "/background.bmp";

    Coordinate startPos;

    FILE *levelData = fopen(levelDataPath.c_str(), "r");

    fscanf(levelData, "%d %d", &this->levelSize.i, &this->levelSize.j);

    this->levelMap = new Tile*[this->levelSize.i];

    for(int i = 0 ; i < this->levelSize.i ; i++) {

        this->levelMap[i] = new Tile[this->levelSize.j];

        for(int j = 0 ; j < this->levelSize.j ; j++) {

            fscanf(levelData, "%d", &this->levelMap[i][j].terrain);
            this->levelMap[i][j].occupied = Tile::freeTile;
            this->levelMap[i][j].value = -1;
            this->levelMap[i][j].path = false;
            this->levelMap[i][j].attack = false;
        }
    }

    fscanf(levelData, "%d", &this->playableCharMax);

    for(unsigned i = 0; i < this->playableCharMax ; i++) {

        fscanf(levelData, "%d %d", &startPos.i, &startPos.j);
        this->charStartingPos.push_back(startPos);
    }

    this->levelBackground = load_bitmap(levelBackgroundPath.c_str(), NULL);

    fclose(levelData);
}

Level::~Level() {

    if(this->levelBackground != NULL) {

        destroy_bitmap(this->levelBackground);
    }

    if(this->levelMap != NULL) {

        for(int i = 0 ; i < this->levelSize.i ; i++) {

            delete [] this->levelMap[i];
        }

        delete [] this->levelMap;
    }

    for(unsigned i = 0 ; i < this->enemies.size() ; i++) {

        delete this->enemies[i];
    }
}

Coordinate Level::getSize() {

    return this->levelSize;
}

unsigned Level::getPlayableCharQnt() {

    return this->playableCharacters.size();
}

unsigned Level::getPlayableCharMax() {

    return this->playableCharMax;
}

Coordinate Level::getStartingPos(int index) {

    return this->charStartingPos[index];
}

Character *Level::getCharacter(int index) {

    return this->playableCharacters[index];
}

void Level::addPlayableChar(Character *newChar) {

    this->playableCharacters.push_back(newChar);
}

void Level::removePlayableChar(int index) {

    this->playableCharacters.erase(this->playableCharacters.begin() + index);
}

unsigned Level::getEnemyQnt() {

    return this->enemies.size();
}

Enemy *Level::getEnemy(int index) {

    return this->enemies[index];
}

void Level::addEnemy(Enemy *newEnemy) {

    this->enemies.push_back(newEnemy);
}

void Level::removeEnemy(int index) {

    this->enemies.erase(this->enemies.begin() + index);
}

int Level::getTileValue(Coordinate pos) {

    return this->levelMap[pos.i][pos.j].value;
}

bool Level::getTileAttack(Coordinate pos) {

    return this->levelMap[pos.i][pos.j].attack;
}

void Level::setTerrain(int terrain, Coordinate tile) {

    this->levelMap[tile.i][tile.j].terrain = terrain;
}

void Level::generatePathOptions(Character *currentChar) {

    std::vector<Coordinate> queue;
    Coordinate current = currentChar->getPosition(), target;
    int tileMod;

    this->levelMap[current.i][current.j].value = 0;
    queue.push_back(current);

    while(!queue.empty()) {

        current = queue.front();

        if(current.i > 0) {

            target.i = current.i - 1;
            target.j = current.j;

            tileMod = currentChar->getClass()->getTerrainMod(this->levelMap[target.i][target.j].terrain) +
                      currentChar->getOccupationMod(this->levelMap[target.i][target.j].occupied);

            if((this->levelMap[target.i][target.j].value == -1) ||
               (this->levelMap[target.i][target.j].value > this->levelMap[current.i][current.j].value + tileMod)) {

                this->levelMap[target.i][target.j].value = this->levelMap[current.i][current.j].value + tileMod;
                queue.push_back(target);
            }
        }

        if(current.j > 0) {

            target.i = current.i;
            target.j = current.j - 1;

            tileMod = currentChar->getClass()->getTerrainMod(this->levelMap[target.i][target.j].terrain) +
                      currentChar->getOccupationMod(this->levelMap[target.i][target.j].occupied);

            if((this->levelMap[target.i][target.j].value == -1) ||
               (this->levelMap[target.i][target.j].value > this->levelMap[current.i][current.j].value + tileMod)) {

                this->levelMap[target.i][target.j].value = this->levelMap[current.i][current.j].value + tileMod;
                queue.push_back(target);
            }
        }

        if(current.i < this->levelSize.i - 1) {

            target.i = current.i + 1;
            target.j = current.j;

            tileMod = currentChar->getClass()->getTerrainMod(this->levelMap[target.i][target.j].terrain) +
                      currentChar->getOccupationMod(this->levelMap[target.i][target.j].occupied);

            if((this->levelMap[target.i][target.j].value == -1) ||
               (this->levelMap[target.i][target.j].value > this->levelMap[current.i][current.j].value + tileMod)) {

                this->levelMap[target.i][target.j].value = this->levelMap[current.i][current.j].value + tileMod;
                queue.push_back(target);
            }
        }

        if(current.j < this->levelSize.j - 1) {

            target.i = current.i;
            target.j = current.j + 1;

            tileMod = currentChar->getClass()->getTerrainMod(this->levelMap[target.i][target.j].terrain) +
                      currentChar->getOccupationMod(this->levelMap[target.i][target.j].occupied);

            if((this->levelMap[target.i][target.j].value == -1) ||
               (this->levelMap[target.i][target.j].value > this->levelMap[current.i][current.j].value + tileMod)) {

                this->levelMap[target.i][target.j].value = this->levelMap[current.i][current.j].value + tileMod;
                queue.push_back(target);
            }
        }

        queue.erase(queue.begin());
    }
}

void Level::generateAttackOptions(Character *currentChar) {

    int attackRange = currentChar->getFullBattleStat(Stat::range), movement = currentChar->getFullClassStat(Stat::move);

    Coordinate aux, target;

    int **auxMat = new int*[2*attackRange + 1];

    for(int i = 0 ; i < 1 + 2*attackRange ; i++) {

        auxMat[i] = new int[2*attackRange + 1];

        for(int j = 0 ; j < 1 + 2*attackRange ; j++) {

            auxMat[i][j] = -1;
        }
    }

    std::vector<Coordinate> queue;
    aux.i = attackRange;
    aux.j = attackRange;
    auxMat[aux.i][aux.j] = 0;
    queue.push_back(aux);

    while(!queue.empty()) {

        aux = queue.front();

        if(aux.i > 0) {

            target.i = aux.i - 1;
            target.j = aux.j;

            if(auxMat[target.i][target.j] == -1) {

                auxMat[target.i][target.j] = auxMat[aux.i][aux.j] + 1;
                queue.push_back(target);
            }
        }

        if(aux.j > 0) {

            target.i = aux.i;
            target.j = aux.j - 1;

            if(auxMat[target.i][target.j] == -1) {

                auxMat[target.i][target.j] = auxMat[aux.i][aux.j] + 1;
                queue.push_back(target);
            }
        }

        if(aux.i < 2*attackRange) {

            target.i = aux.i + 1;
            target.j = aux.j;

            if(auxMat[target.i][target.j] == -1) {

                auxMat[target.i][target.j] = auxMat[aux.i][aux.j] + 1;
                queue.push_back(target);
            }
        }

        if(aux.j < 2*attackRange) {

            target.i = aux.i;
            target.j = aux.j + 1;

            if(auxMat[target.i][target.j] == -1) {

                auxMat[target.i][target.j] = auxMat[aux.i][aux.j] + 1;
                queue.push_back(target);
            }
        }

        queue.erase(queue.begin());
    }

    Coordinate auxStart, auxEnd;

    for(int i = 0 ; i < this->levelSize.i ; i++) {

        for(int j = 0 ; j < this->levelSize.j ; j++) {

            if(this->levelMap[i][j].value <= movement && this->levelMap[i][j].value >= 0) {

                auxStart.i = i - attackRange;
                auxStart.j = j - attackRange;
                auxEnd.i = i + attackRange;
                auxEnd.j = j + attackRange;

                for(aux.i = auxStart.i ; aux.i <= auxEnd.i ; aux.i++) {

                    for(aux.j = auxStart.j ; aux.j <= auxEnd.j ; aux.j++) {

                        if((aux.i >= 0 && aux.j >= 0) && (aux.i < this->levelSize.i && aux.j < this->levelSize.j)) {

                            if(auxMat[aux.i - auxStart.i][aux.j - auxStart.j] <= attackRange) {

                                this->levelMap[aux.i][aux.j].attack = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Level::setPath(Coordinate finalPos, Character *currentChar) {

    int movement = currentChar->getFullClassStat(Stat::move), terrainMod;

    Coordinate current = finalPos;

    levelMap[finalPos.i][finalPos.j].path = true;

    bool noPath = (movement < this->levelMap[finalPos.i][finalPos.j].value);

    while(this->levelMap[current.i][current.j].value != 0 && !noPath) {

        terrainMod = currentChar->getClass()->getTerrainMod(this->levelMap[current.i][current.j].terrain);

        if(current.i > 0 && (this->levelMap[current.i - 1][current.j].value == this->levelMap[current.i][current.j].value - terrainMod)) {

            this->levelMap[current.i - 1][current.j].path = true;
            current.i -= 1;

        } else if(current.j > 0 && (this->levelMap[current.i][current.j - 1].value == this->levelMap[current.i][current.j].value - terrainMod)) {

            this->levelMap[current.i][current.j - 1].path = true;
            current.j -= 1;

        } else if((current.i < this->levelSize.i - 1) && (this->levelMap[current.i + 1][current.j].value == this->levelMap[current.i][current.j].value - terrainMod)) {

            this->levelMap[current.i + 1][current.j].path = true;
            current.i += 1;

        } else if((current.j < this->levelSize.j - 1) && (this->levelMap[current.i][current.j + 1].value == this->levelMap[current.i][current.j].value - terrainMod)) {

            this->levelMap[current.i][current.j + 1].path = true;
            current.j += 1;

        } else {

            noPath = true;
        }
    }

    if(noPath) {

        for(int i = 0 ; i < this->levelSize.i ; i++) {

            for(int j = 0 ; j < this->levelSize.j ; j++) {

                this->levelMap[i][j].path = false;
            }
        }
    }
}

void Level::highlightMap(BITMAP *bmp, Character *currentChar) {

    for(int i = 0 ; i < this->levelSize.i ; i++) {

        for(int j = 0 ; j < this->levelSize.j ; j++) {

            if(this->levelMap[i][j].value - currentChar->getOccupationMod(this->levelMap[i][j].occupied) <= currentChar->getFullClassStat(Stat::move)) {

                rectfill(bmp, j*TILE_SIZE + 1, i*TILE_SIZE + 1, (j + 1)*TILE_SIZE - 1, (i + 1)*TILE_SIZE - 1, makeacol(0, 0, 255, 100));

            } else if(this->levelMap[i][j].attack) {

                rectfill(bmp, j*TILE_SIZE + 1, i*TILE_SIZE + 1, (j + 1)*TILE_SIZE - 1, (i + 1)*TILE_SIZE - 1, makeacol(255, 0, 0, 100));
            }
        }
    }
}

void Level::drawPath(BITMAP *bmp, Coordinate startingPos, Coordinate finalPos) {

    //load_bitmap("sprites/path_sprites/...")

    //coordinate current = startingPos, last, next;

    rectfill(bmp, startingPos.j*TILE_SIZE, startingPos.i*TILE_SIZE,
             (startingPos.j + 1)*TILE_SIZE - 1, (startingPos.i + 1)*TILE_SIZE - 1, makeacol(0, 255, 0, 100));

    for(int i = 0 ; i < this->levelSize.i ; i++) {

        for(int j = 0 ; j < this->levelSize.j ; j++) {

            if(this->levelMap[i][j].path) {

                rectfill(bmp, j*TILE_SIZE, i*TILE_SIZE, (j + 1)*TILE_SIZE - 1, (i + 1)*TILE_SIZE - 1, makeacol(0, 255, 0, 100));
            }
        }
    }

}

void Level::setPathValue(bool value, Coordinate pos) {

    this->levelMap[pos.i][pos.j].path = value;
}

bool Level::getPathValue(Coordinate pos) {

    return this->levelMap[pos.i][pos.j].path;
}

void Level::setOccupied(int occupied, Coordinate pos) {

    this->levelMap[pos.i][pos.j].occupied = occupied;
}

int Level::getOccupied(Coordinate pos) {

    return this->levelMap[pos.i][pos.j].occupied;
}

void Level::resetMap() {

    for(int i = 0 ; i < this->levelSize.i ; i++) {

        for(int j = 0 ; j < this->levelSize.j ; j++) {

            this->levelMap[i][j].value = -1;
            this->levelMap[i][j].path = false;
            this->levelMap[i][j].attack = false;
        }
    }
}

void Level::drawGrid(BITMAP *bmp, int intensity) {

    for(int i = 0 ; i < this->levelSize.i ; i++) {

        for(int j = 0 ; j < this->levelSize.j ; j++) {

            rect(bmp, j*TILE_SIZE, i*TILE_SIZE, (j + 1)*TILE_SIZE - 1, (i + 1)*TILE_SIZE - 1, makeacol(0, 0, 0, intensity*15));
        }
    }
}

BITMAP *Level::getBackground() {

    return this->levelBackground;
}

int getItemIndex(std::string itemName) {

    int index = -1;

    for(unsigned i = 0 ; i < itemList.size() && index == -1 ; i++) {

        if(itemList[i]->getItemName() == itemName) {

            index = i;
        }
    }

    return index;
}

