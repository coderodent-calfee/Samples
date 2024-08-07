#pragma once

#include "PlayerData.h"
#include <list>

class ZoneQueue;
class ZoneQueueBucket;
class ZoneQueueRequirement;

// ZoneQueue is described with a name and a list of requirements
// it may have a number of players added to it
// each player added must fulfill a requirement (until the max number of that requirements is met)
// 
// ZoneQueueBucket is how the requirements are added to a zone
// the bucket holds a requirement, and any players that match that requirement for the zone

typedef std::shared_ptr<ZoneQueue> ZoneQueuePtr;
typedef std::list <ZoneQueueRequirement> RequirementsList;

class ZoneQueue
{
public:
	ZoneQueue(const std::string & _name, const std::list <ZoneQueueRequirement> & requirements);
	
	bool AddPlayer(const PlayerDataPtr& player);
	bool RemovePlayer(const PlayerDataPtr& player);
	std::list<PlayerDataPtr> GetPlayers() const;

	bool Complete() {
		if (IsReady()) {
			_complete = true;
		}
		return _complete;
	}

	bool IsFull() const;
	bool IsComplete() const {return _complete; }
	bool IsReady() const;
	void report() const;
	unsigned getCount() const { return _count; }
	std::string getName() const { return _name; }

private:
	std::string _name;
	std::list <ZoneQueueBucket> _buckets;
	unsigned _count;
	bool _complete;
};

class ZoneQueueRequirement
{
public:
	ZoneQueueRequirement(PlayerClassEnum classRestriction, unsigned minLevel, unsigned maxLevel, unsigned minPlayers, unsigned maxPlayers) :
		_class(classRestriction),
		_minLevel(minLevel),
		_maxLevel(maxLevel),
		_minPlayers(minPlayers),
		_maxPlayers(maxPlayers) {}
	
	bool  IsMet(const PlayerDataPtr& player) const;
	const PlayerClassEnum _class;
	const unsigned _minLevel;
	const unsigned _maxLevel;
	const unsigned _minPlayers;
	const unsigned _maxPlayers;
};

class ZoneQueueBucket {
public:
	ZoneQueueBucket(ZoneQueueRequirement r) : _requirement(r) {};
	std::list<PlayerDataPtr> _players;
	ZoneQueueRequirement _requirement;
	bool IsFull() const;
	bool IsReady() const;
	bool Has(const PlayerDataPtr& player);
	bool Remove(const PlayerDataPtr& player);

private:
	std::list<PlayerDataPtr>::iterator Find(const PlayerDataPtr& player) {
		const PlayerIdType id = player->getId();
		auto iter = std::find_if(std::begin(_players), std::end(_players),
			[&](const auto p) { return p->getId() == id; });
		return iter;
	}
};