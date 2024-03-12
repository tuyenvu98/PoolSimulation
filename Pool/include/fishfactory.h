#pragma once
#include <vector>
#include "fish.h"
#include "position.h"

//Base factory to create fish 

class __declspec(dllexport) FishFactory
{

protected:
  std::vector<Position> IllegalPos_;
public:
  virtual Fish* create()=0;
  virtual std::string name()=0;
  Position startPos();

};
 
extern "C" __declspec(dllexport) FishFactory * create_factory(std::vector<Position> illegalPos);