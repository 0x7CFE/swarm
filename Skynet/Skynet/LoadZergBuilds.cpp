#include "BuildOrderManager.h"

void BuildOrderManagerClass::LoadZergBuilds()
{
	using namespace BWAPI::Races;
	using namespace BWAPI::UnitTypes;
	using namespace BWAPI::UpgradeTypes;
	using namespace BWAPI::TechTypes;

	int ID_1 = 0;
	int ID_2 = 0;
	int ID_3 = 0;
	
	// worker cnt
	// (default) 4/9
	// worker    4/9
	// worker    5/9
	// worker    6/9
	// worker    7/9
	// worker    8/9
	// worker    9/9
	// overlord  9/9
	// pool      8/17 -
	// worker    9/17
	// worker
	// worker
	// worker
	
	BuildOrder zvpDefault(Zerg, BuildOrderID::ZvPDefault, "ZvP Default");
	zvpDefault.setStartingCondition(Condition(ConditionTest::isEnemyProtoss));
	
	zvpDefault.addNextBuild(BuildOrderID::LairIntoSpiral, Condition(ConditionTest::isEnemyProtoss));
	
	// first of all, building 5 drones
	zvpDefault.addItem(Zerg_Drone, 5, TaskType::Highest);
	zvpDefault.addItem(Zerg_Overlord, 1, TaskType::Highest); // on 9
	zvpDefault.addItem(Zerg_Spawning_Pool, 1, TaskType::Army);
	ID_1 = zvpDefault.addItem(Zerg_Zergling, 3, TaskType::Army);
	
	ID_2 = zvpDefault.addItem(Zerg_Drone, 2, TaskType::Worker/*, CB(ID_1, CallBackType::onDispatched)*/);
	zvpDefault.addOrder(Order::TrainWorkers, CB(ID_2, CallBackType::onDispatched));
	zvpDefault.addOrder(Order::SupplyManager, CB(ID_2, CallBackType::onDispatched));
	
	
	mBuildOrders[BuildOrderID::ZvPDefault] = zvpDefault;

	
	// Protoss will start making corsairs to eliminate our overlords
	BuildOrder lairIntoSpiral(Zerg, BuildOrderID::LairIntoSpiral, "Lair into Spiral");
	lairIntoSpiral.setArmyBehaviour(ArmyBehaviour::Defensive);
	
	lairIntoSpiral.addItem(Zerg_Hatchery, 1, BuildingLocation::Expansion);
	
	// FIXME drones act weird when the hatchery gets mutated into lair
	ID_1 = lairIntoSpiral.addItem(Zerg_Lair, 1, BuildingLocation::Base);
	
	lairIntoSpiral.addItem(Zerg_Spire, 1/*, CB(ID_1, CallBackType::onDispatched)*/);
	lairIntoSpiral.addItem(Zerg_Sunken_Colony, 1, BuildingLocation::Expansion);
	lairIntoSpiral.addItem(Zerg_Sunken_Colony, 1, BuildingLocation::Base);
	
	lairIntoSpiral.addOrder(Order::SupplyManager);
	lairIntoSpiral.addOrder(Order::RefineryManager);
	lairIntoSpiral.addOrder(Order::TrainWorkers);
	lairIntoSpiral.addOrder(Order::MacroArmyProduction);
	
	
	mBuildOrders[BuildOrderID::LairIntoSpiral] = lairIntoSpiral;
	
	
}