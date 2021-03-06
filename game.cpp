//
// Created by Kyle Brown on 11/1/2020.
//

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "game.h"
#include "./ant_types/harvester.h"
#include "./ant_types/thrower.h"
#include "./ant_types/fire.h"
#include "./ant_types/long_thrower.h"
#include "./ant_types/short_thrower.h"
#include "./ant_types/wall.h"
#include "./ant_types/ninja.h"
#include "./ant_types/bodyguard.h"

using namespace std;

game::game() {

    cout << "********** BEES VS ANTS TOWER DEFENSE **********" << endl;
    cout << "  ____MADE BY KYLE BROWN & LUIS RODRIGUEZ____" << "\n" << endl;

    this->gameBoard = vector<tile> (10);
    this->foodBank = 50;

    // Placing Queen ant in first tile space
    gameBoard[0].firstAnt = ants(true);
    gameLoop();
}

/**
 * There is nothing to delete as vectors are stored in the runtime stack not on the heap.
 * Additionally like I mention in the tile.cpp file, a vector of object implicitly
 * calls the destructor of each object. This means when the tile destructor gets called, it
 * then calls the destructors for the objects that comprise it. However boiled down everything
 * used in this program does not get allocated onto the heap or is a primitive type.
 *
 * Hence why there is nothing needed to destruct
 */
game::~game() {

}

/**
 * @description the main game loop which first generates a bee, allows the player to buy
 * and place an ant, then have the ants attack, then the bees attack and advance forward.
 * At the end of every turn the loop checks if the queen has died or if the bees have all been
 * defeated, which will then end the game.
 */
void game::gameLoop() {
    do {

        cout << "[********************* BEGIN TURN *********************]" << "\n" << endl;
        cout << "Food bank: " << this->foodBank << endl;

        /**
         * 1) A bee is generated on the right side of the board
         */
        generateBee();
        printGameBoard();

        /**
         * 2) The player can generate an ant and place it anywhere on the board
         * (if they have enough food).
         */
        cout << ">------------------- BUYING AN ANT --------------------<" << "\n" << endl;
        if (this->foodBank < 2) {
            cout << "Insufficient food, can't buy an ant this round" << endl;
        }
        else {
            buyAnt();
        }
        cout << "------------------------------------------------------" << "\n" << endl;

        /**
         * 3) The ants attack the bees. (Order of ant attacks occur left to right)
         */
        cout << ">-------------------- ANTS TURN -----------------------<" << "\n" << endl;
        antsTurn();
        cout << "------------------------------------------------------" << "\n" << endl;

        /**
         * 4) The bees either attack an ant (order of attack is left to right) which is blocking
         * them or pass through to the next square on the board if they are not blocked by an ant
         */
        cout << ">-------------------- BATTLE TURN -----------------------<" << "\n" << endl;
        moveBee();
        cout << "------------------------------------------------------" << "\n" << endl;

        cout << "[********************** END TURN **********************]" << "\n" << endl;

    } while (!queenDead() && checkBeeCount() > 0);

    /**
     * Terminating conditions if bees or ants win
     */
    if(queenDead()) {
        cout << "\n" << "The queen is dead! The bees win!" << endl;
    }

    else {
        cout << "\n" << "All bees are dead! The ants win!" << endl;
    }
}

/**
 * @description Prints the game board in a 1D representation of 10 squares
 * this loop goes through and totals up adjacent ants & bees sharing the same
 * square as well and displays as a comma separated list.
 */
void game::printGameBoard() {

    for(int i = 0; i < 10; i++) {
        cout << "[" << i+1 << ": " << gameBoard[i].firstAnt.symbol << ", " <<
        gameBoard[i].secondAnt.symbol << " | ";

        // prints list of bees at each tile
        for(int j = 0; j < gameBoard[i].beesList.size(); j++) {

            if(j < gameBoard[i].beesList.size() -1) {
                cout << gameBoard[i].beesList[j].symbol << ", ";
            }
            else {
                cout << gameBoard[i].beesList[j].symbol;
            }
        }

        cout << " ] ";
    }
    cout << "\n";
}

/**
 * @description checks every turn if the queen has been killed.
 *
 * @return true if the queen has died (if bees reach tile 1).
 */
bool game::queenDead() {

    if(gameBoard[0].beesList.size() > 0) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @description counts every turn the number of bees as if 0 are
 * present the game ends.
 *
 * @return the number of bees alive on the board.
 */
int game::checkBeeCount() {
    beeCount = 0;

    for(int i = 0; i < gameBoard.size(); i++) {

        for(int j = 0; j < gameBoard[i].beesList.size(); j++) {
            if(gameBoard[i].beesList[j].symbol == "Bee") {
                beeCount++;
            }
        }
    }

    return beeCount;
}

/**
 * @description returns the colony food count.
 *
 * @return -- the current colony food supply.
 */
int game::getFood() {
    return this->foodBank;
}

/**
 * @description reduces the colony food amount by the amount given.
 *
 * @param cost -- the amount of food buying a new ant will cost, whatever cost given
 * is subtracted from the colony total.
 */
void game::setFood(int cost) {
    foodBank -= cost;
}

/**
 * @description Prompt with options to buy an ant, any ant that is placed over
 * a space with another ant will simply replace it.
 */
void game::buyAnt() {
    cout << "You have " << this->getFood() << " points" << endl;
    cout << "--Choose an ant--" << endl;
    cout << "1) Harvester:" << " 2pts" << endl;
    cout << "2) Thrower:" << " 4pts" << endl;
    cout << "3) Fire:" << " 4pts" << endl;
    cout << "4) Long Thrower:" << " 3pts" << endl;
    cout << "5) Short Thrower:" << " 3pts" << endl;
    cout << "6) Wall:" << " 4pts" << endl;
    cout << "7) Ninja:" << " 6pts" << endl;
    cout << "8) Bodyguard:" << " 4pts" << endl;

    int type = -1;
    do {
        cout << "Choose an ant option between 1 and 8" << endl;
        string input = "";
        getline(cin, input);
        type = parseInt(input);

    } while(type < 1 || type > 8);

    switch (type) {
        case 1:
            placeAnt(1); // Harvester
            setFood(2);
            break;
        case 2:
            placeAnt(2); // Thrower
            setFood(4);
            break;
        case 3:
            placeAnt(3); // Fire
            setFood(4);
            break;
        case 4:
            placeAnt(4); // Long Thrower
            setFood(3);
            break;
        case 5:
            placeAnt(5); // Short Thrower
            setFood(3);
            break;
        case 6:
            placeAnt(6); // Wall
            setFood(4);
            break;
        case 7:
            placeAnt(7); // Ninja
            setFood(6);
            break;
        case 8:
            placeAnt(8); // Bodyguard
            setFood(4);
            break;
    }
}

/**
 * @description Creates a new ant type and returns a pointer.
 *
 * @param antId -- the id for which ant type to be created.
 * @return ants * -- a pointer to the newly created ant type.
 */
ants game::createAntType(int antId) {
    ants a;

    switch (antId) {
        case 1:
            a = harvester();
            break;
        case 2:
            a = thrower();
            break;
        case 3:
            a = fire();
            break;
        case 4:
            a = long_thrower();
            break;
        case 5:
            a = short_thrower();
            break;
        case 6:
            a = wall();
            break;
        case 7:
            a = ninja();
            break;
        case 8:
            a = bodyguard();
            break;
    }

    return a;
}

/**
 * @description handles placing an ant at a given tile on the board.
 *
 * @param antId -- the id of the ant type to be created.
 */
void game::placeAnt(int antId) {
    ants a;
    a = createAntType(antId);
    cout << "--Select a location for the ant--" << endl;
    int location;

    do {
        cout << "Choose an ant option between 2 and 10" << endl;
        string input = "";
        getline(cin, input);
        location = parseInt(input);

    } while(location < 2 || location > 10);

    switch (location) {
        case 2:
            checkAntPos(1, a);
            break;
        case 3:
            checkAntPos(2, a);
            break;
        case 4:
            checkAntPos(3, a);
            break;
        case 5:
            checkAntPos(4, a);
            break;
        case 6:
            checkAntPos(5, a);
            break;
        case 7:
            checkAntPos(6, a);
            break;
        case 8:
            checkAntPos(7, a);
            break;
        case 9:
            checkAntPos(8, a);
            break;
        case 10:
            checkAntPos(9, a);
            break;
    }
}

/**
 * @description adds an ant to tile location and checks whether or not
 * the given ant is a body guard, if so places in a space in a tile.
 *
 * @param loc -- the position to add the ant on the board.
 * @param a -- the ant object to be added to a tile.
 */
void game::checkAntPos(int loc, ants a) {
    if(a.symbol == "BG") {
        gameBoard[loc].secondAnt = a;
    }
    else {
        gameBoard[loc].firstAnt = a;
    }
}

/**
 * @description This starts the ants actions for any ants that are not
 * special such as the bodyguard, ninja, fire, and wall ants.
 */
void game::antsTurn() {

    for(int i = 0; i < gameBoard.size(); i++) {

        if(gameBoard[i].firstAnt.symbol == "Harv") {
            foodBank++;
            cout << "[Harvester] Collected 1 food for the colony!" << "\n" << endl;
        }
        else if(gameBoard[i].firstAnt.symbol == "Throw") {
            if(gameBoard[i].beesList.size() > 0) {
                gameBoard[i].beesList[0].armor -= 1;
                if(gameBoard[i].beesList[0].armor <= 0) {
                    gameBoard[i].beesList.erase(gameBoard[i].beesList.begin() + 0);
                }
            }
            cout << "[Thrower] Did 1 damage to a bee in the same tile!" << "\n" << endl;
        }
        else if(gameBoard[i].firstAnt.symbol == "S-Throw") {
            shortThrower(i);
        }
        else if(gameBoard[i].firstAnt.symbol == "L-Throw") {
            longThrower(i);
        }
        else {
            continue;
        }
    }
}

/**
 * @description a helper function that calculates damage the short thrower
 * does to everything in range of 2 spaces.
 *
 * @param loc -- the index the short thrower resides at.
 */
void game::shortThrower(int loc) {

    for(int i = loc; (i < loc+3) && (i < gameBoard.size()); i++) {

        for(int j = 0; j < gameBoard[i].beesList.size(); j++) {

            gameBoard[i].beesList[j].armor -= 1;

            // if the bee's armor is reduced to zero it is deleted
            if(gameBoard[i].beesList[j].armor <= 0 && j == 0) {
                gameBoard[i].beesList.erase(gameBoard[i].beesList.begin() + (j));
                foodBank++; // prevents infinite looping if there are no harvesters
            }
            else if(gameBoard[i].beesList[j].armor <= 0 && j > 0) {
                gameBoard[i].beesList.erase(gameBoard[i].beesList.begin() + (j-1));
                foodBank++; // prevents infinite looping if there are no harvesters
            }
        }
    }
    cout << "[Thrower] Did 1 damage to all bees within tiles " << loc << " through " << loc + 3 << "!" << "\n" << endl;
}

/**
 * @description a helper function that calculates damage the long thrower
 * does at 4 spaces away.
 *
 * @param loc -- the index the long thrower resides at.
 */
void game::longThrower(int loc) {

    if(loc + 4 < gameBoard.size()) {
        int i = loc + 4;

        for (int j = 0; j < gameBoard[i].beesList.size(); j++) {

            gameBoard[i].beesList[j].armor -= 1;

            // if the bee's armor is reduced to zero it is deleted
            if (gameBoard[i].beesList[j].armor <= 0 && j == 0) {
                gameBoard[i].beesList.erase(gameBoard[i].beesList.begin() + (j));
                foodBank++; // prevents infinite looping if there are no harvesters
            } else if (gameBoard[i].beesList[j].armor <= 0 && j > 0) {
                gameBoard[i].beesList.erase(gameBoard[i].beesList.begin() + (j - 1));
                foodBank++; // prevents infinite looping if there are no harvesters
            }
        }
        cout << "[Long Thrower] Did 1 damage to all bees in tile " << i+5 << "!" << "\n" << endl;
    }
}

/**
 * @description Loops through bees on all tiles at every position and gives them a
 * turn to first attack a body guard ant if present, otherwise the ant in the tile.
 * If there are no bodyguards or ants present (exception being the ninja) the bees will
 * continue.
 */
void game::beesTurn() {

    int beeWhoWon = -1;
    int tileLoc;
    bees copyBee;
    bool termEarly = false;

    for(int i = 1; i < gameBoard.size() && !termEarly; i++) {

        if(gameBoard[i].beesList.size() == 0) {
            continue;
        }

        for(int j = 0; j < gameBoard[i].beesList.size() && !termEarly; j++) {

            // case for body guard blocking
            if(gameBoard[i].secondAnt.symbol == "BG") {

                gameBoard[i].secondAnt.armor -= 1;

                // ant has died
                if(gameBoard[i].secondAnt.armor <= 0) {
                    cout << "[Bee] has killed the Bodyguard in tile " << i+1 << "!" << endl;
                    gameBoard[i].secondAnt = ants();
                }

            }
            // otherwise attack the ant in the tile
            else {
                gameBoard[i].firstAnt.armor -= 1;

                // ant has died
                if(gameBoard[i].firstAnt.armor <= 0) {

                    // Fire ant case
                    if(gameBoard[i].firstAnt.symbol == "Fire") {
                        gameBoard[i].firstAnt = ants();
                        gameBoard[i].beesList.resize(0);
                        termEarly = true;
                        cout << "[Fire Ant] BOOM! the fire ant in tile " << i + 1 << " has detonated!" << "\n" << endl;
                    }
                    else {
                        cout << "[Bee] has killed the " << "[" << gameBoard[i].firstAnt.symbol << "]" << " in tile " << i+1 << "!" << endl;
                        gameBoard[i].firstAnt = ants();
                        beeWhoWon = j;
                        tileLoc = i;
                    }
                }
            }

            // this makes a copy of the bee which one the fight
            if(beeWhoWon != -1) {
                copyBee = (gameBoard[i].beesList[j]);
                termEarly = true;
            }
        }
    }

    // Now if the ants are all dead all but the bee who killed them may move on
    if(beeWhoWon != -1) {
        cout << "[Bees] The bees defeated all ants in tile " << tileLoc + 1 << endl;
        postVictoryMove(copyBee, tileLoc);
    }
}

/**
 * @description upon a bee tile victory, the winning be will stay at the tile
 * while the others progress forward.
 *
 * @param copiedBee -- The bee who won the battle to stay at the tile.
 * @param tileLoc -- The tile index where the battle took place.
 */
void game::postVictoryMove(bees &copiedBee, int tileLoc) {

    // Moves all bees from battle to next tile
    for(int j = 0; j < gameBoard[tileLoc].beesList.size(); j++) {
        gameBoard[tileLoc-1].beesList.push_back(gameBoard[tileLoc].beesList[j]);
    }

    // clears current tile of bees then inserts the specific bee that won
    gameBoard[tileLoc].beesList.resize(0);
    gameBoard[tileLoc].beesList.push_back(copiedBee);
    cout << "[Bees] All bees except the bee who won the last fight advance forward!" << "\n" << endl;
}

/**
 * @description Moves list of bees over to left tile and starts a battle if there
 * are occupying ants.
 */
void game::moveBee() {

    for(int i = 1; i < gameBoard.size(); i++) {

        // No ants occupying tiles space
        if((gameBoard[i].firstAnt.symbol == "" && gameBoard[i].secondAnt.symbol == "") ||
            (gameBoard[i].firstAnt.symbol == "Ninja" && gameBoard[i].secondAnt.symbol == "")) {

            // Stacks bees into tile if blocked by ants
            for(int j = 0; j < gameBoard[i].beesList.size(); j++) {
                gameBoard[i-1].beesList.push_back(gameBoard[i].beesList[j]);
            }

            if(gameBoard[i].firstAnt.symbol == "Ninja") {
                for(int j = 0; j < gameBoard[i].beesList.size(); j++) {

                    gameBoard[i-1].beesList[j].armor -= 1;

                    // if the bee's armor is reduced to zero it is deleted
                    if(gameBoard[i-1].beesList[j].armor <= 0 && j == 0) {
                        gameBoard[i-1].beesList.erase(gameBoard[i-1].beesList.begin() + (j));
                        foodBank++; // prevents infinite looping if there are no harvesters
                    }
                    else if(gameBoard[i-1].beesList[j].armor <= 0 && j > 0) {
                        gameBoard[i-1].beesList.erase(gameBoard[i-1].beesList.begin() + (j-1));
                        foodBank++; // prevents infinite looping if there are no harvesters
                    }
                }

                cout << "[Ninja] Sneak Attack!!! The ninja in tile " << i+1 << " has back-stabbed all passing bees!" << "\n" << endl;
            }
            gameBoard[i].beesList.resize(0);
        }
        else {

            // skips ants with no bees in tiles
            if(gameBoard[i].beesList.size() == 0) {
                continue;
            }

            cout << "[~~~Battle~~~] at tile "<< i+1 << "\n" << endl;
            beesTurn();
        }
    }
}

/**
 * @description Dynamically allocates a bee in the last column.
 */
void game::generateBee() {
    gameBoard[9].beesList.push_back(bees());
}

/**
 * @description Attempts to successfully parse and integer from a given
 * string, if successful the int is returned otherwise -1 is returned.
 *
 * @param input -- the string input to be parsed.
 * @return int valid -- the successfully parsed integer or a -1.
 */
int game::parseInt(std::string &input) {

    int number;
    istringstream iss(input);

    // Valid parsed int
    if (iss >> number) {
        return number;
    }

    cout << "invalid input, not an integer" << endl;
    return -1;
}