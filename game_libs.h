#ifndef GAME_LIBS_H_INCLUDED
#define GAME_LIBS_H_INCLUDED

#include <allegro.h>
#include <vector>
#include <cstdio>
#include <string>
#include <ctime>

const int TILE_SIZE = 16;
const int SPRITE_SIZE = 32;
const int CAMERA_W = 20;
const int CAMERA_H = 15;

struct Coordinate {

    int i, j;
};

struct Stat {

    enum classStats {
        health, strength, defense, magic, resistance, dexterity, luck, speed, move, CLASS_STAT_QNT
    };
    enum battleStats {
        range, attack, hit, avoid, critical, BATTLE_STAT_QNT
    };
    const static unsigned STAT_MAX = 80;
    int baseStat;
};

struct Tile {

    enum tileOccupied {
        freeTile, playerTile, enemyTile, TILE_OCCUPIED_QNT
    };
    enum terrainTypes {
        plain, forest, water, mountain, wall, TERRAIN_QNT
    };
    const static unsigned TERRAIN_INFINITY = 500;
    int occupied, terrain, value;
    bool path, attack;
};

struct ItemHandler {

    std::string itemName;
    int uses;
};

class Controller {

    public:
        enum controllerKey {
            keyUp, keyDown, keyLeft, keyRight, keyAction, keyCancel, keyESC, keyL, keyR, CONTROLLER_KEY_QNT
        };
        Controller();
        ~Controller();
        void readInput();
        int checkKeyState(int controllerKey);
        void setKeyState(int controllerKey, int state);

    private:
        int keyArray[Controller::CONTROLLER_KEY_QNT];
};

class InteractableObject {

    public:
        enum directions {
            up, down, left, right
        };
        InteractableObject();
        ~InteractableObject();
        Coordinate getPosition();
        void setPosition(Coordinate newPos);
        BITMAP *getSprites();
        void setSprites(BITMAP *sprite);
        void move(int direction);

    private:
        BITMAP* sprites;
        Coordinate position;
};

class Cursor : public InteractableObject {

    public:
        Cursor();
        ~Cursor();
        bool getPressed();
        void setPressed(bool value);
        bool getEnabled();
        void setEnabled(bool value);
        bool getVisible();
        void setVisible(bool value);

    private:
        bool pressed, enabled, visible;
};

class Hub : public InteractableObject {

    public:
        Hub();
        ~Hub();
        bool getEnabled();
        void setEnabled(bool status);
        Coordinate getSize();

    private:
        bool hubEnabled;
};

class Class {

    public:
        Class();
        Class(std::string classPath);
        ~Class();
        void setClassName(std::string className);
        std::string getClassName();
        void setClassSprites(BITMAP *classSprites);
        BITMAP *getClassSprites(int hairColor, int gender);
        void setStatCap(int stat, int cap);
        int getStatCap(int stat);
        void setTerrainMod(int terrain, int mod);
        int getTerrainMod(int terrain);
        void setOccupationMod(int tileOccupied, int mod);
        int getOccupationMod(int tileOccupied);
        void setMaxLevel(int level);
        int getMaxLevel();
        void setElement(int element);
        int getElement();

    private:
        std::string className;
        int baseStats[Stat::CLASS_STAT_QNT], statCaps[Stat::CLASS_STAT_QNT], terrainMod[Tile::TERRAIN_QNT], occupationMod[Tile::TILE_OCCUPIED_QNT];
        int maxLevel, classElement;
        BITMAP *classSprites;
};

class Character : public InteractableObject {

    public:
        enum gender {
            male, female
        };
        enum hairColor {
            red, blue, white, green
        };
        enum charAnimation {
            cursorOff, MovingRight, MovingDown, MovingUp, MovingLeft, cursorOn, CHAR_ANIMATION_QNT
        };
        const static unsigned ANIMATIONS_STEPS = 4;
        Character();
        ~Character();
        virtual int getOccupationMod(int tileOccupied);
        void setCharName(std::string charName);
        std::string getCharName();
        Coordinate getSpriteIndex();
        void setLastPos(Coordinate pos);
        Coordinate getLastPos();
        void changeAnimation(int newAnimation);
        int getAnimation();
        void setSelected(bool status);
        bool isSelected();
        void setPlayable(bool status);
        bool isPlayable();
        void changeMoving(bool status);
        bool isMoving();
        void addItem(ItemHandler newItem);
        ItemHandler getItem(int index);
        std::vector<ItemHandler> getInventory();
        void setClass(Class *characterClass);
        Class *getClass();
        int getGender();
        void setGender(int gender);
        int getHairColor();
        void setHairColor(int hairColor);
        void setClassStat(Stat newStat, int stat);
        Stat getClassStat(int stat);
        int getFullClassStat(int stat);
        void setBattleStat(Stat newStat, int stat);
        Stat getBattleStat(int stat);
        int getFullBattleStat(int stat);
        void setEquippedWeapon(int index);
        int getEquippedWeapon();
        void addTarget(Character *target);
        void removeTarget(int index);
        Character *getTarget(int index);
        unsigned getTargetListSize();
        void setCurrentHealth(int health);
        int getCurrentHealth();

    private:
        int animation, animationStep, gender, hairColor, equippedWeapon, range, currentHealth;
        Stat stats[Stat::CLASS_STAT_QNT];
        Stat battleStats[Stat::BATTLE_STAT_QNT];
        bool selected, playable, moving;
        std::vector<ItemHandler> inventory;
        std::string charName;
        Class *characterClass;
        Coordinate lastPos;
        std::vector<Character*> targets;
};

class Enemy : public Character {

    public:
        Enemy();
        ~Enemy();
        int getOccupationMod(int tileOccupied);

    private:
};

class Item {

    public:
        Item();
        Item(std::string itemName, int uses);
        ~Item();
        void setItemName(std::string itemName);
        std::string getItemName();
        void setUses(int uses);
        int getUses();
        virtual void useItem(Character *user, int index) = 0;

    private:
        std::string itemName;
        int uses;
};

class Weapon : public Item {

    public:
        enum weaponType {
            magical, physical
        };
        Weapon();
        ~Weapon();
        void setWeaponType(int weaponType);
        int getWeaponType();
        void setRange(int range);
        int getRange();
        void setPower(int power);
        int getPower();
        virtual void useItem(Character *user, int index);

    private:
        int power, range, weaponType;
};

class Level {

    public:
        Level();
        Level(std::string levelIndex);
        ~Level();
        Coordinate getSize();
        unsigned getPlayableCharQnt();
        unsigned getPlayableCharMax();
        Coordinate getStartingPos(int index);
        Character *getCharacter(int index);
        void addPlayableChar(Character *newChar);
        void removePlayableChar(int index);
        unsigned getEnemyQnt();
        Enemy *getEnemy(int index);
        void addEnemy(Enemy *newEnemy);
        void removeEnemy(int index);
        int getTileValue(Coordinate pos);
        bool getTileAttack(Coordinate pos);
        void setTerrain(int terrain, Coordinate tile);
        void generatePathOptions(Character *currentChar);
        void generateAttackOptions(Character *currentChar);
        void setPath(Coordinate finalPos, Character *currentChar);
        void highlightMap(BITMAP *bmp, Character *currentChar);
        void drawPath(BITMAP *bmp, Coordinate startingPos, Coordinate finalPos);
        void setPathValue(bool value, Coordinate pos);
        bool getPathValue(Coordinate pos);
        void setOccupied(int occupied, Coordinate pos);
        int getOccupied(Coordinate pos);
        void resetMap();
        void drawGrid(BITMAP *bmp, int intensity);
        BITMAP *getBackground();

    private:
        Tile **levelMap;
        Coordinate levelSize;
        BITMAP *levelBackground;
        unsigned playableCharMax;
        std::vector<Character*> playableCharacters;
        std::vector<Coordinate> charStartingPos;
        std::vector<Enemy*> enemies;
};

extern std::vector<Item*> itemList;

int getItemIndex(std::string itemName);
float applyElementTriangleBonus(Character *attacker, Character *defender);

#endif // GAME_LIBS_H_INCLUDED
