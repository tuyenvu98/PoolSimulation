#pragma once
#include "pool.h"

using namespace std;
class SquarePool:public Pool
{
public:
    SquarePool(vector<int>&, vector<int>&, vector<vector<int>>&);
    SquarePool() = default;
    vector<int> getBot();
    vector<int> getTop();
    void show(ofstream&);

private:
    vector<int> top_;
    vector<int> bot_;

};