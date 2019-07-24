#pragma once

#include <BattleShipGame/Wrapper/AI.h>
#include <BattleShipGame/Ship.h>
#include <algorithm>
#include <random>
#include <ctime>

class AI : public AIInterface
{
    std::vector<std::pair<int,int>> way;
public:
    virtual std::vector<TA::Ship> init(int size ,std::vector<int> ship_size, bool order, std::chrono::milliseconds runtime) override
    {
        (void)ship_size;
        (void)runtime;

        std::vector<TA::Ship> tmp;
        tmp.push_back({3, 0,  0,TA::Ship::State::Available});
        tmp.push_back({3, 5,  0,TA::Ship::State::Available});
        tmp.push_back({5, 0,  5,TA::Ship::State::Available});
        tmp.push_back({7, 10, 10,TA::Ship::State::Available});
        
        for(int i=0;i<size;++i)
            for(int j=0;j<size;++j)
                way.emplace_back(i,j);

        std::mt19937 mt;
        mt.seed( std::time(nullptr) + 7122 + (order?1:0) );
        std::shuffle(way.begin(), way.end(), mt);
        return tmp;
    }

    virtual void callbackReportEnemy(std::vector<std::pair<int,int>>) override
    {

    }

    virtual std::pair<int,int> queryWhereToHit(TA::Board) override
    {
        auto res = way.back();
        way.pop_back();
        return res;
    }

    virtual void callbackReportHit(bool)  override
    {

    }

    virtual std::vector<std::pair<int,int>> queryHowToMoveShip(std::vector<TA::Ship>ships) override
    {
        std::vector<std::pair<int,int>>res;
        for(auto ship:ships)
            res.emplace_back(ship.x,ship.y);
            
        return res;
    }
};
