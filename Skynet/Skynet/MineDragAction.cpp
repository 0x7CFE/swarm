#include "MineDragAction.h"

#include <limits>

#include "UnitTracker.h"
#include "UnitInformation.h"

bool MineDragAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	// If we detected that we are a target for spider mine (oops!) do something useful as a last resort
	// Let's run to the nearest enemy and hopefully hit him with the mine's splash damage along with our weapon
  
	for (Unit spiderMine : UnitInformation::Instance().getUnitsTargetting(mUnit))
	{
		// Is it really a spider mine?
		if(spiderMine->getType() == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine)
		{
			int distance = std::numeric_limits<int>::max();
			Unit closestUnit;

			// Selecting the nearest enemy who will receive the damage
			for (Unit enemyUnit : UnitTracker::Instance().selectAllEnemy())
			{
				// This unit is not interesting
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
				// If we have an enemy to harm, fight him!
				mUnit->attack(closestUnit);
				return true;
			}
			else
			{
				// Seems that no enemies are in range except the damn mine.
				// Well… nothing could be done for now. 
				// Just rushing to the mine in hope that we kill it before it explodes
				mUnit->move(spiderMine->getPosition());
				return true;
			}
		}
	}

	return false;
}