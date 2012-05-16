#include "MineDragAction.h"

#include <limits>

#include "UnitTracker.h"
#include "UnitInformation.h"

bool MineDragAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	for (Unit unit : UnitInformation::Instance().getUnitsTargetting(mUnit))
	{
		if(unit->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
		{
			int distance = std::numeric_limits<int>::max();
			Unit closestUnit;

			for (Unit enemyUnit : UnitTracker::Instance().selectAllEnemy())
			{
				if(enemyUnit->getType().isFlyer() || enemyUnit->isLifted() || enemyUnit->getType().isBuilding() || enemyUnit->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
					continue;

				int thisDistance = mUnit->getDistance(enemyUnit);
				if(thisDistance < distance)
				{
					distance = distance;
					closestUnit = enemyUnit;
				}
			}
			
			if(closestUnit && distance < 32*3)
			{
				mUnit->attack(closestUnit);
				return true;
			}
			else
			{
				mUnit->move(unit->getPosition());
				return true;
			}
		}
	}

	return false;
}