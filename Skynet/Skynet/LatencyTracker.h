#pragma once

#include "Interface.h"

#include "Singleton.h"
#include "Task.h"

class LatencyTrackerClass
{
public:
	LatencyTrackerClass(){}

	void update();

	void placingStorm(Unit unit, Position pos);
	void placingStasis(Unit unit, Position pos);
        
        void placingEnsnare(Unit who, Position where);
        void placingBroodlings(Unit who, Unit target);
        void placingParasite(Unit who, Unit target);

	bool isStormInRange(Unit unit);
	bool isStasisInRange(Unit unit);
        
        bool isSpawnBroodlingPending(Unit victim);
        bool isParasitePending(Unit victim);
        bool isEnsnarePending(Position where);

private:
	std::map<Unit, std::pair<Position, int>> mStormedPositions;
	std::map<Unit, std::pair<Position, int>> mStasisPositions;
        
        std::map<Unit, std::pair<Position, int>> mPendingEnsnares; // TODO
        
        std::map<Unit, Unit> mPendingBroodlings;
        std::set<Unit> mBroodlingVictims;
        
        std::map<Unit, Unit> mPendingParasites;
        std::set<Unit> mParasiteVictims;
};

typedef Singleton<LatencyTrackerClass> LatencyTracker;