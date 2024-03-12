#pragma once
#include <vector>
#include "fish.h"
#include "obstacle.h"

using namespace std;
class Pool
{
public:
    Pool(vector<Position>, vector<Position>);
    Pool() = default;
    vector<Fish*> getFishes();
    void setObstacle(Obstacle*);
    void addFish(Fish*);
    void sendFish(int);
    virtual void show(ofstream&);
    void fight(ofstream&);
    void makeMove();
    vector<Position> getIllegalPos();
    vector<Position> getBlindPos();
    vector<Position> getSendPos();
    virtual vector<int> getBot() = 0;
    virtual vector<int> getTop() = 0;
protected:
    vector<Obstacle*> obs;
    vector<Fish*> fishes;
    vector<Position> IllegalPos;
    vector<Position> blindPos;
    vector<Position> sendPos;

};
