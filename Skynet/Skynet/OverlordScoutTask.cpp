#include "OverlordScoutTask.h"
#include "ScoutManager.h"

OverlordScoutTask::OverlordScoutTask(TaskType priority, ScoutData data)
	: Task(priority)
	, mData(data)
{
}

int OverlordScoutTask::getEndTime() const
{
	return Requirement::maxTime;
}

int OverlordScoutTask::getEndTime(Unit unit) const
{
	return getEndTime();
}

int OverlordScoutTask::getPriority(Unit unit) const
{
	return 10;
}

Position OverlordScoutTask::getStartLocation(Unit unit) const
{
	if(mData)
		return mData->getNextPosition(unit->getPosition());

	return unit->getPosition();
}

Position OverlordScoutTask::getEndLocation(Unit unit) const
{
	if(mData)
		return mData->getLastPosition(unit->getPosition());

	return unit->getPosition();
}

bool OverlordScoutTask::preUpdate()
{
	if(mData && mData->isAchieved())
		mData.reset();

	if(mUnit && !mData)
		mData = ScoutManager::Instance().getScoutData(ScoutUnitType::Overlord);

	return false;
}

bool OverlordScoutTask::update()
{
	if(mUnit)
	{
		Goal overlordGoal;

		if(mData)
			overlordGoal = Goal(ActionType::FallBack, mData->getNextPosition(mUnit->getPosition()));

		if(overlordGoal.getGoalType() == GoalType::None)
		{
			;//TODO: go to closest cloaked enemy unit
			//TODO: go to closest enemy scv that is in the fog
		}

		mUnitBehaviour.update(overlordGoal, UnitGroup());
	}

	return hasEnded() && !mUnit;
}

void OverlordScoutTask::updateRequirements()
{
	clearRequirements();

	if(!mUnit && !shouldReturnUnit())
	{
		RequirementGroup requirementOne;
		if(mData)
			requirementOne.addUnitFilterRequirement(10, Requirement::maxTime, UnitFilter(BWAPI::UnitTypes::Zerg_Overlord) && UnitFilter(UnitFilterFlags::IsComplete), mData->getNextPosition());
		else
			requirementOne.addUnitFilterRequirement(10, Requirement::maxTime, UnitFilter(BWAPI::UnitTypes::Zerg_Overlord) && UnitFilter(UnitFilterFlags::IsComplete));

		addRequirement(requirementOne);
	}
}

bool OverlordScoutTask::waitingForUnit(Unit unit) const
{
	return false;
}

void OverlordScoutTask::giveUnit(Unit unit)
{
	mUnit = unit;
	mUnitBehaviour.set(unit);
}

void OverlordScoutTask::returnUnit(Unit unit)
{
	if(!mUnit->exists() && mData)
		ScoutManager::Instance().updateLastScoutType(mData, ScoutType::FailedWithAir);

	complete();

	mUnit = StaticUnits::nullunit;
	mUnitBehaviour.onDeleted();
}

bool OverlordScoutTask::morph(Unit unit, BWAPI::UnitType previousType)
{
	return true;
}

UnitGroup OverlordScoutTask::getFinishedUnits()
{
	UnitGroup returnUnits;

	returnUnits.insert(mUnit);

	return returnUnits;
}