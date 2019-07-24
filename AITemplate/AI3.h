#pragma once

#include <BattleShipGame/Wrapper/AI.h>
#include <BattleShipGame/Ship.h>
#include <algorithm>
#include <random>
#include <ctime>
#include <stdlib.h>
#include <cmath>

class AI : public AIInterface
{
    
    std::vector<std::pair<int,int>> hit;
public:
    virtual std::vector<TA::Ship> init(int size ,std::vector<int> ship_size, bool order, std::chrono::milliseconds runtime) override
    {
        (void)size;
        (void)ship_size;
        (void)order;
        (void)runtime;

        std::vector<TA::Ship> tmp;

        int i, j, k;
        
        // set ship size
        int sz[4] = {3, 3, 5, 7};
        // map bool for checking if there has been positioned a ship
        int map[20][20] = {0};

        srand((unsigned)time(nullptr));

        for (i=0; i<4; i++){
            int check = 0;
            // set random position of a ship
            int place_i = rand()%(20-sz[i]);
            int place_j = rand()%(20-sz[i]);

            // check if we can place the ship
            for (j = place_i; j < place_i+sz[i]; j++){
                for (k = place_j; k < place_j+sz[i]; k++){
                    if (map[j][k] == 1) check = 1;
                }
            }

            // if there is already a ship, put the ship again
            if (check) i--;
            else{
                //record states and put a ship on map
                tmp.push_back({sz[i], place_i, place_j,TA::Ship::State::Available});

                for (j = place_i; j < place_i+sz[i]; j++)
                    for (k = place_j; k < place_j+sz[i]; k++)
                        map[j][k] = 1;
            }
        }
        
        return tmp;
    }

    virtual void callbackReportEnemy(std::vector<std::pair<int,int>>) override
    {

    }

    virtual std::pair<int,int> queryWhereToHit(TA::Board b) override
    {
        // a map for checking hit density in region
        int check_hit_den[5][5]={{0},{0}};
        std::vector<std::pair<int,int>> same_rec;
        
        for(int i=0;i<5;++i)
            for(int j=0;j<5;++j)
                for(int k=0;k<4;++k)
                    for(int l=0;l<4;++l){
                        // if the board state is unknown, the hit density region plus 1
                        if(b[4*i+k][4*j+l]==TA::Board::State::Unknown) check_hit_den[i][j]++;
                        // if the board state is hit, the hit density region plus 3
                        else if(b[4*i+k][4*j+l]==TA::Board::State::Hit)
                            check_hit_den[i][j]=check_hit_den[i][j]+3;
                        // if the board state is empty, the hit density region minus 1
                        else if(b[4*i+k][4*j+l]==TA::Board::State::Empty)
                            check_hit_den[i][j]--;
                    }
        
        
        int hit_x, hit_y, ind_x, ind_y, ind_sx, ind_sy;
        int ran_pos, hit_rand_sl;
        int count,times;
        int pre_num = 1000;
        int num;
        bool find=false;
        
        // while not finding a place to attack, then continue searching
        while(!find){
            
            num=-20;
            for (int i=0; i<5; i++)
                for (int j=0; j<5; j++){
                    // find the unhit region with highest hit density
                    if(check_hit_den[i][j]>num && check_hit_den[i][j]<pre_num){
                        same_rec.clear();
                        same_rec.shrink_to_fit();
                        same_rec.emplace_back(std::pair<int, int>(i,j));
                        num=check_hit_den[i][j];
                    }
                    // store the regions with same hit density
                    else if(check_hit_den[i][j]==num) same_rec.emplace_back(std::pair<int, int>(i,j));
                    
                }
            
            pre_num=num;
            times=0;
            while(!find && times<10){
                // random choose of the regions from same_rec
                ran_pos = rand()%same_rec.size();
                ind_x=same_rec[ran_pos].first;
                ind_y=same_rec[ran_pos].second;
                // random position in that region
                hit_rand_sl=rand()%16;
                ind_sx=hit_rand_sl/4;
                ind_sy=hit_rand_sl%4;
                
                count=0;
                // ensure always find an unknown place to hit
                while(b[4*ind_x+ind_sx][4*ind_y+ind_sy]!=TA::Board::State::Unknown && count<40){
                    hit_rand_sl=rand()%16;
                    ind_sx=hit_rand_sl/4;
                    ind_sy=hit_rand_sl%4;
                    count++;
                }
                
                // if find, then break the loop
                if(b[4*ind_x+ind_sx][4*ind_y+ind_sy]==TA::Board::State::Unknown) find=true;
                times++;
            }
        }
        
        // the true position
        hit_x=4*ind_x+ind_sx;
        hit_y=4*ind_y+ind_sy;
        
        
        return std::pair<int, int>(hit_x, hit_y);
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
