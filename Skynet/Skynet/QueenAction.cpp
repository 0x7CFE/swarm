#include "QueenAction.h"

#include "UnitTracker.h"
#include "UnitInformation.h"
#include "LatencyTracker.h"
#include "Logger.h"
#include "UnitHelper.h"

bool QueenAction::castBroodlings(const UnitGroup& allEnemies)
{
	// Checking whether we could cast a spell right now
	if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Spawn_Broodlings) && mUnit->getEnergy() >= BWAPI::TechTypes::Spawn_Broodlings.energyUsed())
	{
		// Ok, we can do the spell. Who will be the target?
		int numTargetting = UnitInformation::Instance().getUnitsTargetting(mUnit).size();

		UnitGroup primaryTargets;     // Units that are best for applying spell
		UnitGroup targetingEnemies;   // Spellable units that are targeting us

		for (Unit unit : allEnemies)
		{
			if(!unit->exists() || unit->isStasised())
				continue;
				
			if(mUnit->getDistance(unit) > BWAPI::TechTypes::Spawn_Broodlings.getWeapon().maxRange())
				continue; // Unit is too far away

			// Primary target for broodlings are siege tanks and high templars
			const BWAPI::UnitType &type = unit->getType();
			if (type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode ||
			    type == BWAPI::UnitTypes::Protoss_High_Templar || 
			    type == BWAPI::UnitTypes::Zerg_Defiler || 
			    type == BWAPI::UnitTypes::Zerg_Ultralisk 
			   ) 
				primaryTargets.insert(unit);

			if (unit->getTarget() == mUnit && unit->getType().isOrganic() && !unit->getType().isFlyer())				
				targetingEnemies.insert(unit);
		}

		bool castUrgently = (numTargetting > 0 && mUnit->totalHitPointFraction() < 0.6);
		
		if (!primaryTargets.empty())
		{
			// FIXME Maybe choose a target randomly
			Unit target = *primaryTargets.begin(); // getClosestUnit();
			mUnit->useTech(BWAPI::TechTypes::Spawn_Broodlings, target);
			return true;
		} else if (castUrgently) {
		      // We're under attack and we MUST do something or we'll die
		      // NOTE Maybe we need to cast another spell? For example Ensnare or at least throw a parasite
		      
		      // Selecting closest unit to minimize damage taken
		      Unit target = primaryTargets.getClosestUnit(mUnit);
		      mUnit->useTech(BWAPI::TechTypes::Spawn_Broodlings, target);
		      return true;
		}
	}
	
	return false;
}

bool QueenAction::castParasite(const UnitGroup& allEnemies)
{
	// Checking whether we could cast a spell right now
	if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Spawn_Broodlings) && mUnit->getEnergy() >= BWAPI::TechTypes::Spawn_Broodlings.energyUsed())
	{
		// Ok, we can do the spell. Who will be the target?

		UnitGroup primaryTargets;     // Units that are best for applying spell
		UnitGroup secondaryTargets;   // All other units that may be parasited

		for (Unit unit : allEnemies)
		{
			if(!unit->exists() || unit->isStasised() || unit->isParasited())
				continue;
				
			if(mUnit->getDistance(unit) > BWAPI::TechTypes::Parasite.getWeapon().maxRange())
				continue; // Unit is too far away

			// Primary target for broodlings are siege tanks and high templars
			const BWAPI::UnitType &type = unit->getType();
			if (
			    // TODO Rewrite using priority map
			    
			    // Detectors
			    type == BWAPI::UnitTypes::Terran_Science_Vessel ||
			    type == BWAPI::UnitTypes::Protoss_Observer || 
			    type == BWAPI::UnitTypes::Zerg_Overlord || 
			    
			    // Transport
			    type == BWAPI::UnitTypes::Terran_Dropship || 
			    type == BWAPI::UnitTypes::Protoss_Shuttle ||
			    
			    // Powerful air units
			    type == BWAPI::UnitTypes::Terran_Battlecruiser ||
			    type == BWAPI::UnitTypes::Protoss_Carrier  ||
			    
			    // Air units
			    type == BWAPI::UnitTypes::Protoss_Arbiter || 
			    type == BWAPI::UnitTypes::Protoss_Scout ||
			    type == BWAPI::UnitTypes::Protoss_Corsair ||
			    type == BWAPI::UnitTypes::Terran_Wraith ||
			    type == BWAPI::UnitTypes::Terran_Valkyrie ||
			    type == BWAPI::UnitTypes::Zerg_Mutalisk ||
			    type == BWAPI::UnitTypes::Zerg_Guardian ||
			    type == BWAPI::UnitTypes::Zerg_Devourer ||
			    
			    // Neutral flying unit
			    type == BWAPI::UnitTypes::Critter_Kakaru  ||

			    // Spellcasters
			    type == BWAPI::UnitTypes::Zerg_Queen || 
			    type == BWAPI::UnitTypes::Zerg_Defiler ||
			    type == BWAPI::UnitTypes::Protoss_High_Templar ||
			    type == BWAPI::UnitTypes::Protoss_Dark_Archon ||  // WARNING May use Feedback spell
			    
			    // Powerful units
			    type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode  ||  // ORLY? 
			    type == BWAPI::UnitTypes::Protoss_Reaver
			) 
				primaryTargets.insert(unit);
			else
				secondaryTargets.insert(unit);
		}
		
		//bool castUrgently = (numTargetting > 0 && mUnit->totalHitPointFraction() < 0.3);
		
		if (!primaryTargets.empty())
		{
		      // TODO Select the unit with the highest priority
		      Unit target = primaryTargets.getClosestUnit(mUnit);
		      mUnit->useTech(BWAPI::TechTypes::Parasite, target);
		      return true;
		} else {
		      // TODO
		}
	}
	
	return false;
}

bool QueenAction::castEnsnare(const UnitGroup& allEnemies)
{
	// TODO Army support. Ensnare a cloaked unit to reveal it when no detectors present
	// TODO Priority list of targets: 
	//		Good against marines: disables the effect of stimpacks 
	//		Corsairs and Mutas power may be greatly reduced with ensnare
	//		As a defence spell ensnare may be useful against incoming enemy army
  
	// Checking whether we could cast a spell right now
	if(BWAPI::Broodwar->self()->hasResearched(BWAPI::TechTypes::Ensnare) && mUnit->getEnergy() >= BWAPI::TechTypes::Ensnare.energyUsed())
	{
		// Ok, we can do the spell. Who will be the target?
		int numTargetting = UnitInformation::Instance().getUnitsTargetting(mUnit).size();

		UnitGroup targetsToChooseFrom;
// 		UnitGroup needsDetecting;

		for (Unit unit : allEnemies)
		{
			// Filtering out units that are already dead, not important or already under the spell
			// TODO Plagued units are preferable target, making them more vulnerable 
			if(!unit->exists() || unit->isStasised() || (unit->getEnsnareTimer() > 0) )
				continue;

			// These units should be skipped. It's just a waste of energy to spell them
			const BWAPI::UnitType &type = unit->getType();
			if(type.isBuilding() || 
			   type == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine || 
			   type == BWAPI::UnitTypes::Zerg_Egg || 
			   type == BWAPI::UnitTypes::Protoss_Interceptor || 
			   type == BWAPI::UnitTypes::Zerg_Larva || 
			   type == BWAPI::UnitTypes::Protoss_Scarab)
				continue;

			// Unit is too far away
			if(mUnit->getDistance(unit) > BWAPI::TechTypes::Ensnare.getWeapon().maxRange())
				continue;

// 			UnitType& unitType = mUnit->getType();
// 			if (! unit->isDetected()) // TODO
// 			{
// 				if(unitType.hasPermanentCloak() || 
// 				   unitType == BWAPI::UnitTypes::Zerg_Lurker || 
// 				   unit->isCloaked() || 
// 				   unit->isBurrowed())
// 					 needsDetecting.insert(unit);
// 			} else 
			
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

			// There are more than 2 targets or there are targets and we need to cast urgently
			if(targets.size() >= 3 || (castUrgently && !targets.empty()))
			{
				// Selecting targets fitting to one good slimy shot :)
				targets = targets.getBestFittingToCircle(64, BWAPI::Broodwar->getRemainingLatencyFrames()+BWAPI::Broodwar->getLatencyFrames());
				if(targets.size() >= 3 || (castUrgently && !targets.empty()))
				{
					// Ok, some units are really grouped enough to be the victim
					// Selecting the center of the group
					BWAPI::Position castLocation = targets.getCenter();

					int ensnaredCount = 0;
					// Finding own units that may fall under the spell
					for (Unit unit : UnitTracker::Instance().selectAllUnits())
					{
						// TODO Supply correct coefficients
						if ( (unit->getEnsnareTimer() == 0) && unit->getDistance(castLocation) < 64)
							++ensnaredCount;

						if(ensnaredCount > 5)
							break; 
					}

					// Ok, the spell is not lethal, so continue
					if (ensnaredCount <= 5)
					{
						// Checking whether the target's center is in the firing range
						int distanceToCastLocation = mUnit->getDistance(castLocation);
						if(distanceToCastLocation <= BWAPI::TechTypes::Ensnare.getWeapon().maxRange())
						{
							// Time to be slimed, LOL!
							mUnit->useTech(BWAPI::TechTypes::Ensnare, castLocation);
							// TODO LatencyTracker::Instance().placingEnsnare(mUnit, castLocation);
						}
						else
						{
							// Oops, seems that cast location is too far
							// Firing a bit closer
							
							Vector newCastLocation = mUnit->getPosition() - castLocation;

							newCastLocation.normalise();
							newCastLocation *= float(distanceToCastLocation - BWAPI::TechTypes::Ensnare.getWeapon().maxRange());
							newCastLocation += Vector(castLocation);

							mUnit->useTech(BWAPI::TechTypes::Ensnare, newCastLocation);
							// TODO LatencyTracker::Instance().placingEnsnare(mUnit, newCastLocation);
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
	
	return true;
}

// Defined in BasicUnitAction.cpp. 
void stayAtRange(Unit unit, Position targetPositon, int maxRange, int currentRange);

bool QueenAction::update(const Goal &squadGoal, const UnitGroup &squadUnitGroup)
{
	// Checking whether queen is already casting a spell
	if(mUnit->getOrder() == BWAPI::Orders::CastEnsnare || 
	   mUnit->getOrder() == BWAPI::Orders::CastParasite ||
	   mUnit->getOrder() == BWAPI::Orders::CastSpawnBroodlings)
		return true;

	bool engaged = false;
	const UnitGroup& allEnemies = UnitTracker::Instance().selectAllEnemy();
	
	// Some spell had just been casted, need to wait for some time
	if(mUnit->getSpellCooldown() > BWAPI::Broodwar->getRemainingLatencyFrames())
		return false;
	else {
		
		// NOTE Current implementation of spell casters is almost independent.
		//	Maybe we should gather all target information and then decide what spell to apply
		
		if (castBroodlings(allEnemies))
		    engaged = true;
		
		if (castParasite(allEnemies))
		    engaged = true;
		
		if (castEnsnare(allEnemies))
		    engaged = true;
	}

	// If no spell is engaged we need to check our position to be optimal
	if (!engaged) 
	{
		// Need to be close to our units, but still not too close to the enemy
		// TODO Special case for Dark Archon and High Templar
		int maxRange = 0;
		Unit sniperUnit;
		for (Unit enemy : allEnemies) 
		{
			if (mUnit->getDistance(enemy) > 300) // TODO constant
			      continue; // this unit is too far away to be afraid of
		  
			int currentRange = enemy->getWeaponMaxRange(mUnit);
			if (currentRange > maxRange)
			{
				maxRange = currentRange;
				sniperUnit = enemy;
			}
		}
		
		if (mUnit->getDistance(sniperUnit) < maxRange)
		{
			// We're too close to enemy. Running away
			stayAtRange(mUnit, 
				    sniperUnit->getPosition(), 
				    maxRange + 64, 
				    sniperUnit->getDistance(mUnit));
			return true;
		} else {
			// Enemies are not so close. We may hold our group
			UnitGroup unitsToStayNearby;
			for (Unit unit : squadUnitGroup)
			{
				if(!UnitHelper::isArmyUnit(unit->getType()))
					continue;

				if(unit->getType() == BWAPI::UnitTypes::Zerg_Queen || unit->getType().isBuilding())
					continue;

				if(mUnit->getDistance(unit) > 250) // TODO What constant should we use?
					continue;

				unitsToStayNearby.insert(unit);
			}
			
			unitsToStayNearby = unitsToStayNearby.getBestFittingToCircle(136); // TODO Fix constant
			if(!unitsToStayNearby.empty())
			{
				Position squadLocation = unitsToStayNearby.getCenter();
				if(mUnit->getDistance(squadLocation) > 300)
				{
					mUnit->move(squadLocation);
					return true;
				}
			}
		}
	}
	
	return engaged;
}