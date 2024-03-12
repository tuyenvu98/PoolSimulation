#include "catfish.h"
//#include "position.h"
#include "fishfactory.h"
#include <string>

//Concrete factory to create cat fish 

class __declspec(dllexport) CatFishFactory : public FishFactory
{

public:
  CatFishFactory(std::vector<Position> );
  Fish* create ();
  std::string name();
 
};
