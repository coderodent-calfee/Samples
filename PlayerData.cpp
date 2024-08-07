
#include <iostream>

#include "PlayerData.h"


const char* PlayerClassString[static_cast<int>(PlayerDataClass::MAX)] = { "NINJA", "CLERIC", "FIGHTER", "ELF", "BERSERKER", "WIZARD", "MONK", "ROBOT", "ANY" };



void PlayerData::dump() const {
	std::cout << "( \"" << _name << "\", " << _playerID << 
		", " << getClassName(_class) << ", " << _level << "),\n";
}


bool CheckPlayerClass() {
	size_t playerClassSizeString = sizeof(PlayerClassString);
	size_t playerClassString0Size = sizeof(PlayerClassString[0]);
	size_t playerClassStringSize = playerClassSizeString / playerClassString0Size;
	size_t classEnumSize = static_cast<int>(PlayerDataClass::MAX);
	if (playerClassStringSize != classEnumSize) {
		std::cout << "playerClassStringSize =" << playerClassStringSize << " " << "playerClassStringSize =" << playerClassStringSize << std::endl;
	}
	return (playerClassStringSize == classEnumSize);
}