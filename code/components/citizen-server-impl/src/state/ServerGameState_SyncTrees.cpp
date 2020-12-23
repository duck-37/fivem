#include "StdInc.h"
#include <state/SyncTrees_Five.h>

namespace fx
{
std::shared_ptr<sync::SyncTreeImplBase> MakeSyncTree(sync::NetObjEntityType objectType)
{
	switch (objectType)
	{
	case sync::NetObjEntityType::Automobile:
		return std::make_shared<sync::SyncTreeImpl<sync::CAutomobileSyncTree>>();
	case sync::NetObjEntityType::Bike:
		return std::make_shared<sync::SyncTreeImpl<sync::CBikeSyncTree>>();
	case sync::NetObjEntityType::Boat:
		return std::make_shared<sync::SyncTreeImpl<sync::CBoatSyncTree>>();
	case sync::NetObjEntityType::Door:
		return std::make_shared<sync::SyncTreeImpl<sync::CDoorSyncTree>>();
	case sync::NetObjEntityType::Heli:
		return std::make_shared<sync::SyncTreeImpl<sync::CHeliSyncTree>>();
	case sync::NetObjEntityType::Object:
		return std::make_shared<sync::SyncTreeImpl<sync::CObjectSyncTree>>();
	case sync::NetObjEntityType::Ped:
		return std::make_shared<sync::SyncTreeImpl<sync::CPedSyncTree>>();
	case sync::NetObjEntityType::Pickup:
		return std::make_shared<sync::SyncTreeImpl<sync::CPickupSyncTree>>();
	case sync::NetObjEntityType::PickupPlacement:
		return std::make_shared<sync::SyncTreeImpl<sync::CPickupPlacementSyncTree>>();
	case sync::NetObjEntityType::Plane:
		return std::make_shared<sync::SyncTreeImpl<sync::CPlaneSyncTree>>();
	case sync::NetObjEntityType::Submarine:
		return std::make_shared<sync::SyncTreeImpl<sync::CSubmarineSyncTree>>();
	case sync::NetObjEntityType::Player:
		return std::make_shared<sync::SyncTreeImpl<sync::CPlayerSyncTree>>();
	case sync::NetObjEntityType::Trailer:
		return std::make_shared<sync::SyncTreeImpl<sync::CAutomobileSyncTree>>();
	case sync::NetObjEntityType::Train:
		return std::make_shared<sync::SyncTreeImpl<sync::CTrainSyncTree>>();
	}

	return {};
}
}
