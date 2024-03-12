#include "shark.h"
#include <stdlib.h> 
#include <vector>
#include <algorithm>


Shark::Shark ()
{
    init("shark");
}
Shark::Shark (Position p )
{
    pos = p;
    target = pos;
    init("shark");
}

std::string Shark::name()
{
    return "Shark";
}



