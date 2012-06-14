#pragma once

#include "Interface.h"
#include "MicroAction.h"

class BurrowAction : public SingleMicroActionBaseClass
{
public:
	BurrowAction(Unit unit) : SingleMicroActionBaseClass(unit) {}

	bool idleBurrow();
	bool update(const Goal &squadGoal, const UnitGroup &squadUnitGroup);
};