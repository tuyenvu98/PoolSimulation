#include "pool.h"
#include <iostream>
#include <algorithm>
#include <set>
//A pool object will include Obstacle and a vector of fishes


Pool::Pool(vector<Position> illegal, vector<Position> send)
{
    if (illegal.size() == 0)
    {
        std::cout << "ERROR: Can not create pool without wall." << endl;;
        exit(2);
    }
    IllegalPos.insert(IllegalPos.end(), illegal.begin(), illegal.end());
    sendPos = send;
}

vector<Fish*> Pool::getFishes()
{
    return fishes;
}
//IllegalPos is vector of posible position for fishes to move to
vector<Position> Pool::getIllegalPos()
{
    return IllegalPos;
}

//blindPos is vector of position that cover by sea weed, fishes will not fight there
vector<Position> Pool::getBlindPos()
{
    return blindPos;
}

vector<Position> Pool::getSendPos()
{
    return sendPos;
}

// Add Obstacle to the pool if it is not bigger than the pool
// IllegalPos is a list of possible positions for fishes to move, it will be in side the pool but outside the Obstacle 
void Pool::setObstacle(Obstacle* p)
{
    bool check = true;
    vector<Position> tmp = p->getInsidePos();
    /*for (auto pole : tmp)
        if (pole.dimensions[0]>w_ ||pole.dimensions[1] >h_ || pole.dimensions[2] > d_)
            {
                std::cout <<"Can not put this " <<p.name()<< " to the pool as it is too big" << endl;
                check=false;
            }
    if (check)*/

    if (p->name() == "Penjing")
        IllegalPos.insert(IllegalPos.begin() + 1, tmp.begin(), tmp.end());
    else if (p->name() == "SeaWeed")
        blindPos.insert(blindPos.begin(), tmp.begin(), tmp.end());
    else
    {
        std::cout << "Obstacle with unknown type, ignored." << std::endl;
        return;
    }

    obs.push_back(p);

}



void Pool::show(ofstream& poolLog)
{

    int size = IllegalPos[0].dimensions.size();
    cout << "---------------------------------------------------------------" << endl;

    cout << "Fishes : " << fishes.size() << endl;
    vector<Position> Shark, CatFish, SwordFish;
    for (auto i : fishes)
    {
        string str = i->name() + " with Pos :";
        for (int m = 0; m < size; m++)
            str += to_string(i->getPos().dimensions[m]) + ",";
        str += (" and  strength: " + to_string(i->getStrength()));
        cout << str << endl;
        if (i->name() == "Shark")
            Shark.push_back(i->getPos());
        else if (i->name() == "Sword Fish")
            SwordFish.push_back(i->getPos());
        else
            CatFish.push_back(i->getPos());
    }
    cout << "---------------------------------------------------------------" << endl;

}

//Add fish to the list of fishes
void Pool::addFish(Fish* f)
{
    if (f)
        fishes.push_back(f);
}

/*Check if fishes meet each other,
if they meet in the sea weed, nothing happens
else the stronger one eat weaker one*/
void Pool::fight(ofstream& poolLog)
{
    if (fishes.size() <= 1)
        return;

    auto f = [&](Fish* a, Fish* b) {return a->getStrength() > b->getStrength(); };

    std::sort(fishes.begin(), fishes.end(), f);
    auto it = fishes.begin();

    while (it != fishes.end())
    {
        if (std::find(blindPos.begin(), blindPos.end(), (*it)->getPos()) == blindPos.end())
        {
            auto comparePosition = [&](Fish* a) {return (*it)->getPos() == a->getPos(); };
            for (auto it2 = it + 1; it2 != fishes.end(); it2++)
                if (comparePosition(*it2))
                {
                    (*it)->eat(*it2);
                    cout << (*it)->name() << " ate " << (*it2)->name() << endl;
                    poolLog << (*it)->name() << " ate " << (*it2)->name() << endl;
                }
            //Remove killed fishes eaten by *it
            auto killed = std::remove_if(it + 1, fishes.end(), comparePosition);
            fishes.erase(killed, fishes.end());
        }
        it++;
    }
}

//Every fish in pool makes move, check if is there any died fish (strength <0) then remove it
void Pool::makeMove()
{
    //auto sent = std::remove_if(fishes.begin(), fishes.end(), [&](Fish* f) {return std::find(sendPos.begin(), sendPos.end(), f->getTarget()) != sendPos.end(); });
    //fishes.erase(sent, fishes.end());
    for (auto it = fishes.begin(); it != fishes.end(); it++)
    {
        (*it)->move(IllegalPos, sendPos);
    }
    auto dead = std::remove_if(fishes.begin(), fishes.end(), [](Fish* f) {return f->getStrength() <= 0; });
    fishes.erase(dead, fishes.end());

}


void Pool::sendFish(int index)
{
    fishes[index] = fishes.back();
    fishes.pop_back();
}