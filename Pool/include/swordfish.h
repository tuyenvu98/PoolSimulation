#include "fish.h"

class __declspec(dllexport) SwordFish: public Fish
{
    public :
        SwordFish ();
        SwordFish (Position );
        std::string name();

};
