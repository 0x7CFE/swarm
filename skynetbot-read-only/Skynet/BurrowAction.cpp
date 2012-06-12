#include "BurrowAction.h"

#include "UnitTracker.h"
#include "UnitInformation.h"
#include "LatencyTracker.h"
#include "Logger.h"
#include "UnitHelper.h"

bool BurrowAction::idleBurrow() {
	if ( (BWAPI::Broodwar->getFrameCount() - mUnit->getLastOrderExecuteTime() > 2 * 24) 
	      && !mUnit->isBurrowed())
	{
		mUnit->burrow();
		return true;
	}
	return false;
}


bool BurrowAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	if (BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Burrowing) && mUnit->getType() != BWAPI::UnitTypes::Zerg_Lurker)
	{
		if( mUnit->isIdle() )
			return idleBurrow();
	}
	if(mUnit->getType() == BWAPI::UnitTypes::Zerg_Lurker)
	{
		if( idleBurrow() )
			return true;

		const UnitGroup& allEnemies = UnitTracker::Instance().selectAllEnemy();
		int numTargetting = UnitInformation::Instance().getUnitsTargetting(mUnit).size();

		if(mUnit->isDetected()) {
			if(!numTargetting) {
				// оценить что важней - закопаться и добить текущую цель или же передислоцироваться так, чтобы не было детектора
			} else {
				// если парочка хилых целей - закопаться и прибить, иначе попытаться свалить
			}
		}

		UnitGroup primaryTargets;
		UnitGroup unitsToStayNearby;
		for each(Unit unit in allEnemies)
		{
			if(!unit->exists())
				continue;
			if(mUnit->getDistance(unit) > BWAPI::WeaponTypes::Subterranean_Spines.maxRange() ) {
				unitsToStayNearby.insert(unit);
				continue;
			}
			primaryTargets.insert(unit);
		}
		Unit target;
		if(!primaryTargets.empty()) {
			target = primaryTargets.getClosestUnit(mUnit);
			if(!mUnit->isBurrowed())
				mUnit->burrow();
			else
				mUnit->attack(target);
			return true;
		}
		if(!unitsToStayNearby.empty()) {
			target = unitsToStayNearby.getClosestUnit(mUnit);
			Position pos = target->getPosition();
			if(mUnit->isBurrowed())
				mUnit->unburrow();
			else
				mUnit->move(pos);
			return true;
		}
	}
	return false;
}