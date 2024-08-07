#include "SimpleZoneQueueManager.h"

bool SimpleZoneQueueManager::AddPlayerToZone(const PlayerDataPtr & player, const ZoneQueuePtr & zone){
    bool added = zone->AddPlayer(player);
    return added;
}


ZoneQueuePtr const & SimpleZoneQueueManager::AddZoneQueue(const std::string & name, const std::list <ZoneQueueRequirement> & requirements) {
    ZoneQueuePtr zone = std::make_shared<ZoneQueue>(name, requirements);
    _allZones.push_back(zone);
    return _allZones.back();
}


bool SimpleZoneQueueManager::RemovePlayerFromZone(const PlayerDataPtr& player, const ZoneQueuePtr& zone) {
    bool result = zone->RemovePlayer(player);
    return result;
}

std::list<ZoneQueuePtr> SimpleZoneQueueManager::GetZonesByName(const std::string& name) const {
    std::list<ZoneQueuePtr> output;
    for (auto zone : _allZones) {
        if (zone->getName() == name) {
            output.push_back(zone);
        }
    }
    return output;
}

std::list<ZoneQueuePtr> SimpleZoneQueueManager::GetReadyZones() const {
    std::list<ZoneQueuePtr> output;
    for (auto zone : _allZones) {
        if (zone->IsReady()) {
            output.push_back(zone);
        }
    }
    return output;
}
