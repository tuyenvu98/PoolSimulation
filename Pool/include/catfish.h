#include "fish.h"
#define CATFISH 500
#define CATFISHRATE 2
#define CATFISHSTEP 3

class __declspec(dllexport) CatFish: public Fish
{
    public :
        CatFish ();
        CatFish (Position );
        std::string name();
};
