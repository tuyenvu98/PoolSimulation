#include "swordfish.h"
#include <vector>
#include <algorithm>


SwordFish::SwordFish ()
{
    init("swordfish");
}

SwordFish::SwordFish (Position p )
{
    pos = p;
    target = pos;
    init("swordfish");
}

std::string SwordFish::name()
{
    return "SwordFish";
}

