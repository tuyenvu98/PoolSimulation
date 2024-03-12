#include "fish.h"
#include <sstream>
#include <fstream>
#include <iostream>
Fish::Fish (Position p)
{
    pos = p;
}

//Return current possition of the fish
Position Fish::getPos()
{
    return pos;
}

Position Fish::getTarget()
{
    return target;
}

//Return strength of the fish
int Fish::getStrength()
{
    return strength;
}
//Return max strength of the fish
int Fish::getMaxStrength()
{
    return maxStrength;
}

//Eat the weaker fish
void Fish::eat(Fish *f)
{
    strength += f->strength;
}

void Fish::move(std::vector<Position> illegal, std::vector<Position> send)
{
    int d = pos.dimensions.size();
    if (count < step - 1)
        count++;
    else
    {
        pos = target;
        count = 0;
        Position r = illegal[0];
        while (std::find(illegal.begin(), illegal.end(), r) != illegal.end()&& std::find(send.begin(), send.end(), r) == send.end())
        {
            bool validateCanMove = false;
            for (int i = 0; i < d; i++)
            {
                Position tmp = pos;
                int randomD = rand()%3000;
                int d;
                if (randomD > 2000)
                    d = 1;
                else if (randomD > 1000)
                    d = -1;
                else d = 0;
                r.dimensions[i] = pos.dimensions[i] + d;
                tmp.dimensions[i] = pos.dimensions[i] + d;
                if (std::find(illegal.begin(), illegal.end(), tmp) == illegal.end())
                    validateCanMove = true;
            }
            if(!validateCanMove)
                Position r = illegal[0];
        }
        target = r;
    }
    strength -= maxStrength * rate * 0.01;

}

void Fish::init(const std::string& type)
{

    std::string fileName = std::string(configFolder) + type + std::string(suffix);
    std::ifstream configFile(fileName);
    if (!configFile.good())
    {
        std::cout << "File " << fileName <<" is not valid." << std::endl;
        exit(1);
    }
    std::string tmp;

    while (getline(configFile, tmp))
    {
        std::stringstream ss(tmp);
        std::vector<std::string> v;
        while (ss.good()) {
            std::string substr;
            getline(ss, substr, ' ');
            substr.erase(remove_if(substr.begin(), substr.end(), [](char s) {return s == ' '; }));
            if (!substr.empty())
                v.push_back(substr);
        }
        if (v.size() < 2)
            continue;
        std::stringstream str(v[1]);
        int value;
        str >>value;
        if (value <= 0)
            continue;
        if (v[0] == "STRENGTH")
            maxStrength = value;
        else if (v[0] == "RATE")
            rate = value;
        else if (v[0] == "STEP")
            step = value;
    }
    strength = maxStrength;
}

void Fish::serialize(std::string& str) 
{
    str = name() + ' ';
    str += std::to_string(strength) + ' ';
    for (int i = 0; i < pos.dimensions.size(); i++)
        str += std::to_string(pos.dimensions[i]) + ",";
    str.erase(str.end() - 1);
    str += ' ';
    for (int i = 0; i < target.dimensions.size(); i++)
        str += std::to_string(target.dimensions[i]) + ",";
    str.erase(str.end() - 1);
}

void Fish::deserialize(int s, Position p)
{
    strength = s;
    pos = p;
    target = p;
}