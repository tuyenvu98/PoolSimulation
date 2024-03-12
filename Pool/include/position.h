#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class __declspec(dllexport) Position
{

    public :
        static int d;
        std::vector<int> dimensions;
        Position()=default;
        bool operator==(Position rhs)
        {
            if (this->dimensions.size() != rhs.dimensions.size())
            {
                std::cout << "Warning : Comparing 2 position with different number of dimensions." << std::endl;
                return false;
            }
            for (int i = 0; i < rhs.dimensions.size(); i++)
                if (this->dimensions[i] != rhs.dimensions[i])
                    return false;
            return true;
        }

        Position (std::vector<int> dims)
        {
            this->dimensions = dims;
        }
};

