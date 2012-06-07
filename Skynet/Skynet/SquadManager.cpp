#include "SquadManager.h"
#include "TaskManager.h"
#include "BuildOrderManager.h"
#include "BaseTracker.h"

#include "DefaultSquad.h"

SquadManagerClass::SquadManagerClass()
{
	mDefaultSquad = createSquad(SquadType::DefaultSquad);
}

void SquadManagerClass::update()
{
	for(std::map<Base, DefenseSquadPointer>::iterator it = mDefenseSquads.begin(); it != mDefenseSquads.end();)
	{
		// If there are no enemy threats and no valuable buildings to protect, we do not need a defence squad here
		if(it->first->getEnemyThreats().empty() || (it->first->isMinedOut() && it->first->getNumberOfTechBuildings() == 0))
			it->second->cancel();

		if(it->second->hasEnded())
			mDefenseSquads.erase(it++);
		else
			++it;
	}

	for (Base base : BaseTracker::Instance().getPlayerBases())
	{
		// If we have a base with tech and resources but without a defence squad, we should create one
		if(!base->getEnemyThreats().empty() && (base->getNumberOfTechBuildings() > 0 || (!base->isMinedOut() && base->getResourceDepot())) && mDefenseSquads.count(base) == 0)
		{
			DefenseSquadPointer squad = std::static_pointer_cast<DefenseSquadTask>(createSquad(SquadType::DefenseSquad));
			squad->setGoal(Goal(ActionType::Defend, base));
			mDefenseSquads[base] = squad;
		}
	}
}

void SquadManagerClass::onChangeBuild()
{
	const std::map<SquadType, int> &squads = BuildOrderManager::Instance().getCurrentBuild().getSquads();
	for(std::map<SquadType, int>::const_iterator it = squads.begin(); it != squads.end(); ++it)
	{
		if(it->first == SquadType::DefaultSquad || it->first == SquadType::DefenseSquad)
			continue;

		int numNeeded = it->second - mSquads[it->first].size();
		while(numNeeded != 0)
		{
			if(numNeeded < 0)
			{
				++numNeeded;

				// Finding the smallest squad
				int smallest = std::numeric_limits<int>::max();
				BaseSquadTaskPointer smallestSquad;
				for (BaseSquadTaskPointer squad : mSquads[it->first])
				{
					int size = squad->controlSize();
					if(size < smallest)
					{
						smallest = size;
						smallestSquad = squad;
					}
				}

				// And erasing it (why?)
				if(smallestSquad)
				{
					smallestSquad->cancel();
					mSquads[it->first].erase(smallestSquad);
				}
			}
			else
			{
				--numNeeded;
				createSquad(it->first);
			}
		}
	}

	for(std::map<SquadType, std::set<BaseSquadTaskPointer>>::iterator o = mSquads.begin(); o != mSquads.end();)
	{
		if(o->first != SquadType::DefaultSquad && o->first != SquadType::DefenseSquad && squads.find(o->first) == squads.end())
		{
			for (BaseSquadTaskPointer task : o->second)
			{
				task->cancel();
			}

			mSquads.erase(o++);
		}
		else ++o;
	}

	mCurrentBehaviour = BuildOrderManager::Instance().getCurrentBuild().getArmyBehaiour();
	for(std::map<SquadType, std::set<BaseSquadTaskPointer>>::iterator o = mSquads.begin(); o != mSquads.end(); ++o)
	{
		for (BaseSquadTaskPointer squad : o->second)
		{
			squad->changeBehaviour(mCurrentBehaviour);
		}
	}
}

BaseSquadTaskPointer SquadManagerClass::createSquad(SquadType type)
{
	BaseSquadTaskPointer task;

	switch(type.underlying())
	{
	case SquadType::DefaultSquad:
		task = BaseSquadTaskPointer(new DefaultSquadTask(mCurrentBehaviour));
		break;
	case SquadType::DefenseSquad:
		task = BaseSquadTaskPointer(new DefenseSquadTask(mCurrentBehaviour));
		break;
	case SquadType::ReaverDropSquad:
		//task = BaseSquadTaskPointer(new ReaverDropSquad()); TODO?
		break;
	case SquadType::DarkTemplerSquad:
		//task = BaseSquadTaskPointer(new DarkTemplerSquad());
		break;
	case SquadType::CorsairSquad:
		//task = BaseSquadTaskPointer(new CorsairSquad());
		break;
	}

	if(task)
	{
		mSquads[type].insert(task);
		TaskManager::Instance().addTask(task);
	}

	return task;
}