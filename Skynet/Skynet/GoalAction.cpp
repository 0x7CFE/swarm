#include "GoalAction.h"

#include "Vector.h"

Position getWeightedCenter(const UnitGroup& unitGroup, Position goal) const
{
        if(unitGroup.empty())
                return BWAPI::Positions::None;

        if(unitGroup.size() == 1)
                return (*unitGroup.begin())->getPosition();

        Vector position;

        Position groupCenter = unitGroup.getCenter();
        UnitGroup mainGroup();
        
        for (Unit unit : unitGroup.mUnits)
        {
                if (unit->getDistance(goal) < unit->getDistance(groupCenter))
                        mainGroup.insert(unit);
                
        }

        return mainGroup.getCenter();
}

void GoalAction::squadMove(const UnitGroup &squadUnitGroup, Position position, int accuracy)
{
        UnitGroup mass = squadUnitGroup.getBestFittingToCircle(200);
        Position massCenter = mass.getCenter();
        //getWeightedCenter(squadUnitGroup, position);
        if (mUnit->getDistance(massCenter) > 600)
                mUnit->move(massCenter, 256);
        else
                mUnit->move(position, accuracy);
}

bool GoalAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	switch(squadGoal.getGoalType().underlying())
	{
	case GoalType::Base:
		{
			Unit enemy = squadGoal.getBase()->getClosestEnemyBuilding(mUnit->getPosition());
			if(enemy)
			{
				// mUnit->move(enemy->getPosition(), 128);
				squadMove(squadUnitGroup, enemy->getPosition(), 128);
				return true;
			}
			else
			{
				//mUnit->move(squadGoal.getBase()->getCenterLocation(), 128);
                                squadMove(squadUnitGroup, squadGoal.getBase()->getCenterLocation(), 128);
				return true;
			}
		}
	case GoalType::Position:
		// mUnit->move(squadGoal.getPosition(), 128);
                squadMove(squadUnitGroup, squadGoal.getPosition(), 128);
		return true;
	case GoalType::Unit:
		// mUnit->move(squadGoal.getUnit()->getPosition(), 128);
                squadMove(squadUnitGroup, squadGoal.getUnit()->getPosition(), 128);
		return true;
	case GoalType::UnitGroup:
		// mUnit->move(squadGoal.getEngageUnits().getClosestUnit(mUnit)->getPosition(), 128);
                squadMove(squadUnitGroup, squadGoal.getEngageUnits().getClosestUnit(mUnit)->getPosition(), 128);
		return true;
	}
	return false;
}