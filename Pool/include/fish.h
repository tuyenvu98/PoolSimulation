#pragma once
#include <cstdlib> 
#include <vector>
#include <map>
#include "position.h"
#define suffix ".txt"
#define configFolder "config\\"
//Define strength, decreasing rate and move step for each type of fish
class __declspec(dllexport) Fish
{
    protected :
	    int maxStrength=0;
        int rate=1;
        int step=1;
        int strength=0;
        int count=0;
        Position pos;
        Position target;
        void init(const std::string&);
    public :
        Fish() {};
        Fish(Position p);
        virtual ~Fish()=default;
        Position getPos();
        Position getTarget();
        int getStrength();
        int getMaxStrength();
        virtual std::string name()=0;
        void eat(Fish*);
        void move(std::vector<Position>, std::vector<Position>);
        void serialize(std::string&);
        void deserialize(int, Position);
};

