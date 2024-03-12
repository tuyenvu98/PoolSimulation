#include "swordfishfactory.h"

SwordFishFactory::SwordFishFactory(std::vector<Position> IllegalPos)
{
    IllegalPos_ = IllegalPos;
}

Fish* SwordFishFactory ::create ()
{
    Position pos = startPos();
    return new SwordFish(pos);
}

std::string SwordFishFactory::name()
{
    return "SwordFish";
}

extern "C" __declspec(dllexport) FishFactory * create_factory(std::vector<Position> illegalPos)
{
    return new SwordFishFactory(illegalPos);
}