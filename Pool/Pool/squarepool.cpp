#include "squarepool.h"
#include <algorithm>
#include <set>

//Recursively get the positions of pool wall
void recur(int i, int index, const vector<int> bot, const vector<int> top, vector<int>& cur, set<vector<int>>& set)
{
    if (index == i)
        index++;

    if (index == bot.size())
        return;

    for (int m = bot[index]; m <= top[index]; m++)
    {
        cur.push_back(m);
        recur(i, index + 1, bot,top, cur, set);
        if (cur.size() == bot.size() - 1)
        {
            cur.insert(cur.begin() + i, bot[i]);
            set.insert(cur);
            cur[i] = top[i];
            set.insert(cur);
            cur.erase(cur.begin() + i);
        }
        cur.pop_back();
    }

}

SquarePool::SquarePool(vector<int>& bot, vector<int>& top, vector<vector<int>>& send)
{
    vector<int> cur;
    set<vector<int>> set;
    top_ = top;
    bot_ = bot;
    for (int i = 0; i < bot_.size(); i++)
    {
        recur(i, 0, bot,top, cur, set);
    }
    for (auto pos : set)
        IllegalPos.push_back(pos);
    for (auto s : send)
        sendPos.push_back(s);
}

vector<int> SquarePool::getBot()
{
    return bot_;
}

vector<int> SquarePool::getTop()
{
    return top_;
}

void SquarePool::show(ofstream& poolLog)
{
    int size = IllegalPos[0].dimensions.size();
    cout << "---------------------------------------------------------------" << endl;
    poolLog << "---------------------------------------------------------------" << endl;
    cout << "Fishes : " << fishes.size() << endl;
    poolLog << "Fishes : " << fishes.size() << endl;
    vector<Position> Shark, CatFish, SwordFish;
    for (auto i : fishes)
    {
        string str = i->name() + " with Pos :";
        for (int m = 0; m < size; m++)
            str += to_string(i->getPos().dimensions[m]) + ",";
        str += (" and  strength: " + to_string(i->getStrength()));
        cout << str << endl;
        poolLog << str << endl;
        if (i->name() == "Shark")
            Shark.push_back(i->getPos());
        else if (i->name() == "SwordFish")
            SwordFish.push_back(i->getPos());
        else
            CatFish.push_back(i->getPos());
    }

    for (int i = top_[1]; i >= bot_[1]; i--)
    {
        cout << i << "\t|";
        for (int j = bot_[0]; j <= top_[0]; j++)
        {
            vector<int>  v{ j,i };
            if (size > 2)
                v.push_back(top_[2] / 2);

            Position tmp(v);
            if (std::find(Shark.begin(), Shark.end(), tmp) != Shark.end())
                cout << "\033[1;34mS\033[0m" << "|";
            else if (std::find(SwordFish.begin(), SwordFish.end(), tmp) != SwordFish.end())
                cout << "\033[1;34mW\033[0m" << "|";
            else if (std::find(CatFish.begin(), CatFish.end(), tmp) != CatFish.end())
                cout << "\033[1;34mC\033[0m" << "|";
            else if (std::find(blindPos.begin(), blindPos.end(), tmp) != blindPos.end())
                cout << "\033[1;32mB\033[0m" << "|";
            else if (std::find(sendPos.begin(), sendPos.end(), tmp) != sendPos.end())
                cout << "\033[1;33mJ\033[0m" << "|";
            else if (std::find(IllegalPos.begin(), IllegalPos.end(), tmp) != IllegalPos.end())
                cout << "\033[1;31mX\033[0m" << "|";
            else
                cout << "_|";
        }
        cout << endl;
    }
    cout << " \t|";
    for (int i = bot_[0]; i <= top_[0]; i++)
        cout << i << "|";
    cout << endl;
    cout << "---------------------------------------------------------------" << endl;
    poolLog << "---------------------------------------------------------------" << endl;

}