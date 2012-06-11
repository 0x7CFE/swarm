#include "ExpansionManager.h"

#include "BuildOrderManager.h"
#include "BaseTracker.h"
#include "UnitTracker.h"
#include "TaskManager.h"
#include "PlayerTracker.h"
#include "MacroManager.h"
#include "ResourceManager.h"
#include "Logger.h"

ExpansionManagerClass::ExpansionManagerClass()
{
}

void ExpansionManagerClass::update()
{
	if(BuildOrderManager::Instance().getOrder(Order::RefineryManager))
		updateRefineries();

	if(BuildOrderManager::Instance().getOrder(Order::ExpansionManager))
	{
		updateDefense();
		updateExpands();
	}
}

void ExpansionManagerClass::updateRefineries()
{
	int refNeeded = 0;
	for each(Base base in BaseTracker::Instance().getActiveBases(true))
	{
		if(base->getActivateTime() < unsigned int(BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->self()->getRace().getRefinery().buildTime()))
			refNeeded += base->getGeysers().size();
	}

	for(std::list<TaskPointer>::iterator it = mRefineryTasks.begin(); it != mRefineryTasks.end();)
	{
		if((*it)->hasEnded())
			mRefineryTasks.erase(it++);
		else
		{
			if(!(*it)->inProgress())
				--refNeeded;
			++it;
		}
	}

	if(ResourceManager::Instance().requiresRefineries() && refNeeded > 0 && BWAPI::Broodwar->getFrameCount() % 50 == 0)
		mRefineryTasks.push_front(TaskManager::Instance().build(BWAPI::Broodwar->self()->getRace().getRefinery(), TaskType::RefineryManager));
	else if(refNeeded < 0)
	{
		for(int i = 0; i < refNeeded; ++i)
		{
			std::list<TaskPointer>::iterator begin = mRefineryTasks.begin();
			(*begin)->cancel();
			mRefineryTasks.erase(begin);
		}
	}
}


void ExpansionManagerClass::updateDefense(BWAPI::UnitType defenseType, int neededPerBase)
{
        // FIXME What actual condition should be here?
        if(!MacroManager::Instance().hasRequirements(defenseType) && BWAPI::Broodwar->self()->getRace() != BWAPI::Races::Zerg)
                return;

        const BWAPI::Race& myRace = BWAPI::Broodwar->self()->getRace();
        
        // Base support buildings are needed when there are many (distant) bases
        std::set<Base> myBases = BaseTracker::Instance().getActiveBases(true);
        if(myBases.size() >= 2)
        {
                // FIXME In case of Zerg defensesNeeded should be counted
                //       depending on the base chokepoint count
                int defensesNeeded = 0;

                for each(Base base in myBases)
                {
                        if(base->getMinerals().empty())
                                continue; // Nothing to defend?

                        bool hasPylon = defenseType.requiresPsi() ? false : true;
                        int thisCount = 0;
                        int creepCount = 0;
                        
                        // Searching for defenses that were built already
                        for each(Unit building in base->getBuildings())
                        {
                                if(building->getType() == defenseType)
                                        ++thisCount; // Ok

                                // For Protoss only
                                if(building->getType() == BWAPI::UnitTypes::Protoss_Pylon)
                                {
                                        if(building->isCompleted())
                                                hasPylon = true;
                                }
                                else if (building->getType() == BWAPI::UnitTypes::Zerg_Creep_Colony)
                                {
                                        if(building->isCompleted())
                                                creepCount++;
                                }
                        }

                        if (myRace == BWAPI::Races::Zerg && thisCount < neededPerBase)
                        {
                                if (!creepCount) {
                                        // To build defence we need a creep colonies that will be morphed into actual defence type
                                        TCreepMap::iterator iCreep = mCreepTasks.find(base);
                                        // If there are no previous creep tasks or it has ended, then we may schedule another one:
                                        if (iCreep == mCreepTasks.end() || iCreep->second->hasEnded())
                                        {
                                                TaskPointer creepTask = TaskManager::Instance().build(BWAPI::UnitTypes::Zerg_Creep_Colony, TaskType::Defense);
                                                mCreepTasks[base] = creepTask;
                                                mDefenseTasks.push_back(creepTask); // just for cancel operation
                                        }
                                } else {
                                        defensesNeeded += std::min(neededPerBase, creepCount);
                                }
                        } else {
                                if (hasPylon)
                                {
                                        defensesNeeded += neededPerBase;
                                        defensesNeeded -= std::min(thisCount, neededPerBase);
                                }
                                else 
                                {
                                        if( (myRace == BWAPI::Races::Protoss) 
                                                && BWAPI::Broodwar->self()->supplyTotal() >= 380 
                                                && (!mPylon || mPylon->hasEnded()))
                                                        mPylon = TaskManager::Instance().build(BWAPI::UnitTypes::Protoss_Pylon, TaskType::Defense);
                                }
                        }
                }

                // Removing completed tasks that were ordered previously
                for(std::list<TaskPointer>::iterator it = mDefenseTasks.begin(); it != mDefenseTasks.end();)
                {
                        if((*it)->hasEnded())
                                mDefenseTasks.erase(it++);
                        else
                        {
                                if(!(*it)->inProgress())
                                        --defensesNeeded;
                                ++it;
                        }
                }

                // Looking whether we need to build some defense structures
                if(defensesNeeded > 0)
                {
                        for(int i = 0; i < defensesNeeded; ++i)
                        {
                                LOGMESSAGE(String_Builder() << "Built Defense.");
                                
                                if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
                                {
                                        // TODO BuildingLocation::BaseChoke for Zerg
                                        if ((defenseType == BWAPI::UnitTypes::Zerg_Sunken_Colony) || 
                                            (defenseType == BWAPI::UnitTypes::Zerg_Spore_Colony))
                                        {
                                                mDefenseTasks.push_front(TaskManager::Instance().build(defenseType, TaskType::Defense)); 
                                                //mDefenseTasks.push_front(TaskManager::Instance().build(BWAPI::UnitTypes::Zerg_Creep_Colony, TaskType::Defense)); 
                                        }
                                } else
                                        mDefenseTasks.push_front(TaskManager::Instance().build(defenseType, TaskType::Defense)); 
                                
                        }
                }
                else if(defensesNeeded < 0) 
                {
                        // Oops, seems that there are too many defense structures already
                        for(int i = 0; i < defensesNeeded; ++i)
                        {
                                std::list<TaskPointer>::iterator begin = mDefenseTasks.begin();
                                LOGMESSAGE(String_Builder() << "Cancelled Defense.");
                                (*begin)->cancel();
                                mDefenseTasks.erase(begin);
                        }
                }
        }
}

void ExpansionManagerClass::updateDefense()
{
	BWAPI::Race& myRace = BWAPI::Broodwar->self()->getRace();
	int neededPerBase = PlayerTracker::Instance().isEnemyRace(BWAPI::Races::Zerg) ? 4 : 2;
	
	if (myRace == BWAPI::Races::Protoss)
		updateDefense(BWAPI::UnitTypes::Protoss_Photon_Cannon, neededPerBase); 
	else if (myRace == BWAPI::Races::Terran)
		updateDefense(BWAPI::UnitTypes::Terran_Missile_Turret, neededPerBase); 
	else if (myRace == BWAPI::Races::Zerg)
	{
//		updateDefense(BWAPI::UnitTypes::Zerg_Creep_Colony, 4);
		updateDefense(BWAPI::UnitTypes::Zerg_Sunken_Colony, 3);
		//updateDefense(BWAPI::UnitTypes::Zerg_Spore_Colony, 1);
	}
}

void ExpansionManagerClass::updateExpands()
{
	bool unstartedTasks = false;

	for(std::list<TaskPointer>::iterator it = mExpandTasks.begin(); it != mExpandTasks.end();)
	{
		if((*it)->hasEnded())
			mExpandTasks.erase(it++);
		else
		{
			if(!(*it)->inProgress())
				unstartedTasks = true;
			++it;
		}
	}

	if(ResourceManager::Instance().isSaturated() && !unstartedTasks)
	{
		LOGMESSAGE(String_Builder() << "Expanded because im saturated.");
		mExpandTasks.push_front(TaskManager::Instance().build(BWAPI::Broodwar->self()->getRace().getCenter(), TaskType::Expansion, BuildingLocation::Expansion));
	}
}