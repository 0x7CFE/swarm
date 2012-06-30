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
	
	// This is default starting point for ZvP game
	
	BuildOrder zvpDefault(Zerg, BuildOrderID::ZvPDefault, "ZvP Default");
	zvpDefault.setStartingCondition(Condition(ConditionTest::isEnemyProtoss));
	zvpDefault.addNextBuild(BuildOrderID::HydraPreExpand, Condition(ConditionTest::isEnemyProtoss));
//	zvpDefault.addNextBuild(BuildOrderID::HydraPreExpand, Condition(ConditionTest::myPlannedUnitTotalGreaterEqualThan, Zerg_Hatchery, 2));
	
	// first of all, building 5 drones
	zvpDefault.addItem(Zerg_Drone, 5, TaskType::Highest);
	zvpDefault.addItem(Zerg_Overlord, 1, TaskType::Highest); // on 9
	ID_1 = zvpDefault.addItem(Zerg_Spawning_Pool, 1, TaskType::Highest);
	zvpDefault.addItem(Zerg_Drone, 3, TaskType::Highest); //, CB(ID_1, CallBackType::onStarted));
	ID_2 = zvpDefault.addItem(Zerg_Zergling, 3, TaskType::Highest);
	       zvpDefault.addOrder(Order::Scout, CB(ID_2, CallBackType::onStarted)); // scouting until choke gets blocked
	ID_2 = zvpDefault.addItem(Zerg_Drone, CB(ID_1, CallBackType::onDispatched), 3);
	
	
	ID_3 = zvpDefault.addItem(Zerg_Hatchery, 1, BuildingLocation::Expansion);
        //ID_3 = zvpDefault.addItem(Zerg_Creep_Colony, CB(ID_3, CallBackType::onCompleted), 1, BuildingLocation::Expansion); 
        //ID_3 = zvpDefault.addItem(Zerg_Sunken_Colony, CB(ID_3, CallBackType::onCompleted), 1, BuildingLocation::Expansion);
	
	//zvpDefault.addOrder(Order::Scout, CB(ID_1, CallBackType::onDispatched)); // Send is too early
	zvpDefault.addOrder(Order::TrainWorkers, CB(ID_2, CallBackType::onDispatched));
	zvpDefault.addOrder(Order::SupplyManager, CB(ID_2, CallBackType::onDispatched));
	
	mBuildOrders[BuildOrderID::ZvPDefault] = zvpDefault;
	
	// This build order is designed to overcome first protoss attack (mainly zealots)
	// Then expand to the Natural
	
	BuildOrder hydraPreExpand(Zerg, BuildOrderID::HydraPreExpand, "HydraPreExpand");
	hydraPreExpand.setArmyBehaviour(ArmyBehaviour::Default);
	hydraPreExpand.addNextBuild(BuildOrderID::ZvPMiddle, Condition(ConditionTest::isEnemyProtoss)); 
        //Condition(ConditionTest::myPlannedUnitTotalGreaterEqualThan, Zerg_Hydralisk, 16));
        //, Condition(ConditionTest::myUnitTotalBuildCountGreaterEqualThan, Zerg_Hatchery, 3));
	hydraPreExpand.setDefaultBuild(BuildOrderID::ZvPMiddle, 24*60*10); // NOTE seconds or frames?
	
	ID_1 = hydraPreExpand.addItem(Zerg_Extractor, 1, TaskType::Highest);
	       hydraPreExpand.addItem(Zerg_Drone, CB(ID_1, CallBackType::onDispatched));
	     //  hydraPreExpand.addOrder(Order::RefineryManager, CB(ID_1, CallBackType::onDispatched));
	    //   hydraPreExpand.addItem(Zerg_Drone, CB(ID_1, CallBackType::onDispatched), 4);
	       
	ID_1 = hydraPreExpand.addItem(Zerg_Hydralisk_Den, TaskType::Highest, CB(ID_1, CallBackType::onCompleted));
	       hydraPreExpand.addItem(Zerg_Drone, CB(ID_1, CallBackType::onStarted), 1);
	
        ID_2 = hydraPreExpand.addItem(Zerg_Zergling, 5, TaskType::Highest);
        ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest, CB(ID_1, CallBackType::onCompleted), 2);
        ID_2 = hydraPreExpand.addItem(Zerg_Zergling,  TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 3);
        ID_3 = hydraPreExpand.addItem(Zerg_Evolution_Chamber, CB(ID_2, CallBackType::onStarted));
        ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 2);
        ID_2 = hydraPreExpand.addItem(Zerg_Drone,     TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 1);
        ID_2 = hydraPreExpand.addItem(Zerg_Zergling,  TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 4);
        
        ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 4);
        ID_3 = hydraPreExpand.addItem(Zerg_Carapace, 1, CB(ID_2, CallBackType::onCompleted));
        ID_2 = hydraPreExpand.addItem(Zerg_Zergling,  TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 3);
        ID_2 = hydraPreExpand.addItem(Zerg_Drone,     TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 1);
               //hydraPreExpand.addOrder(Order::MacroProductionFacilities); 
        ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 4);
               // TODO If enough money
        
        ID_3 = hydraPreExpand.addItem(Zerg_Hatchery, CB(ID_2, CallBackType::onCompleted), 1);
               hydraPreExpand.addItem(Metabolic_Boost, 1, CB(ID_2, CallBackType::onStarted));
        ID_3 = hydraPreExpand.addItem(Grooved_Spines, 1, CB(ID_2, CallBackType::onStarted));
               hydraPreExpand.addItem(Muscular_Augments, 1, CB(ID_3, CallBackType::onDispatched));
        
        ID_2 = hydraPreExpand.addItem(Zerg_Zergling,  TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 3);
        ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 5);
        ID_2 = hydraPreExpand.addItem(Zerg_Drone,     TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 1);
        ID_3 = hydraPreExpand.addItem(Zerg_Hatchery, CB(ID_2, CallBackType::onCompleted), 1);
        //ID_2 = hydraPreExpand.addItem(Zerg_Zergling,  TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 6);
        //ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest, CB(ID_2, CallBackType::onCompleted), 6);

//         ID_2 = hydraPreExpand.addItem(Zerg_Zergling, 20, TaskType::Highest);
// 	ID_2 = hydraPreExpand.addItem(Zerg_Hydralisk, TaskType::Highest,  CB(ID_1, CallBackType::onCompleted), 25);

	//ID_2 = hydraPreExpand.addItem(Zerg_Hatchery, TaskType::Highest, CB(ID_1, CallBackType::onCompleted), 1, BuildingLocation::BaseParimeter);
//	hydraPreExpand.addItem(Zerg_Drone, CB(ID_2, CallBackType::onStarted), 4);

	       // TODO Place properly
//  	ID_2 = hydraPreExpand.addItem(Zerg_Creep_Colony, CB(ID_1, CallBackType::onCompleted), 2, BuildingLocation::Expansion); 
//  	       hydraPreExpand.addItem(Zerg_Drone, CB(ID_2, CallBackType::onStarted));
//  	ID_2 = hydraPreExpand.addItem(Zerg_Sunken_Colony, CB(ID_2, CallBackType::onCompleted), 2, BuildingLocation::Expansion);
	
	hydraPreExpand.addProduce(Zerg_Zergling, 6);
	hydraPreExpand.addProduce(Zerg_Hydralisk, 4);
        hydraPreExpand.addProduce(Zerg_Drone, 1);
	
	hydraPreExpand.addSquad(SquadType::DefaultSquad);
	
	hydraPreExpand.addOrder(Order::SupplyManager);
	hydraPreExpand.addOrder(Order::TrainWorkers, CB(ID_1, CallBackType::onDispatched));
	hydraPreExpand.addOrder(Order::MacroArmyProduction);
	
	mBuildOrders[BuildOrderID::HydraPreExpand] = hydraPreExpand;
	
	// This build order 
	BuildOrder zvpMiddle(Zerg, BuildOrderID::ZvPMiddle, "ZvP Middle");
	zvpMiddle.setArmyBehaviour(ArmyBehaviour::Default);
        zvpMiddle.addNextBuild(BuildOrderID::ZvPEnd, Condition(ConditionTest::isEnemyProtoss));
        zvpMiddle.setDefaultBuild(BuildOrderID::ZvPEnd, 24*60*8);
	
	ID_1 = zvpMiddle.addItem(Zerg_Lair, 1, TaskType::Highest);
//	       zvpMiddle.addItem(Pneumatized_Carapace, 1, CB(ID_2, CallBackType::onCompleted));
	
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
//         zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
//         zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
//         zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
//         zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
//         zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Zergling, 3, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Drone, 1, TaskType::Highest);
        zvpMiddle.addItem(Zerg_Queen, 1, TaskType::Highest);
	zvpMiddle.addItem(Zerg_Hydralisk, 2, TaskType::Highest);
	
	zvpMiddle.addSquad(SquadType::DefaultSquad);
//	zvpMiddle.addSquad(SquadType::DefenseSquad);
	
	//ID_3 = zvpMiddle.addItem(Burrowing, TaskType::Highest/*, CB(ID_2, CallBackType::onCompleted)*/);
	zvpMiddle.addOrder(Order::Scout, CB(ID_1, CallBackType::onCompleted));
	
//	ID_1 =	zvpMiddle.addItem(Grooved_Spines, 1, CB(ID_1, CallBackType::onDispatched));
//	ID_1 =	zvpMiddle.addItem(Muscular_Augments, 1, CB(ID_1, CallBackType::onCompleted));
//	ID_1 =	zvpMiddle.addItem(Metabolic_Boost, 1, CB(ID_1, CallBackType::onDispatched));
	
        ID_1 =  zvpMiddle.addItem(Lurker_Aspect, CB(ID_1, CallBackType::onCompleted));
	//ID_1 =  zvpMiddle.addItem(Antennae, 1, CB(ID_1, CallBackType::onCompleted));
	ID_1 = zvpMiddle.addItem(Zerg_Evolution_Chamber, CB(ID_1, CallBackType::onDispatched));
	zvpMiddle.addItem(Zerg_Queen, CB(ID_1, CallBackType::onDispatched), 3);
        zvpMiddle.addItem(Pneumatized_Carapace, 1, CB(ID_1, CallBackType::onDispatched));
	
	zvpMiddle.addProduce(Zerg_Zergling, 30, 150);
	zvpMiddle.addProduce(Zerg_Hydralisk, 20, 150);
        zvpMiddle.addProduce(Zerg_Drone, 1, 100);
        zvpMiddle.addProduce(Zerg_Queen, 3, 160);//, Condition(ConditionTest::isResearching, Spawn_Broodlings)); 
	//zvpMiddle.addProduce(Zerg_Mutalisk, 1);
	
	//Condition(ConditionTest::myPlannedUnitTotalGreaterEqualThan, Zerg_Queens_Nest, 1));
//	zvpMiddle.addProduce(Zerg_Defiler, 8, 100, Condition(ConditionTest::isResearching, Plague));
	zvpMiddle.addProduce(Zerg_Lurker, 6, 145, Condition(ConditionTest::isResearching, Lurker_Aspect)); // TODO Lurker operation
	/*zvpMiddle.addProduce(Zerg_Scourge, 8, 115, 
			     Condition(ConditionTest::enemyUnitCountGreaterEqualThan, Protoss_Corsair, 5) || 
			     Condition(ConditionTest::enemyUnitCountGreaterEqualThan, Protoss_Shuttle, 1) || 
			     Condition(ConditionTest::enemyUnitCountGreaterEqualThan, Protoss_Scout, 3)
			    );*/
	
        zvpMiddle.addOrder(Order::ExpansionManager);
        zvpMiddle.addOrder(Order::RefineryManager);
	zvpMiddle.addOrder(Order::SupplyManager);
	zvpMiddle.addOrder(Order::TrainWorkers);
	zvpMiddle.addOrder(Order::MacroArmyProduction);
	zvpMiddle.addOrder(Order::MacroCanTech);
	zvpMiddle.addOrder(Order::MacroProductionFacilities); // For zerg this should handle additional hatcheries
	zvpMiddle.addOrder(Order::CanRemoveSquads); // What does this mean?
	
	mBuildOrders[BuildOrderID::ZvPMiddle] = zvpMiddle;
	
	BuildOrder zvpEnd(Zerg, BuildOrderID::ZvPEnd, "ZvP End");
	//zvpEnd.addNextBuild(BuildOrderID::ZvPEnd, 24*60*15);
	
	zvpEnd.setArmyBehaviour(ArmyBehaviour::Aggresive);
	zvpEnd.addProduce(Zerg_Zergling, 20, 150);
	zvpEnd.addProduce(Zerg_Hydralisk, 15, 150);
	
	zvpEnd.addProduce(Zerg_Queen, 5, 200/*, Condition(ConditionTest::myPlannedUnitTotalGreaterEqualThan, Zerg_Queens_Nest, 1)*/);
	zvpEnd.addProduce(Zerg_Defiler, 5, 150/*, Condition(ConditionTest::isResearching, Plague)*/);
	zvpEnd.addProduce(Zerg_Lurker, 6, 150, Condition(ConditionTest::isResearched, Lurker_Aspect));
	zvpEnd.addProduce(Zerg_Scourge, 10, 115, Condition(ConditionTest::enemyUnitCountGreaterEqualThan, Protoss_Corsair, 1) || Condition(ConditionTest::enemyUnitCountGreaterEqualThan, Protoss_Shuttle, 1));
	
	zvpEnd.addSquad(SquadType::DefaultSquad);
//         zvpEnd.addSquad(SquadType::DefenseSquad);
	
	zvpEnd.addOrder(Order::Scout);
        zvpEnd.addOrder(Order::RefineryManager);
	zvpEnd.addOrder(Order::ExpansionManager);
	zvpEnd.addOrder(Order::SupplyManager);
	zvpEnd.addOrder(Order::TrainWorkers);
	zvpEnd.addOrder(Order::MacroArmyProduction);
	zvpEnd.addOrder(Order::MacroCanTech);
	zvpEnd.addOrder(Order::MacroProductionFacilities); // For zerg this should handle additional hatcheries
	zvpEnd.addOrder(Order::CanRemoveSquads); // What does this mean?
	
	mBuildOrders[BuildOrderID::ZvPEnd] = zvpEnd;
	
}