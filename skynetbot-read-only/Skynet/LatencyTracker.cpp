#include "LatencyTracker.h"

const int StormRadius = 64;
const int StasisRadius = 96;

void LatencyTrackerClass::update()
{
        const int currentTime = BWAPI::Broodwar->getFrameCount();
        
        if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Protoss)
        {
                for(std::map<Unit, std::pair<Position, int>>::iterator it = mStormedPositions.begin(); it != mStormedPositions.end();)
                {
                        if(!it->first->exists() || (it->first->getOrder() != BWAPI::Orders::CastPsionicStorm && currentTime > it->second.second))
                                mStormedPositions.erase(it++);
                        else
                                ++it;
                }

                for(std::map<Unit, std::pair<Position, int>>::iterator it = mStasisPositions.begin(); it != mStasisPositions.end();)
                {
                        if(!it->first->exists() || (it->first->getOrder() != BWAPI::Orders::CastStasisField && currentTime > it->second.second))
                                mStasisPositions.erase(it++);
                        else
                                ++it;
                }
        } else if (BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg)
        {
                for (std::map<Unit, std::pair<Unit, int>>::iterator iBroodling = mPendingBroodlings.begin(); iBroodling != mPendingBroodlings.end(); )
                {
                        int timeWhenCasted = iBroodling->second.second;
                        if (!iBroodling->first->exists() || (iBroodling->first->getOrder() != BWAPI::Orders::CastSpawnBroodlings && currentTime > timeWhenCasted))
                        {
                                Unit target = iBroodling->second.first;
                                mBroodlingVictims.erase(target);
                                mPendingBroodlings.erase(iBroodling++);
                        } else
                                ++iBroodling;
                }
                
                for (std::map<Unit, std::pair<Unit, int>>::iterator iParasite = mPendingParasites.begin(); iParasite != mPendingParasites.end(); )
                {
                        int timeWhenCasted = iParasite->second.second;
                        if (!iParasite->first->exists() || (iParasite->first->getOrder() != BWAPI::Orders::CastParasite && currentTime > timeWhenCasted))
                        {
                                Unit target = iParasite->second.first;
                                mParasiteVictims.erase(target);
                                mPendingParasites.erase(iParasite++);
                        } else
                                ++iParasite;
                }
        }
}

void LatencyTrackerClass::placingStorm(Unit unit, Position pos)
{
        mStormedPositions[unit] = std::make_pair(pos, BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->getRemainingLatencyFrames());
}

void LatencyTrackerClass::placingStasis(Unit unit, Position pos)
{
        mStasisPositions[unit] = std::make_pair(pos, BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->getRemainingLatencyFrames());
}

bool LatencyTrackerClass::isStormInRange(Unit unit)
{
        if(mStormedPositions.empty())
                return false;

        for(std::map<Unit, std::pair<Position, int>>::iterator it = mStormedPositions.begin(); it != mStormedPositions.end(); ++it)
        {
                if(it->second.first.getApproxDistance(unit->getPosition()) <= StormRadius)
                        return true;
        }

        return false;
}

bool LatencyTrackerClass::isStasisInRange(Unit unit)
{
        if(mStasisPositions.empty())
                return false;

        for(std::map<Unit, std::pair<Position, int>>::iterator it = mStasisPositions.begin(); it != mStasisPositions.end(); ++it)
        {
                if(it->second.first.getApproxDistance(unit->getPosition()) <= StasisRadius)
                        return true;
        }

        return false;
}

void LatencyTrackerClass::placingEnsnare ( Unit who, Position where ) {
        mPendingEnsnares[who] = std::make_pair(where, BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->getRemainingLatencyFrames());
}

void LatencyTrackerClass::placingBroodlings ( Unit who, Unit target ) {
        mPendingBroodlings[who] = std::make_pair(target, BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->getRemainingLatencyFrames());
        mBroodlingVictims.insert(target);
}

void LatencyTrackerClass::placingParasite ( Unit who, Unit target ) {
        mPendingParasites[who] = std::make_pair(target, BWAPI::Broodwar->getFrameCount() + BWAPI::Broodwar->getRemainingLatencyFrames() + 64);
        mParasiteVictims.insert(target);
}

bool LatencyTrackerClass::isSpawnBroodlingPending ( Unit victim ) {
        return mBroodlingVictims.find(victim) != mBroodlingVictims.end();
}

bool LatencyTrackerClass::isParasitePending ( Unit victim ) {
        return mParasiteVictims.find(victim) != mParasiteVictims.end();
}

bool LatencyTrackerClass::isEnsnarePending ( Position where ) {
        return false; // TODO
}

