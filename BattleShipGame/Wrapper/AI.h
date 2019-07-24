#pragma once

#include <BattleShipGame/Board.h>
#include <BattleShipGame/Ship.h>

#include <chrono>
#include <vector>
#include <utility>

#define AI_ABI_VER 20190528

class AIInterface
{
public:
    AIInterface() = default;
    virtual ~AIInterface() = default;

    int abi(){ return AI_ABI_VER; }

    /*
        pass game information in the beginning
        it should return how to put you ships
    */
    virtual std::vector<TA::Ship> init(int size, std::vector<int> ship_size, bool order, std::chrono::milliseconds runtime) = 0;

    /*
        for each each round, the host will call callbackEnemyReport to pass where the enemy hit
        first player in first round, vector size will be zero
    */
    virtual void callbackReportEnemy(std::vector<std::pair<int,int>>) = 0;

    /*
        for each ship, if the ship state is Ship::State::Available,
        The host will use queryHowToMoveShip(Ship) to query how to move
    */
    virtual std::pair<int,int> queryWhereToHit(TA::Board) = 0;

    /*
        it's be called after queryWhereToHit
        report the state of the enemy
    */
    virtual void callbackReportHit(bool) = 0;

    /*
        for each ship, if the ship state is Ship::State::Available,
        The host will use queryHowToMoveShip(Ship) to query how to move

        this function should return (x, y) using std::pair

        return : i-th element is new position of the i-th ship
    */
    virtual std::vector<std::pair<int,int>> queryHowToMoveShip(std::vector<TA::Ship>) = 0;
};
