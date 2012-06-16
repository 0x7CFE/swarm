#include "SupplyManager.h"

#include "BuildOrderManager.h"
#include "ResourceTracker.h"
#include "TaskManager.h"
#include "UnitTracker.h"

void SupplyManagerClass::update()
{
	if(!BuildOrderManager::Instance().getOrder(Order::SupplyManager))
		return;

	const int &supplyTime = BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->self()->getRace().getSupplyProvider().buildTime() + 150;
	if(ResourceTracker::Instance().totalSupplyAtTime(supplyTime) < 400)
	{
		const int currentSupply = BWAPI::Broodwar->self()->supplyUsed();
		const int &freeSupply = ResourceTracker::Instance().availableSupplyAtTime(supplyTime);
		if(freeSupply <= ( (currentSupply > 120) ? 16 : (currentSupply > 60 ? 8 : 0) ))
		{
			if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
			{
                                if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
                                {
                                        // Removing finished tasks and counting active ones
                                        for (std::list<TaskPointer>::iterator supplyTask = mSupplyTasks.begin(); supplyTask != mSupplyTasks.end(); )
                                        {
                                                if (supplyTask->hasEnded())
                                                        mSupplyTasks.erase(supplyTask++);
                                                else
                                                        ++supplyTask;
                                        }
                                        
                                        // Checking if we're already creating a lot of overlords
                                        if (mSupplyTasks.size() < 3)
                                        {
                                                mLastItem = TaskManager::Instance().build(BWAPI::UnitTypes::Zerg_Overlord, TaskType::Supply);
                                                mSupplyTasks.push_back(mLastItem);
                                        }
                                }
			} 
			else 
                        {
                                if(!mLastItem || mLastItem->hasDispatched() || mLastItem->hasEnded())
                                        mLastItem = TaskManager::Instance().build(BWAPI::Broodwar->self()->getRace().getSupplyProvider(), TaskType::Supply);
                        }
		}
	}
}
