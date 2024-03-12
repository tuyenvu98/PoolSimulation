#include "fish.h"


class __declspec(dllexport) Shark: public Fish
{
    
    public :
        Shark ();
        Shark (Position );
        std::string name();

};
