#include <BattleShipGame/Wrapper/Porting.h>
#include "AI3.h"

// Do not edit this!
void* getai()
{
    AIInterface *ptr = new AI();
    return ptr;
}
