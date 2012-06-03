#pragma once

#include "Interface.h"

#include "Singleton.h"
#include "Base.h"
#include "TaskPump.h"

class BaseDefenceManagerClass
{
public:
	BaseDefenceManagerClass();

	void update();
	void onDestroy(Unit unit);

private:
	std::map<Unit, Base> mUnitToBase;
	//std::map<Base, std::map<Unit, GatherTaskPointer>> mResourceTasks;
	std::set<Base> mMyActiveBases;
	std::set<Unit> mActiveDefenceUnits;
	std::set<Unit> mUnitsInProgress;
	
	std::set<Chokepoint> mDefendedBases;
	std::set<Chokepoint> mDefendedChokes;

	TaskPump mTaskPump;
	bool mSaturated;
};

typedef Singleton<BaseDefenceManagerClass> DefenceManager;