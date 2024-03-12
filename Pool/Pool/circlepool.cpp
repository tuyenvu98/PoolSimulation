#include "circlepool.h"
#include <algorithm>
#include <set>

//Recursively get the positions of pool wall
void recur( int index,int r,int d,vector<vector<int>>& cur, set<vector<int>>& set)
{
    if (index == d-1)
        return;
    
    for (int m = 0; m <= r; m++)
    {
        if (cur.size() == 0)
        {
            cur.push_back({ r - m });
            cur.push_back({ r + m });
        }
        else
        {
            vector<vector<int>> add =cur;
            int size = cur.size();
            cur.insert(cur.end(), add.begin(), add.end());
            for (int i=0;i<size;i++)
                cur[i].push_back(r + m);
            for (int i = size; i < size*2; i++)
                cur[i].push_back(r - m);
        }
        recur(index + 1, r,d, cur, set);
        for (int a=0;a<cur.size();a++)
        {
            int sum = 0;
            for (int i = 0; i < cur[a].size(); i++)
            {
                sum += (r - cur[a][i]) * (r - cur[a][i]);
            }
            int tmp = r * r - sum;
            int last = (tmp > 0) ? sqrt(r * r - sum) : 0;

            for (int j = last; j <= r; j++)
            {
                cur[a].push_back(r - j);
                set.insert(cur[a]);
                cur[a].pop_back();

                cur[a].push_back(r + j);
                set.insert(cur[a]);
                cur[a].pop_back();
            }
            cur[a].pop_back();
        }
        cur.erase(cur.begin()+cur.size()/2,cur.end());
    }

}

CirclePool::CirclePool(int r,int d)
{
    r_ = r;
    vector<vector<int>> cur;
    set<vector<int>> set;
    recur(0, r,d, cur, set);
    vector<Position> tmp(set.begin(), set.end());
    auto f = [&](Position p) {return p.dimensions.size() != d; };
    auto invalid = std::remove_if(tmp.begin(), tmp.end(), f);
    tmp.erase(invalid, tmp.end());
    IllegalPos = tmp;
}

int CirclePool::getR()
{
    return r_;
}

void CirclePool::show(ofstream& poolLog)
{
    int size = IllegalPos[0].dimensions.size();
    std::cout << "---------------------------------------------------------------" << endl;

    std::cout << "Fishes : " << fishes.size() << endl;
    vector<Position> Shark, CatFish, SwordFish;
    for (auto i : fishes)
    {
        string str = i->name() + " with Pos :";
        for (int m = 0; m < size; m++)
            str += to_string(i->getPos().dimensions[m])+",";
        str += (" and  strength: " + to_string(i->getStrength()));
        std::cout << str << endl;
        if (i->name() == "Shark")
            Shark.push_back(i->getPos());
        else if (i->name() == "SwordFish")
            SwordFish.push_back(i->getPos());
        else
            CatFish.push_back(i->getPos());
    }

    for (int i = 2 * r_; i >= 0; i--)
    {
        std::cout << i << "\t|";
        for (int j = 0; j <= 2 * r_; j++)
        {
            
            vector<int>  v{ j,i };
            if (size > 2)
                v.push_back(r_);

            Position tmp(v);
            if (std::find(Shark.begin(), Shark.end(), tmp) != Shark.end())
                std::cout << "\033[1;34mS\033[0m" << "|";
            else if (std::find(SwordFish.begin(), SwordFish.end(), tmp) != SwordFish.end())
                std::cout << "\033[1;34mW\033[0m" << "|";
            else if (std::find(CatFish.begin(), CatFish.end(), tmp) != CatFish.end())
                std::cout << "\033[1;34mC\033[0m" << "|";
            else if (std::find(blindPos.begin(), blindPos.end(), tmp) != blindPos.end())
                std::cout << "\033[1;32mB\033[0m" << "|";
            else if (std::find(IllegalPos.begin(), IllegalPos.end(), tmp) != IllegalPos.end())
                std::cout << "\033[1;31mX\033[0m" << "|";
            else
                std::cout << "_|";
        }
        std::cout << endl;
    }
    std::cout << " \t|";
    for (int i = 0; i <= 2*r_; i++)
        std::cout << i << "|";
    std::cout << endl;
    std::cout << "---------------------------------------------------------------" << endl;

}