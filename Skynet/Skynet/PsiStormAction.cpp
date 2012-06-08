#include "PsiStormAction.h"

#include "UnitTracker.h"
#include "UnitInformation.h"
#include "LatencyTracker.h"
#include "Logger.h"

bool PsiStormAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	if(mUnit->getOrder() == BWAPI::Orders::CastPsionicStorm)
		return true;

	//TODO : Morph arcon temp in here with some non ideal code till group micro actions is complete, checks if it can actually morph the archon,
	// ie not using 3.6
	static bool allowsArchonMorph = true;
	if(allowsArchonMorph)
	{
		if(mUnit->getOrder() == BWAPI::Orders::ArchonWarp)
			return true;

		// If we're not in the battlefield but in the base, do not try to warp
		bool warpPriorityLow = squadGoal.getActionType() != ActionType::Attack || squadGoal.getGoalType() != GoalType::Base || mUnit->getPosition().getApproxDistance(squadGoal.getBase()->getCenterLocation()) > 32*20;
		const int energyToTransformAt = warpPriorityLow ? 48 : 65;
		if(mUnit->getEnergy() <= energyToTransformAt)
		{
			// We need to warp. Find a pair to warp with
			Unit otherTemp;
			int closestDistance = std::numeric_limits<int>::max();
			for (Unit ht : UnitTracker::Instance().selectAllUnits(BWAPI::UnitTypes::Protoss_High_Templar))
			{
				// We could not warp with ourselves
				if(ht == mUnit)
					continue;

				// This templar is already warping with someone :( Foreveralone.jpg
				if(ht->getOrder() == BWAPI::Orders::ArchonWarp)
					continue;

				// Other unit has too much energy and may be useful as is
				if(ht->getEnergy() > energyToTransformAt)
					continue;

				int distance = mUnit->getDistance(ht);
				if(distance < closestDistance)
				{
					// Yeah, come on baby, make me feel good!
					closestDistance = distance;
					otherTemp = ht;
				}
			}

			if(otherTemp)
			{	
				// Not sure what is going on here. Seems that we're checking 
				// whether our pair has a better candidate than us
			  
				Unit otherClosestTemp;
				int otherClosestDistance = std::numeric_limits<int>::max();
				for (Unit ht : UnitTracker::Instance().selectAllUnits(BWAPI::UnitTypes::Protoss_High_Templar))
				{
					if(ht == otherTemp)
						continue;

					if(ht->getOrder() == BWAPI::Orders::ArchonWarp)
						continue;

					if(ht->getEnergy() > energyToTransformAt)
						continue;

					int distance = otherTemp->getDistance(ht);
					if(distance < otherClosestDistance)
					{
						otherClosestDistance = distance;
						otherClosestTemp = ht;
					}
				}

				if(otherClosestTemp == mUnit)
				{
					// If other templar thinks that we're the best, then lets do our dirty thingie ;>
				  
					mUnit->useTech(BWAPI::TechTypes::Archon_Warp, otherTemp);
					otherTemp->useTech(BWAPI::TechTypes::Archon_Warp, mUnit);

					if(BWAPI::Broodwar->getLastError() == BWAPI::Errors::Incompatible_TechType)
					{
						LOGMESSAGEERROR("Archon Warp Failed, Old BWAPI Version?");
						allowsArchonMorph = false;
					}

					return true;
				}
			}
		}
	}

	if(mUnit->getSpellCooldown() > BWAPI::Broodwar->getRemainingLatencyFrames())
		return false;

	// Checking whether we could cast a storm right now
	if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Psionic_Storm) && mUnit->getEnergy() >= BWAPI::TechTypes::Psionic_Storm.energyUsed())
	{
		// Ok, we can do the storm. Who will be the target?
	  
		int numTargetting = UnitInformation::Instance().getUnitsTargetting(mUnit).size();

		UnitGroup targetsToChooseFrom;

		for (Unit unit : UnitTracker::Instance().selectAllEnemy())
		{
			// Filtering out units that are already dead, not important or already under the storm
			if(!unit->exists() || unit->isStasised() || unit->isUnderStorm())
				continue;

			// These units are too fast or too cheap to be the target (not effective)
			const BWAPI::UnitType &type = unit->getType();
			if(type.isBuilding() || 
			   type == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine || 
			   type == BWAPI::UnitTypes::Zerg_Egg || 
			   type == BWAPI::UnitTypes::Protoss_Interceptor || 
			   type == BWAPI::UnitTypes::Zerg_Larva || 
			   type == BWAPI::UnitTypes::Protoss_Scarab)
				continue;

			// Unit is too far away
			if(mUnit->getDistance(unit) > BWAPI::TechTypes::Psionic_Storm.getWeapon().maxRange())
				continue;

			// Ok, seems this unit could be the victim
			targetsToChooseFrom.insert(unit);
		}

		UnitGroup targets(targetsToChooseFrom);

		bool castUrgently = (numTargetting > 2 || (numTargetting > 0 && mUnit->totalHitPointFraction() < 0.6));

		int tries = 0;
		bool tryAgain = true;
		while(tryAgain)
		{
			tryAgain = false;

			/* There are 3 targets or there are targets and we need to cast urgently */
			if(targets.size() >= 3 || (castUrgently && !targets.empty()))
			{
				// Selecting targets fitting to one shot space
				targets = targets.getBestFittingToCircle(64, BWAPI::Broodwar->getRemainingLatencyFrames()+BWAPI::Broodwar->getLatencyFrames());
				if(targets.size() >= 3 || (castUrgently && !targets.empty()))
				{
					// Ok, some units are really grouped enough to be the victim
					// Selecting the center of group
					BWAPI::Position castLocation = targets.getCenter();

					int inStormCount = 0;
					for (Unit unit : UnitTracker::Instance().selectAllUnits())
					{
						if(!unit->isUnderStorm() && unit->getDistance(castLocation) < 64)
							++inStormCount;

						if(inStormCount > 1)
							break; 
					}

					if(inStormCount <= 1)
					{
						int distanceToStorm = mUnit->getDistance(castLocation);
						if(distanceToStorm <= BWAPI::TechTypes::Psionic_Storm.getWeapon().maxRange())
						{
							mUnit->useTech(BWAPI::TechTypes::Psionic_Storm, castLocation);
							LatencyTracker::Instance().placingStorm(mUnit, castLocation);
						}
						else
						{
							Vector newCastLocation = mUnit->getPosition() - castLocation;

							newCastLocation.normalise();

							newCastLocation *= float(distanceToStorm - BWAPI::TechTypes::Psionic_Storm.getWeapon().maxRange());

							newCastLocation += Vector(castLocation);

							mUnit->useTech(BWAPI::TechTypes::Psionic_Storm, newCastLocation);
							LatencyTracker::Instance().placingStorm(mUnit, newCastLocation);
						}

						return true;
					}
					else
					{
						++tries;
						if(tries < 3)
						{
							targets = targetsToChooseFrom;
							for (Unit unit : targetsToChooseFrom)
							{
								if(unit->getDistance(castLocation) < 64)
									targets.erase(unit);
							}

							if(targets.size() >= 3 || (castUrgently && !targets.empty()))
								tryAgain = true;
						}
					}
				}
			}
		}
	}

	return false;
}