#include <BWAPI.h>
#include <boost/smart_ptr/enable_shared_from_this.hpp>

typedef BWAPI::TilePosition TilePosition;
typedef BWAPI::Position Position;
typedef BWAPI::Player* Player;

class UnitClass;
typedef std::shared_ptr<UnitClass> Unit;

#include "Vector.h"
#include "WalkPosition.h"
#include "Unit.h"
#include "UnitGroup.h"