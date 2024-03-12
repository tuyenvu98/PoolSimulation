
#include "pool.h"

using namespace std;
class CirclePool :public Pool
{
public:
    CirclePool(int,int );
    CirclePool() = default;
    int getR();
    void show(ofstream&);
    vector<int> getBot() { return {}; };

    vector<int> getTop() { return {}; };

private:
    int r_;


};