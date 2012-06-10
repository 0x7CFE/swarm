#pragma once

#include "Interface.h"

#include "Singleton.h"
#include "Task.h"
#include "Base.h"

class ExpansionManagerClass
{
public:
	ExpansionManagerClass();

	void update();

private:
	std::list<TaskPointer> mRefineryTasks;
	void updateRefineries();

        typedef std::map<Base, TaskPointer> TCreepMap;
        TCreepMap mCreepTasks;
        
	TaskPointer mPylon;
	std::list<TaskPointer> mDefenseTasks;
	void updateDefense();
	void updateDefense(BWAPI::UnitType defenseType, int neededPerBase);

	std::list<TaskPointer> mExpandTasks;
	void updateExpands();
};

typedef Singleton<ExpansionManagerClass> ExpansionManager;