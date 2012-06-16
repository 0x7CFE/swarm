#pragma once

#include "Interface.h"

#include "Singleton.h"
#include "Task.h"

class SupplyManagerClass
{
public:
	SupplyManagerClass(){}

	void update();

private:
	TaskPointer mLastItem;
        std::list<TaskPointer> mSupplyTasks;
};

typedef Singleton<SupplyManagerClass> SupplyManager;