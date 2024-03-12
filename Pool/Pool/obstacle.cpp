#include "obstacle.h"
#include <cmath>
#include <iostream>


Obstacle::Obstacle(vector<vector<int>> p, string name)
{
    for (auto pos : p)
    {
        insidePos.push_back(pos);
    }
    name_ = name;
}

std::vector<Position> Obstacle::getInsidePos()
{
    return insidePos;
}

std::string Obstacle::name()
{
    return name_;
}

