#pragma once
#include <vector>
#include "position.h"
#include <string>
using namespace std;
class Obstacle
{
    std::vector<Position> insidePos;
    std::string name_;
    public :
        Obstacle ()= default;
        Obstacle(vector<vector<int>> ,std::string );

        std::vector<Position> getInsidePos();
        std::string name();
};
