#include "GoalAction.h"

#include "Vector.h"

bool GoalAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	switch(squadGoal.getGoalType().underlying())
	{
	case GoalType::Base:
		{
			Unit enemy = squadGoal.getBase()->getClosestEnemyBuilding(mUnit->getPosition());
			if(enemy)
			{
				if (mUnit->isBurrowed())
				      mUnit->unburrow();
				mUnit->move(enemy->getPosition(), 128);
				return true;
			}
			else
			{
				if (mUnit->isBurrowed())
				      mUnit->unburrow();
				mUnit->move(squadGoal.getBase()->getCenterLocation(), 128);
				return true;
			}
		}
	case GoalType::Position:
		if (mUnit->isBurrowed())
		      mUnit->unburrow();
		mUnit->move(squadGoal.getPosition(), 128);
		return true;
	case GoalType::Unit:
		if (mUnit->isBurrowed())
		      mUnit->unburrow();
		mUnit->move(squadGoal.getUnit()->getPosition(), 128);
		return true;
	case GoalType::UnitGroup:
		if (mUnit->isBurrowed())
		      mUnit->unburrow();
		mUnit->move(squadGoal.getEngageUnits().getClosestUnit(mUnit)->getPosition(), 128);
		return true;
	}
	return false;
}