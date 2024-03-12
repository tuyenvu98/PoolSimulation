#include "sharkfactory.h"

SharkFactory::SharkFactory(std::vector<Position> IllegalPos)
{
   IllegalPos_ = IllegalPos;
}

Fish* SharkFactory::create ()
{
    Position pos = startPos();
    return new Shark(pos);
}
std::string SharkFactory::name()
{
    return "Shark";
}

extern "C" __declspec(dllexport) FishFactory * create_factory(std::vector<Position> illegalPos)
{
    return new SharkFactory(illegalPos);
}
