#pragma once

#include "Interface.h"
#include "MicroAction.h"

/* Micro management for the high templar's psi storm.
   Could be used as a base for Zerg's defiler's plague or Queen's ensnare */
class PsiStormAction : public SingleMicroActionBaseClass
{
public:
	PsiStormAction(Unit unit) : SingleMicroActionBaseClass(unit) {}

	bool update(const Goal &squadGoal, const UnitGroup &squadUnitGroup);
};