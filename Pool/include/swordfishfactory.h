#include "swordfish.h"
#include "fishfactory.h"
#include<string>

//Concrete factory to create sword fish 

class __declspec(dllexport) SwordFishFactory : public FishFactory
{

public:
  SwordFishFactory(std::vector<Position> );
  Fish* create ();
  std::string name();
 
};

 