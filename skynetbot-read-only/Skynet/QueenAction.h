#pragma once

#include "Interface.h"
#include "MicroAction.h"

/* Micro management for the Zerg Queen */
class QueenAction : public SingleMicroActionBaseClass
{
public:
	QueenAction(Unit unit) : SingleMicroActionBaseClass(unit) {}

	bool update(const Goal &squadGoal, const UnitGroup &squadUnitGroup);
	bool castBroodlings(const UnitGroup& allEnemies);
	bool castParasite(const UnitGroup& allEnemies, ActionType currentAction);
	bool castEnsnare(const UnitGroup& allEnemies);
};
