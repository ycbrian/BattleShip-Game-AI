#pragma once

#include <vector>

namespace TA
{
    struct Ship
    {
        /* 
            it mean Ship hold those position b[i][j]
            for i = x, x+1, ..., x+size-1
            for j = y, y+1, ..., y+size-1
        */

        enum class State
        {
            Available,
            Hit,
            Sink
        };

        int size;
        int x;
        int y;

        Ship::State state;
    };
}

