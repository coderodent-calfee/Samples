#pragma once

#include "PlayerData.h"
#include "ZoneQueue.h"


//    API
//    You can call your functions anything you want, invent new functions, hang them off of structs / classes, etc.as you see fit.
//    But, to give some idea of what the external API(called by the message pump and a tick function) might look like, consider entry points such as the following :
//
//AddPlayerToQueue(queue, player) or queue.AddPlayer(player)
//Called when a player requests to be added to a queue.Can fail if they don't fit into the queue.
// 
//RemovePlayerFromQueue(queue, player) or queue.RemovePlayer(player)
//Called when a player leaves the game or otherwise leaves a queue.
// 
//QueueMeetsRequirements(queue) or queue.MeetsRequirements()
//Called to determine if the queue is ready to send to a server.
// 
//CompleteQueue(queue) or queue.Complete()
//Called when the queue(and the players in it) have been sent to their new map.
// 
//Queues can be defined as static data in the code.No create / destroy / etc API is needed for them.
//
class SimpleZoneQueueManager;
typedef std::shared_ptr<SimpleZoneQueueManager> SimpleZoneQueueMgrPtr;

class SimpleZoneQueueManager
{
public:
    bool AddPlayerToZone(const PlayerDataPtr & player, const ZoneQueuePtr & zone);
    bool RemovePlayerFromZone(const PlayerDataPtr & player, const ZoneQueuePtr & zone);
    ZoneQueuePtr const & AddZoneQueue(const std::string& _name, const std::list <ZoneQueueRequirement> & requirements);
    
    std::list<ZoneQueuePtr> GetZonesByName(const std::string& name) const;
    std::list<ZoneQueuePtr> GetAllZones() const { return _allZones; };
    std::list<ZoneQueuePtr> GetReadyZones() const;

private:
    std::list<ZoneQueuePtr> _allZones;
};

