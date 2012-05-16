#include "UnitInformation.h"
#include "UnitTracker.h"

UnitInformationClass::UnitInformationClass()
{
}

void UnitInformationClass::update()
{
	mUnitTargets.clear();
	for (Unit unit : UnitTracker::Instance().getAllUnits())
	{
		if(unit->getTarget())
			mUnitTargets[unit->getTarget()].insert(unit);
	}
}