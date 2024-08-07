#pragma once

#include <memory>
#include <string>

enum class PlayerDataClass : unsigned int { NINJA, CLERIC, FIGHTER, ELF, BERSERKER, WIZARD, MONK, ROBOT, ANY, MAX };
typedef unsigned __int64 PlayerIdType;
class PlayerData;
typedef std::shared_ptr<PlayerData> PlayerDataPtr;
extern const char* PlayerClassString[];

typedef enum class PlayerDataClass PlayerClassEnum;

#define MAX_LEVEL (100)
#define MAX_CLASS (static_cast<int>(PlayerDataClass::ANY)-1)


class PlayerData {
public:
	PlayerData(std::string name, PlayerIdType playerID, PlayerClassEnum clazz, unsigned int level) :
		_name(name), _playerID(playerID), _class(clazz), _level(level)
	{};

	static std::string getClassName(PlayerClassEnum clazz) {
		return PlayerClassString[static_cast<int>(clazz)];
	}
	void dump() const;
	std::string getName() const { return _name; }
	unsigned getLevel() const { return _level; }
	unsigned setLevel(unsigned level) { _level = level;  return _level; }
	PlayerClassEnum getPlayerClass() const { return _class; }
	PlayerIdType    getId() const { return _playerID; }
private:
	const std::string     _name;
	const PlayerIdType    _playerID;
	const PlayerClassEnum  _class;
	unsigned int          _level;
};


extern bool CheckPlayerClass();
