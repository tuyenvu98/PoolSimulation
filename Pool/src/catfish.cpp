#include "catfish.h"
#include <stdlib.h> 
#include <vector>
#include <algorithm>


CatFish::CatFish() 
{
    init("catfish");
}


CatFish::CatFish (Position p)
{
    pos= p;
    target = pos;
    init("catfish");
}

std::string CatFish::name()
{
    return "CatFish";
}

