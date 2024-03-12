#include "fishfactory.h"

//Get a random start position for fishes
Position FishFactory::startPos()
{
	if (IllegalPos_.size() == 0)
	{
		std::cout << "ERROR: Pool without wall please check again."<<std::endl;
		exit(2);
	}
	Position maxPos = *(IllegalPos_.end() - 1);
	Position minPos = *(IllegalPos_.begin());
	Position s = maxPos;
	while (std::find(IllegalPos_.begin(), IllegalPos_.end(), s) != IllegalPos_.end())
	{
		for (int i = 0; i < maxPos.dimensions.size(); i++)
		{
			int tmp = minPos.dimensions[i] + rand()%1000%(maxPos.dimensions[i]- minPos.dimensions[i] + 1);
			s.dimensions[i] = tmp;
		}
	}
	return s;
};