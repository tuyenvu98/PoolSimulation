#include "catfishfactory.h"

CatFishFactory::CatFishFactory(std::vector<Position> IllegalPos)
{
    IllegalPos_ = IllegalPos;
}
 
Fish* CatFishFactory::create ()
{
    Position pos = FishFactory::startPos();
    return new CatFish(pos);
}

std::string CatFishFactory::name()
{
    return "CatFish";
}
 
extern "C" __declspec(dllexport) FishFactory * create_factory(std::vector<Position> illegalPos)
{
    return new CatFishFactory(illegalPos);
}
 