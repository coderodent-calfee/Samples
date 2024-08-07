#include <iostream>
#include <ostream>


#include "ZoneQueue.h"


ZoneQueue::ZoneQueue(const std::string & name, const std::list <ZoneQueueRequirement> & requirements) : _name(name), _complete(false), _count(0){
	for (auto& requirement : requirements) {
		ZoneQueueBucket bucket(requirement);
		_buckets.push_back(bucket);
	}
}

bool ZoneQueue::AddPlayer(const PlayerDataPtr& player) {
	if (IsComplete()) { return false; }
	if (IsFull()) { return false; }
	for (auto& bucket : _buckets) {
		if (bucket._requirement.IsMet(player)) {
			if (bucket.Has(player)) {
				return false; // the player is already in this queue; check no more buckets
			}
			if (bucket.IsFull()) {
				continue;
			}
			bucket._players.push_back(player);
			_count++;
			return true;
		}
	}
	return false;
}

bool ZoneQueue::RemovePlayer(const PlayerDataPtr&  player) {
	for (auto& bucket : _buckets) {
		if (bucket._requirement.IsMet(player)) {
			if (bucket.Remove(player)) {
				_count--;
				return true;
			}
		}
	}
	return false;
}

bool ZoneQueue::IsFull() const {
	for (auto& bucket : _buckets) {
		if (bucket.IsFull() == false) {
			return false;
		}
	}
	return true;
}


bool ZoneQueue::IsReady() const {
	for (auto& bucket : _buckets) {
		if (bucket.IsReady() == false) {
			return false;
		}
	}
	return true;
}


std::list<PlayerDataPtr> ZoneQueue::GetPlayers() const  {
	std::list<PlayerDataPtr> playersInQueue;
	for (auto& bucket : _buckets) {
		playersInQueue.insert(playersInQueue.end(), bucket._players.begin(), bucket._players.end());
	}
	return playersInQueue;
}

void ZoneQueue::report() const {
	std::cout << std::endl << "ZoneQueue:" << _name << " isReady:" << IsReady() << " isFull:" << IsFull() << " isComplete:" << IsComplete() << std::endl;

	for (auto& bucket : _buckets) {
		std::cout << "Requirement: class " << PlayerData::getClassName(bucket._requirement._class);
		std::cout << ", level " << bucket._requirement._minLevel << "-" << bucket._requirement._maxLevel << " " << " isReady:" << bucket.IsReady() << std::endl;
		for (auto player : bucket._players) {
			std::cout << "\t"; 
			player->dump();
		}
	}
}

bool ZoneQueueBucket::IsFull() const {
	return _players.size() == _requirement._maxPlayers;
}

bool ZoneQueueBucket::IsReady() const {
	return _players.size() >= _requirement._minPlayers;
}

bool ZoneQueueBucket::Has(const PlayerDataPtr& player) {
	return Find(player) != _players.end();
}


bool ZoneQueueBucket::Remove(const PlayerDataPtr& player) {
	auto iter = Find(player);
	if (iter == _players.end()){
		return false;
	}
	_players.erase(iter);
	return true;
}

bool ZoneQueueRequirement::IsMet(const PlayerDataPtr &player) const {
	return ((_class == PlayerDataClass::ANY || player->getPlayerClass() == _class)
		&& (player->getLevel() >= _minLevel)
		&& (player->getLevel() <= _maxLevel));
}


