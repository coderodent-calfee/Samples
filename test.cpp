#include "SimpleZoneQueueManager.h"

#include "test.h"

#include <deque>
#include <set>
#include <iostream>
#include <cassert>

#include "CycleCheck.h"
#include "Tasks.h"
#include <iterator> //for std::ostream_iterator
#include <algorithm> //for std::copy

// =========================Island Explorer=========================================
// Rasoul Kabbirzadeh

#include <cassert>
#include <iostream>
#include <vector>
using namespace std;


// Given a non-empty 2D array grid of 0's and 1's, an island is a group of 1's connected 8-directionally (horizontal or vertical or diagonal.)
// Find the number of islands in the grid.

// ex. 01000
//     01000001
//     000001

// edges are water

// use tree? 
// start visitor when we hit a 1

#define UNEXPLORED_SEA 0
#define EXPLORED_SEA (-2)
#define UNEXPLORED_LAND 1
#define FIRST_ISLAND 2
#define OFF_MAP (-1)


void printMap(vector<vector<int>>& m) {
	for (auto row : m) {
		for (auto terrain : row) {
			char t;
			if (terrain == EXPLORED_SEA) { t = '~'; }
			else if (terrain == UNEXPLORED_SEA) { t = '.'; }
			else if (terrain == UNEXPLORED_LAND) { t = '*'; }
			else { 
				t = 'A' + terrain - FIRST_ISLAND;
				if (t > 'Z') {
					t += 'a' - ('Z' + 1);
				}
			}
			cout << t;
		}
		cout << std::endl;
	}
	cout << std::endl;
}

bool offMap(vector<vector<int>>& m, int x, int y) {
	if (x < 0) { return true; }
	if (y < 0) { return true; }
	if ((size_t)y >= m.size()) { return true; }
	if ((size_t)x >= m[y].size()) { return true; }
	return false;
}

int getTerrain(vector<vector<int>>& m, int x, int y) {
	if (offMap(m,x,y)) { return OFF_MAP; }
	return m[y][x];
}
bool setTerrain(vector<vector<int>>& m, int x, int y, int terrain) {
	if (offMap(m, x, y)) { return false; }
	m[y][x] = terrain;
	return true;
}

class Explorer {
public:
	virtual vector<std::pair<int,int>> explore(vector<vector<int>> &, int x, int y) = 0;
	virtual int getId() = 0;
	virtual void setId(int) = 0;
	virtual vector<std::pair<int, int>> exploreNoRecurse(vector<vector<int>>&, int x, int y) = 0;
};

int directions[] = { -1, 0, 1 };


class Sailor : public Explorer {
public:
	Sailor() {}
	int getId() override { return EXPLORED_SEA; }
	void setId(int id) override {}
	virtual void doExplore(vector<vector<int>>& m, int x, int y) {
		auto terrain = getTerrain(m, x, y);
		if (terrain == UNEXPLORED_SEA) {
			explore(m, x, y);
		}
		else if (terrain == UNEXPLORED_LAND) {
			unexploredCoast.push_back(make_pair<>(x, y));
		}
	}
	vector<std::pair<int, int>> explore(vector<vector<int>>& m, int x, int y) override {
		setTerrain(m, x, y, getId());
		//		cout << x << "/" << y << ":" <<  ((getId()== EXPLORED_SEA)?"Sailing":"Walking") << std::endl;
		//		printMap(m);

		for (int dX : directions) {
			for (int dY : directions) {
				if (dX || dY) {
					if (offMap(m, x + dX, y + dY)) { continue; }
					doExplore(m, x + dX, y + dY);
				}
			}
		}
		return unexploredCoast;
	}
	// NO RECURSION: SAILOR
	vector<std::pair<int, int>> exploreNoRecurse(vector<vector<int>>& m, int x, int y) override {

		vector<std::pair<int, int>> unexploredSea;
		unexploredSea.push_back(make_pair<>(x, y));
		while (unexploredSea.size()) {
			auto next = unexploredSea.back();
			unexploredSea.pop_back();
         if (!offMap(m, next.first, next.second) && getTerrain(m, next.first, next.second) == UNEXPLORED_SEA) {
				setTerrain(m, next.first, next.second, getId());
			}
			for (int dX : directions) {
				for (int dY : directions) {
					if (dX || dY) {
						if (offMap(m, next.first + dX, next.second + dY)) { continue; }
						auto terrain = getTerrain(m, next.first + dX, next.second + dY);
						if (terrain == UNEXPLORED_SEA) {
							unexploredSea.push_back(make_pair<>(next.first + dX, next.second + dY));
						}
						else if (terrain == UNEXPLORED_LAND) {
							unexploredCoast.push_back(make_pair<>(next.first + dX, next.second + dY));
						}
					}
				}
			}
		}
		return unexploredCoast;
	}


	vector<std::pair<int, int>> unexploredCoast;
};

class Walker : public Sailor {
public:
	Walker(int id) : islandId(id) {}
	int islandId;
	void setId(int id) override { islandId = id; }
	int getId() override { return islandId; }
	void doExplore(vector<vector<int>>& m, int x, int y) override {
		auto terrain = getTerrain(m, x, y);
		if (terrain == UNEXPLORED_LAND) {
			explore(m, x, y);
		}
		else if (terrain == UNEXPLORED_SEA) {
			unexploredCoast.push_back(make_pair<>(x, y));
		}
	}
	// NO RECURSION: WALKER
	vector<std::pair<int, int>> exploreNoRecurse(vector<vector<int>>& m, int x, int y) override {

		vector<std::pair<int, int>> unexploredTerrain;
		unexploredTerrain.push_back(make_pair<>(x, y));
		while (unexploredTerrain.size()) {
			auto next = unexploredTerrain.back();
			unexploredTerrain.pop_back();
			if (!offMap(m, next.first, next.second) && getTerrain(m, next.first, next.second) == UNEXPLORED_LAND) {
				setTerrain(m, next.first, next.second, getId());
			}

			for (int dX : directions) {
				for (int dY : directions) {
					if (dX || dY) {
						if (offMap(m, next.first + dX, next.second + dY)) { continue; }
						auto terrain = getTerrain(m, next.first + dX, next.second + dY);
						if (terrain == UNEXPLORED_LAND) {
							unexploredTerrain.push_back(make_pair<>(next.first + dX, next.second + dY));
						}
						else if (terrain == UNEXPLORED_SEA) {
							unexploredCoast.push_back(make_pair<>(next.first + dX, next.second + dY));
						}
					}
				}
			}
		}
		return unexploredCoast;
	}

};


vector<vector<int>> makeMap(int minWidth, int maxWidth, int height, unsigned percentLand) {
	static int madeMapCount = 0;
	cout << "vector<vector<int>> madeMap" << madeMapCount++ << " = {";
	vector<vector<int>> m;
	for (int y = 0; y < height; y++) {
		if (y)cout << ',';
		cout << std::endl;
		cout << " vector<int>({";
		int width = roll(maxWidth - minWidth) + minWidth;
		vector<int> row;
		for (int x = 0; x < width; x++) {
			auto r = roll(99);
			if(r == 0) { 
				cout << "";  
			}
			else if(r == percentLand) { 
				cout << "";  
			}
			int terrain = (int)(r < percentLand);
			if(x)cout << ',';
			cout << terrain;
			row.push_back(terrain);
		}
		m.push_back(row);
		cout << "})";
	}
	cout << std::endl << "};" << std::endl << std::endl;
	return m;
}


// TODO: possible improvement; remove recursion
// for walker, place unexplored land into a list (a set, really)
// then instead of recursion, just select the next from the set
int exploreMap(vector<vector<int>>& m) {
	cout << "Begin:" << std::endl;
	printMap(m);
	int islandCount = 0;
	Walker walker(islandCount);
	Sailor sailor;
	Explorer* explorer;

	vector<std::pair<int, int>> unexploredCoast;
	int x = 0;
	int y = 0;
	unexploredCoast.push_back(std::make_pair<>(x, y));
	while (unexploredCoast.size()) {
		auto next = unexploredCoast.back();
		unexploredCoast.pop_back();
		x = next.first;
		y = next.second;
		auto terrain = getTerrain(m, x, y);
		if (terrain == UNEXPLORED_LAND) {
			explorer = &walker;
			explorer->setId(islandCount++ + FIRST_ISLAND);
		}
		else if (terrain == UNEXPLORED_SEA) {
			explorer = &sailor;
		}
		else { continue; } // already explored
		auto moreCoast = explorer->explore(m, x, y);
		unexploredCoast.insert(unexploredCoast.end(), moreCoast.begin(), moreCoast.end());
	}
	cout << "End:" << std::endl;
	printMap(m);
	cout << "There are: " << islandCount << " islands" << std::endl << std::endl;
	return islandCount;
}

int exploreMapNoRecurse(vector<vector<int>>& m) {
	cout << "Begin:" << std::endl;
	printMap(m);
	int islandCount = 0;
	Walker walker(islandCount);
	Sailor sailor;
	Explorer* explorer;

	vector<std::pair<int, int>> unexploredCoast;
	int x = 0;
	int y = 0;
	unexploredCoast.push_back(std::make_pair<>(x, y));
	while (unexploredCoast.size()) {
		auto next = unexploredCoast.back();
		unexploredCoast.pop_back();
		x = next.first;
		y = next.second;
		auto terrain = getTerrain(m, x, y);
		if (terrain == UNEXPLORED_LAND) {
			explorer = &walker;
			explorer->setId(islandCount++ + FIRST_ISLAND);
		}
		else if (terrain == UNEXPLORED_SEA) {
			explorer = &sailor;
		}
		else { continue; } // already explored
		auto moreCoast = explorer->exploreNoRecurse(m, x, y);
		unexploredCoast.insert(unexploredCoast.end(), moreCoast.begin(), moreCoast.end());
	}
	cout << "End:" << std::endl;
	printMap(m);
	cout << "There are: " << islandCount << " islands" << std::endl << std::endl;
	return islandCount;
}

void testIslandExplorer() {
	vector<vector<int>> terrainMap = {
		 vector<int>({0,1,0,0,0}),
		 vector<int>({0,0,1,0,0})
	};
	int islandCount = 0;
	islandCount = exploreMap(terrainMap);
	assert(islandCount == 1);

	vector<vector<int>> barrierIsland = {
		 vector<int>({1,0,1,0,0,1,0,0,0,0}),
		 vector<int>({1,0,0,0,0,1,1,0,1,1}),
		 vector<int>({0,0,1,1,1,0,0,0,0,0}),
		 vector<int>({1,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,1,0,0,0,0})
	};

	islandCount = exploreMap(barrierIsland);
	assert(islandCount == 5);

	vector<vector<int>> ringIslandMap = {
		 vector<int>({0,0,0,0,0,1,1,1,0,1,1,1,0,1,1,0,0,0,0,0}),
		 vector<int>({0,0,0,1,1,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0}),
		 vector<int>({0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,1,0,0}),
		 vector<int>({1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0}),
		 vector<int>({0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0}),
		 vector<int>({1,0,0,0,1,0,0,0,1,1,0,1,0,0,1,1,0,1,1,0}),
		 vector<int>({0,0,0,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,1}),
		 vector<int>({1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0}),
		 vector<int>({1,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0,0,1,0,0}),
		 vector<int>({0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,0,0})
	};
	islandCount = exploreMap(ringIslandMap);
	assert(islandCount == 10);

	vector<vector<int>> madeMap0 = {
		 vector<int>({1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0}),
		 vector<int>({0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0})
	};
	islandCount = exploreMap(madeMap0);
	assert(islandCount == 34);

	vector<vector<int>> allLand = {
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1})
	};
	islandCount = exploreMap(allLand);
	assert(islandCount == 1);


	vector<vector<int>> allSea = {
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0})
	};
	islandCount = exploreMap(allSea);
	assert(islandCount == 0);

	//int minWidth, maxWidth, height;
	//unsigned percentLand;

	//minWidth = 50;
	//maxWidth = 100;
	//height = 50;
	//percentLand = 50;
	//auto madeMap = makeMap( minWidth,  maxWidth,  height,  percentLand);
	//islandCount = exploreMap(madeMap);
}
void testIslandExplorerNoRecurse() {
	vector<vector<int>> terrainMap = {
		 vector<int>({0,1,0,0,0}),
		 vector<int>({0,0,1,0,0})
	};
	int islandCount = 0;
	islandCount = exploreMapNoRecurse(terrainMap);
	assert(islandCount == 1);

	vector<vector<int>> barrierIsland = {
		 vector<int>({1,0,1,0,0,1,0,0,0,0}),
		 vector<int>({1,0,0,0,0,1,1,0,1,1}),
		 vector<int>({0,0,1,1,1,0,0,0,0,0}),
		 vector<int>({1,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,1,0,0,0,0})
	};

	islandCount = exploreMapNoRecurse(barrierIsland);
	assert(islandCount == 5);

	vector<vector<int>> ringIslandMap = {
		 vector<int>({0,0,0,0,0,1,1,1,0,1,1,1,0,1,1,0,0,0,0,0}),
		 vector<int>({0,0,0,1,1,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0}),
		 vector<int>({0,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,0,1,0,0}),
		 vector<int>({1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,1,0,0,1,0}),
		 vector<int>({0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,0,0,0}),
		 vector<int>({1,0,0,0,1,0,0,0,1,1,0,1,0,0,1,1,0,1,1,0}),
		 vector<int>({0,0,0,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,1}),
		 vector<int>({1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0}),
		 vector<int>({1,0,1,0,0,0,1,0,1,0,0,1,1,0,0,0,0,1,0,0}),
		 vector<int>({0,0,0,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,0,0})
	};
	islandCount = exploreMapNoRecurse(ringIslandMap);
	assert(islandCount == 10);

	vector<vector<int>> madeMap0 = {
		 vector<int>({1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0}),
		 vector<int>({0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0}),
		 vector<int>({0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,1,0})
	};
	islandCount = exploreMapNoRecurse(madeMap0);
	assert(islandCount == 34);

	vector<vector<int>> allLand = {
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1}),
		 vector<int>({1,1,1,1,1,1,1,1,1,1})
	};
	islandCount = exploreMapNoRecurse(allLand);
	assert(islandCount == 1);


	vector<vector<int>> allSea = {
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0}),
		 vector<int>({0,0,0,0,0,0,0,0,0,0})
	};
	islandCount = exploreMapNoRecurse(allSea);
	assert(islandCount == 0);

	//int minWidth, maxWidth, height;
	//unsigned percentLand;

	//minWidth = 50;
	//maxWidth = 100;
	//height = 50;
	//percentLand = 50;
	//auto madeMap = makeMap( minWidth,  maxWidth,  height,  percentLand);
	//islandCount = exploreMap(madeMap);
}




/// -------------------PriceDivergenceMonitor----------------------
#include "PriceDivergenceMonitor.h"




void testPriceDivergenceMonitor() {
	testPriceDivergenceMonitorRegisterPair();
	testPriceDivergenceMonitorUpdatePrice();
	testPriceDivergenceMonitorReportDivergence();
}

void testPriceDivergenceMonitorRegisterPair() {
	string stockNames[] = { "One", "Two", "Three", "Four"};
	
	PriceDivergenceMonitor pdm(4);
	assert(pdm.getStockPairs()->size() == 0);


	pdm.RegisterPair(stockNames[0], stockNames[1]);
	assert(pdm.getStockPairs()->size() == 1);

	auto set = pdm.getStockPairs();
	assert(set->begin()->first.compare(stockNames[0]) == 0);
	assert(set->begin()->second.compare(stockNames[1]) == 0);

	pdm.RegisterPair(stockNames[2], stockNames[3]);
	assert(pdm.getStockPairs()->size() == 2);

	set = pdm.getStockPairs();
	for (auto i = set->begin(); i != set->end(); i++) {
		auto& pair = *i;
		if (pair.first.compare(stockNames[0]) == 0) {
			assert(pair.second.compare(stockNames[1]) == 0);
		}
		else {
			assert(pair.first.compare(stockNames[3]) == 0);
			assert(pair.second.compare(stockNames[2]) == 0);
		}
	}

	pdm.RegisterPair(stockNames[0], stockNames[1]);
	assert(pdm.getStockPairs()->size() == 2);
	pdm.RegisterPair(stockNames[1], stockNames[0]);
	assert(pdm.getStockPairs()->size() == 2);
	pdm.RegisterPair(stockNames[0], stockNames[2]);
	assert(pdm.getStockPairs()->size() == 3);

	//{
	//	auto outputset = pdm.getStockPairs();
	//	for (auto i = outputset->begin(); i != outputset->end(); i++) {
	//		auto pair = *i;
	//		cout << " got " << pair.first << " " << pair.second << endl;
	//	}
	//}

}

void testPriceDivergenceMonitorUpdatePrice() {
	string stockNames[] = { "One", "Two", "Three", "Four" };

	PriceDivergenceMonitor pdm(4);
	assert(pdm.getStocks()->size() == 0);

	pdm.UpdatePrice(stockNames[0], 10);
	assert(pdm.getStocks()->size() == 1);

	auto set = pdm.getStocks();
	assert(set->begin()->first.compare(stockNames[0]) == 0);
	assert(set->begin()->second == 10);

	pdm.UpdatePrice(stockNames[0], 11);
	assert(pdm.getStocks()->size() == 1);
	set = pdm.getStocks();
	assert(set->begin()->first.compare(stockNames[0]) == 0);
	assert(set->begin()->second == 11);

	pdm.UpdatePrice(stockNames[1], 12);
	assert(pdm.getStocks()->size() == 2);

	set = pdm.getStocks();
	for (auto i = set->begin(); i != set->end(); i++) {
		auto& pair = *i;
		if (pair.first.compare(stockNames[0]) == 0) {
			assert(pair.second == 11);
		}
		else {
			assert(pair.first.compare(stockNames[1]) == 0);
			assert(pair.second == 12);
		}
	}

	//{
	//	auto outputset = pdm.getStocks();
	//	for (auto i = outputset->begin(); i != outputset->end(); i++) {
	//		auto pair = *i;
	//		cout << " got " << pair.first << " " << pair.second << endl;
	//	}
	//}
}

void testPriceDivergenceMonitorReportDivergence() {
	string stockNames[] = { "One", "Two", "Three", "Four" };
	PriceDivergenceMonitor pdm(4);
	pdm.RegisterPair(stockNames[0], stockNames[1]);
	pdm.RegisterPair(stockNames[2], stockNames[3]);

	pdm.UpdatePrice(stockNames[0], 10);
	pdm.UpdatePrice(stockNames[1], 10);
	pdm.UpdatePrice(stockNames[2], 10);
	pdm.UpdatePrice(stockNames[3], 10);


	struct debug_callback : public report_callback {
		int _callBackCount = 0;
		void operator () (const std::string& updatedStockName, int
			updatedStockPrice, const std::string& otherStockName, int otherStockPrice) override
		{
			_callBackCount++;
			cout << "updated " << updatedStockName << "@" << updatedStockPrice << " triggered " <<
				otherStockName << "@" << otherStockPrice << endl;
		}
	} r;

	pdm.SetReportCallback(&r);
	assert(r._callBackCount == 0);

	pdm.UpdatePrice(stockNames[1], 11);
	assert(r._callBackCount == 0);

	pdm.UpdatePrice(stockNames[1], 14);
	assert(r._callBackCount == 0);
	pdm.UpdatePrice(stockNames[1], 15);
	assert(r._callBackCount == 1);
	pdm.UpdatePrice(stockNames[0], 11);
	assert(r._callBackCount == 1);
	pdm.UpdatePrice(stockNames[0], 10);
	assert(r._callBackCount == 2);

	pdm.RegisterPair(stockNames[0], stockNames[2]);
	assert(r._callBackCount == 2);
	pdm.UpdatePrice(stockNames[2], 10);
	assert(r._callBackCount == 2);

	pdm.UpdatePrice(stockNames[2], 5);
	assert(r._callBackCount == 4);

	pdm.UpdatePrice(stockNames[1], 14);
	assert(r._callBackCount == 4);
	pdm.UpdatePrice(stockNames[0], 20);
	assert(r._callBackCount == 6);

	//{
	//	auto outputset = pdm.getStocks();
	//	for (auto i = outputset->begin(); i != outputset->end(); i++) {
	//		auto pair = *i;
	//		cout << " got " << pair.first << " " << pair.second << endl;
	//	}
	//}
	//{
	//	auto outputset = pdm.getStockPairs();
	//	for (auto i = outputset->begin(); i != outputset->end(); i++) {
	//		auto pair = *i;
	//		cout << " got " << pair.first << " " << pair.second << endl;
	//	}
	//}

}


/// -------------------SimpleZoneQueueManager----------------------
void testSimpleZoneQueueManager() {
	testPlayerClassEnum();

	testInitialZones();

	testAddPlayersToZoneQueues();

	testRemovingPlayerFromZone();

	testNinjaZones();

	testCompleteZone();

	testReadyZones();
}

// UTILITY
std::deque<PlayerDataPtr> InitPlayerData();

unsigned roll(unsigned max) {
	if (max == 0) { return 0; }
	if (max == RAND_MAX) {
		return std::rand();
	}
	return std::rand() / ((RAND_MAX + 1U) / max);
}


PlayerIdType rollID()
{
	PlayerIdType id = roll(15);
	id <<= 15;
	id |= roll(RAND_MAX);
	id <<= 15;
	id |= roll(RAND_MAX);
	id <<= 15;
	id |= roll(RAND_MAX);
	id <<= 15;
	id |= roll(RAND_MAX);
	return id;
}

PlayerDataPtr RandomPlayer() {
	char initials[6];
	for (int i = 0; i < 5; i++) {
		initials[i] = 'A' + roll(26);
	}
	initials[5] = 0;
	std::string name(initials);
	PlayerDataPtr player = std::make_shared<PlayerData>(name, rollID(), PlayerClassEnum(roll(MAX_CLASS + 1)), roll(MAX_LEVEL));
	return player;
}


std::deque<PlayerDataPtr> InitRandomPlayerData(int numPlayers) {
	std::deque<PlayerDataPtr> players;
	for (int i = 0; i < numPlayers; i++) {
		auto additionalPlayer = RandomPlayer();
		players.push_back(additionalPlayer);
	}
	return players;
}

PlayerDataPtr MakePlayerData(std::string name, PlayerIdType playerID, PlayerClassEnum clazz, unsigned int level) {
	return std::make_shared<PlayerData>(name, playerID, clazz, level);
}


void CheckZoneData(const ZoneQueuePtr& zone) {
	unsigned count = zone->getCount();
	zone->report();
    auto numberOfPlayers = zone->GetPlayers().size();
	std::cout << "count = " << count << " No. of players in queue:" << zone->GetPlayers().size() << std::endl;
	assert(numberOfPlayers == count);
}

void DisplayPlayerData(const std::deque<PlayerDataPtr> & players) {
	std::cout << "PLAYER DATA:" << std::endl;
	for (auto& player : players) {
		player->dump();
	}
	std::cout << "END PLAYER DATA:" << std::endl << std::endl;
}


void utilMakeZoneQueues(std::shared_ptr<SimpleZoneQueueManager> & zoneQueueMgr) {

	ZoneQueueRequirement ninja(PlayerDataClass::NINJA, 0, 100, 1, 1);
	ZoneQueueRequirement fighter(PlayerDataClass::FIGHTER, 30, 50, 1, 2);
	ZoneQueueRequirement wizard(PlayerDataClass::WIZARD, 30, 50, 1, 1);
	ZoneQueueRequirement cleric(PlayerDataClass::CLERIC, 30, 50, 1, 1);

	zoneQueueMgr->AddZoneQueue("Anyone", RequirementsList({ ZoneQueueRequirement(PlayerDataClass::ANY, 0, 100, 1, 100) }));
	zoneQueueMgr->AddZoneQueue("AnyoneAwesome", RequirementsList({ ZoneQueueRequirement(PlayerDataClass::ANY, 100, 100, 1, 100) }));
	zoneQueueMgr->AddZoneQueue("Newbies", RequirementsList({ ZoneQueueRequirement(PlayerDataClass::ANY, 0, 0, 1, 100) }));
	zoneQueueMgr->AddZoneQueue("FiveAwesomeNinjas", RequirementsList({ ZoneQueueRequirement(PlayerDataClass::NINJA, 100, 100, 5, 5) }));
	zoneQueueMgr->AddZoneQueue("NinjaAndRobot", RequirementsList({ ninja, ZoneQueueRequirement(PlayerDataClass::ROBOT, 0, 100, 1, 1) }));
	zoneQueueMgr->AddZoneQueue("StandardParty", RequirementsList({ ninja, fighter, wizard, cleric }));
}
void utilAddPlayersToAllZones(SimpleZoneQueueMgrPtr& zoneQueueMgr, std::deque<PlayerDataPtr>& players) {
	auto allZones = zoneQueueMgr->GetAllZones();
	for (auto& player : players) {
		for (auto& zone : allZones) {
			bool added = zoneQueueMgr->AddPlayerToZone(player, zone);
		}
	}
}

// TESTS

void testPlayerClassEnum() {
	assert(CheckPlayerClass());
}


void testInitialZones() {
	auto zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();
	utilMakeZoneQueues(zoneQueueMgr);

	std::cout << "INITIAL ZONES:" << std::endl;
	CheckZoneData(zoneQueueMgr->GetZonesByName("Anyone").front());
	CheckZoneData(zoneQueueMgr->GetZonesByName("AnyoneAwesome").front());
	CheckZoneData(zoneQueueMgr->GetZonesByName("Newbies").front());
	CheckZoneData(zoneQueueMgr->GetZonesByName("FiveAwesomeNinjas").front());
	CheckZoneData(zoneQueueMgr->GetZonesByName("NinjaAndRobot").front());
	CheckZoneData(zoneQueueMgr->GetZonesByName("StandardParty").front());
}


void testAddPlayersToZoneQueues() {
	auto zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();
	std::deque<PlayerDataPtr> players = InitPlayerData();

	utilMakeZoneQueues(zoneQueueMgr);
	utilAddPlayersToAllZones(zoneQueueMgr, players);
	auto anyoneZone = zoneQueueMgr->GetZonesByName("Anyone").front();
	auto anyone100Zone = zoneQueueMgr->GetZonesByName("AnyoneAwesome").front();
	auto anyone0Zone = zoneQueueMgr->GetZonesByName("Newbies").front();
	auto fiveNinjaZone = zoneQueueMgr->GetZonesByName("FiveAwesomeNinjas").front();
	auto ninjaAndRobotZone = zoneQueueMgr->GetZonesByName("NinjaAndRobot").front();
	auto standardPartyZone = zoneQueueMgr->GetZonesByName("StandardParty").front();

	std::cout << std::endl << "POPULATED ZONES:" << std::endl;
	CheckZoneData(anyoneZone);
	CheckZoneData(anyone100Zone);
	CheckZoneData(anyone0Zone);
	CheckZoneData(fiveNinjaZone);
	CheckZoneData(ninjaAndRobotZone);
	CheckZoneData(standardPartyZone);

	assert(anyoneZone->IsFull());
	assert(anyone100Zone->IsReady());
	assert(fiveNinjaZone->getCount() == 0);
	assert(standardPartyZone->IsReady());
	assert(standardPartyZone->IsFull());


}


void testRemovingPlayerFromZone() {
	auto zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();
	std::deque<PlayerDataPtr> players = InitPlayerData();

	utilMakeZoneQueues(zoneQueueMgr);
	utilAddPlayersToAllZones(zoneQueueMgr, players);
	
	auto ninjaAndRobotZone = zoneQueueMgr->GetZonesByName("NinjaAndRobot").front();

	auto playerList = ninjaAndRobotZone->GetPlayers();

	for (auto player : playerList) {
		bool removed = zoneQueueMgr->RemovePlayerFromZone(player, ninjaAndRobotZone);
		assert(removed);
		assert(!ninjaAndRobotZone->IsReady());
		assert(!ninjaAndRobotZone->IsFull());

		removed = zoneQueueMgr->RemovePlayerFromZone(player, ninjaAndRobotZone);
		assert(removed == false);
		break;
	}
	assert(ninjaAndRobotZone->GetPlayers().size() == 1);
	CheckZoneData(ninjaAndRobotZone);

}

void testNinjaZones() {
	auto zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();
	std::deque<PlayerDataPtr> players = InitPlayerData();

	utilMakeZoneQueues(zoneQueueMgr);
	utilAddPlayersToAllZones(zoneQueueMgr, players);

	auto anyone100Zone = zoneQueueMgr->GetZonesByName("AnyoneAwesome").front();
	auto fiveNinjaZone = zoneQueueMgr->GetZonesByName("FiveAwesomeNinjas").front();
	auto ninjaAndRobotZone = zoneQueueMgr->GetZonesByName("NinjaAndRobot").front();

	for (int i = 0; i < 5; i++) {
		std::string name = "ninja ";
		name.push_back('1' + i);
		auto ninja = MakePlayerData(name, 1000 + i, PlayerDataClass::NINJA, 100);
		zoneQueueMgr->AddPlayerToZone(ninja, anyone100Zone);
		zoneQueueMgr->AddPlayerToZone(ninja, fiveNinjaZone);
		// as a test: add them again; should not change and should return false
		bool added = zoneQueueMgr->AddPlayerToZone(ninja, fiveNinjaZone);
		assert(added == false);
		added = zoneQueueMgr->AddPlayerToZone(ninja, ninjaAndRobotZone);
		assert(added == false);
	}
	assert(ninjaAndRobotZone->GetPlayers().size() == 2);
	assert(fiveNinjaZone->GetPlayers().size() == 5);
	assert(anyone100Zone->GetPlayers().size() == 11);

	CheckZoneData(fiveNinjaZone);
	CheckZoneData(ninjaAndRobotZone);
	CheckZoneData(anyone100Zone);
	assert(anyone100Zone->IsComplete() == 0);
	assert(anyone100Zone->IsFull() == 0);
}

void testCompleteZone() {

	auto zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();
	std::deque<PlayerDataPtr> players = InitPlayerData();

	utilMakeZoneQueues(zoneQueueMgr);
	utilAddPlayersToAllZones(zoneQueueMgr, players);
	auto anyone100Zone = zoneQueueMgr->GetZonesByName("AnyoneAwesome").front();

	assert(anyone100Zone->GetPlayers().size() == 6);

	auto awesomeWizard = MakePlayerData(std::string("Awesome Wizard"), 10000, PlayerDataClass::WIZARD, 100);
	bool awesomeWizardAdded = zoneQueueMgr->AddPlayerToZone(awesomeWizard, anyone100Zone);
	assert(awesomeWizardAdded);
	assert(anyone100Zone->GetPlayers().size() == 7);
	bool awesomeWizardRemoved = zoneQueueMgr->RemovePlayerFromZone(awesomeWizard, anyone100Zone);
	assert(awesomeWizardRemoved);
	anyone100Zone->Complete(); // locking the zone for transport
	assert(anyone100Zone->IsComplete());
	assert(anyone100Zone->IsFull() == 0);
	assert(anyone100Zone->GetPlayers().size() == 6);
	// got in before but not anymore
	awesomeWizardAdded = zoneQueueMgr->AddPlayerToZone(awesomeWizard, anyone100Zone);
	assert(awesomeWizardAdded == false);
	assert(anyone100Zone->GetPlayers().size() == 6);
}

void testReadyZones() {

	// TODO: add players - done
	// check for ready zones; complete them
	// make new zones to replace them
	// check lists of zones returned for names

	int numPlayers = 100;
	auto zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();
	utilMakeZoneQueues(zoneQueueMgr);

	zoneQueueMgr->AddZoneQueue("Anyone", RequirementsList({ ZoneQueueRequirement(PlayerDataClass::ANY, 0, 100, 1, 100) }));
	auto anyoneZone = zoneQueueMgr->GetZonesByName("Anyone").front();

	auto anyone100Zone = zoneQueueMgr->GetZonesByName("AnyoneAwesome").front();
	auto anyone0Zone = zoneQueueMgr->GetZonesByName("Newbies").front();
	auto fiveNinjaZone = zoneQueueMgr->GetZonesByName("FiveAwesomeNinjas").front();
	auto ninjaAndRobotZone = zoneQueueMgr->GetZonesByName("NinjaAndRobot").front();
	auto standardPartyZone = zoneQueueMgr->GetZonesByName("StandardParty").front();


	std::deque<PlayerDataPtr> players = InitRandomPlayerData(numPlayers);


	utilAddPlayersToAllZones(zoneQueueMgr, players);
	CheckZoneData(anyoneZone);
	CheckZoneData(anyone100Zone);
	CheckZoneData(anyone0Zone);
	CheckZoneData(fiveNinjaZone);
	CheckZoneData(ninjaAndRobotZone);
	CheckZoneData(standardPartyZone);

	std::cout << "players:" << players.size() << std::endl << std::endl;

	players = InitRandomPlayerData(numPlayers);
	utilAddPlayersToAllZones(zoneQueueMgr, players);
	CheckZoneData(anyoneZone);
	CheckZoneData(anyone100Zone);
	CheckZoneData(anyone0Zone);
	CheckZoneData(fiveNinjaZone);
	CheckZoneData(ninjaAndRobotZone);
	CheckZoneData(standardPartyZone);
	std::cout << "players:" << players.size() << std::endl << std::endl;

}



// initial player data

std::deque<PlayerDataPtr> InitPlayerData() {

#define	BERSERKER	 PlayerDataClass::BERSERKER	
#define	CLERIC		 PlayerDataClass::CLERIC		
#define	ELF		     PlayerDataClass::ELF		
#define	FIGHTER	     PlayerDataClass::FIGHTER	
#define	MONK		 PlayerDataClass::MONK		
#define	NINJA		 PlayerDataClass::NINJA		
#define	ROBOT		 PlayerDataClass::ROBOT		
#define	WIZARD		 PlayerDataClass::WIZARD		

	std::deque<PlayerDataPtr> players({
		MakePlayerData("AOF", 8474328278879526800, BERSERKER, 81),
		MakePlayerData("EWS", 105390017189090620, FIGHTER, 51),
		MakePlayerData("ZLD", 6377574565991894455, NINJA, 0),
		MakePlayerData("ERL", 11279399058760115878, NINJA, 35),
		MakePlayerData("WSY", 2838577867559705300, BERSERKER, 93),
		MakePlayerData("UVZ", 6071531260986295050, BERSERKER, 101),
		MakePlayerData("JCR", 15306055885602707576, NINJA, 5),
		MakePlayerData("VSM", 8598460615056723420, WIZARD, 20),
		MakePlayerData("PKT", 5939341035203585732, BERSERKER, 61),
		MakePlayerData("NZT", 10943397487071248541, CLERIC, 34),
		MakePlayerData("DYD", 5103460464199522162, WIZARD, 91),
		MakePlayerData("DWV", 3247991662184808576, CLERIC, 58),
		MakePlayerData("TKH", 13514439595023904532, WIZARD, 57),
		MakePlayerData("VAN", 2247434185103617097, ELF, 67),
		MakePlayerData("MJW", 5744949937402988700, ELF, 32),
		MakePlayerData("FTV", 15019680489703540343, BERSERKER, 40),
		MakePlayerData("NFV", 3276760096113778462, WIZARD, 63),
		MakePlayerData("IYH", 12491462164334927780, CLERIC, 34),
		MakePlayerData("GDA", 14077834290623119827, MONK, 6),
		MakePlayerData("CLT", 1656416974895228202, BERSERKER, 69),
		MakePlayerData("LGP", 2887087535493183071, WIZARD, 53),
		MakePlayerData("EIE", 16253337129022306311, ELF, 100),
		MakePlayerData("YBX", 13155654936944594260, CLERIC, 29),
		MakePlayerData("LMP", 14555377885221564420, MONK, 64),
		MakePlayerData("ETO", 3003045918584483063, CLERIC, 91),
		MakePlayerData("MCT", 12456750709995335441, BERSERKER, 61),
		MakePlayerData("XPS", 16504094179883914819, WIZARD, 4),
		MakePlayerData("CWW", 4703168081638582517, CLERIC, 75),
		MakePlayerData("OLK", 8246454127785879597, ELF, 30),
		MakePlayerData("VYW", 16287587025416930085, FIGHTER, 73),
		MakePlayerData("PYB", 4732526921777718568, WIZARD, 88),
		MakePlayerData("XOR", 6337401829807599857, ELF, 11),
		MakePlayerData("RNP", 11645239843285319401, NINJA, 39),
		MakePlayerData("TMJ", 6866463514874949607, BERSERKER, 79),
		MakePlayerData("DTY", 4676166277757987948, NINJA, 61),
		MakePlayerData("WPS", 1969079913020164209, NINJA, 86),
		MakePlayerData("EQI", 11301389441646644792, FIGHTER, 32),
		MakePlayerData("UXD", 3616209483010422466, ELF, 63),
		MakePlayerData("OHO", 2084502864204489952, FIGHTER, 22),
		MakePlayerData("XUT", 14098513833030000625, FIGHTER, 62),
		MakePlayerData("OZB", 7026680056148565451, BERSERKER, 8),
		MakePlayerData("DDY", 387869516153141093, NINJA, 8),
		MakePlayerData("YVN", 6063393363848297014, WIZARD, 85),
		MakePlayerData("KGP", 16643013375260227697, WIZARD, 25),
		MakePlayerData("PUO", 5769548039035540600, CLERIC, 14),
		MakePlayerData("WBV", 14446269905674393665, WIZARD, 66),
		MakePlayerData("SUA", 11627750709703346054, WIZARD, 87),
		MakePlayerData("HDZ", 15411729389406000117, FIGHTER, 62),
		MakePlayerData("GNX", 7009915521340472504, ROBOT, 10),
		MakePlayerData("UWF", 12813634788395712269, NINJA, 46),
		MakePlayerData("BGE", 11397060400450495435, CLERIC, 25),
		MakePlayerData("QRX", 10550126728782755333, ROBOT, 46),
		MakePlayerData("YOP", 6168419850841823527, FIGHTER, 54),
		MakePlayerData("BKP", 3048706642568599874, CLERIC, 7),
		MakePlayerData("LRK", 3400494830375272036, BERSERKER, 84),
		MakePlayerData("EXO", 11586434882366551085, WIZARD, 44),
		MakePlayerData("XSS", 6806673929283225864, ELF, 90),
		MakePlayerData("JTT", 2536626897136940191, WIZARD, 74),
		MakePlayerData("JNC", 8825417033492597651, CLERIC, 91),
		MakePlayerData("CBA", 11693715877728343678, FIGHTER, 35),
		MakePlayerData("SFS", 9336720087172611262, WIZARD, 62),
		MakePlayerData("MZS", 11958231768029652902, WIZARD, 53),
		MakePlayerData("EBI", 5170449354164977921, CLERIC, 79),
		MakePlayerData("PTK", 16859541767830915083, MONK, 35),
		MakePlayerData("WKH", 10827287477912123988, NINJA, 28),
		MakePlayerData("WYK", 14844342267020610077, BERSERKER, 73),
		MakePlayerData("UMP", 14610674441804816951, BERSERKER, 59),
		MakePlayerData("IGU", 1989452079650183021, MONK, 37),
		MakePlayerData("NXU", 15907345562804089334, CLERIC, 97),
		MakePlayerData("QEQ", 12437822285684948367, FIGHTER, 26),
		MakePlayerData("CUW", 9768686247026875687, FIGHTER, 12),
		MakePlayerData("ZAQ", 1469956503957980992, CLERIC, 69),
		MakePlayerData("JUG", 4578270777752117877, CLERIC, 64),
		MakePlayerData("DBA", 6242566688014714312, WIZARD, 35),
		MakePlayerData("SIR", 9285629578806802081, BERSERKER, 88),
		MakePlayerData("WMC", 13678517524986130083, WIZARD, 57),
		MakePlayerData("QTV", 16040045834396208583, MONK, 77),
		MakePlayerData("JVL", 11268114062866624861, NINJA, 17),
		MakePlayerData("RVE", 5786034046122493662, NINJA, 14),
		MakePlayerData("XQL", 6518636126187973051, CLERIC, 2),
		MakePlayerData("DQO", 3805463651308813700, ELF, 99),
		MakePlayerData("PCZ", 7737993356116087329, CLERIC, 76),
		MakePlayerData("QYR", 5243763142177441273, BERSERKER, 29),
		MakePlayerData("MYR", 7838435829822921265, BERSERKER, 77),
		MakePlayerData("QXP", 8588958730269169514, BERSERKER, 92),
		MakePlayerData("GFS", 739400794446185905, WIZARD, 95),
		MakePlayerData("LBZ", 199237144454539377, BERSERKER, 12),
		MakePlayerData("QKL", 2879471688861333424, WIZARD, 22),
		MakePlayerData("AIJ", 13709542850845426138, MONK, 13),
		MakePlayerData("TTY", 2195920740243148265, FIGHTER, 47),
		MakePlayerData("WTB", 1342994916270918569, ROBOT, 42),
		MakePlayerData("DQP", 15760111108515704936, WIZARD, 69),
		MakePlayerData("ZUF", 2975654784384800858, ELF, 17),
		MakePlayerData("JHD", 13559560235309152620, NINJA, 20),
		MakePlayerData("LUL", 4103697318300250728, WIZARD, 50),
		MakePlayerData("WBM", 12977013603242580365, WIZARD, 63),
		MakePlayerData("OBN", 843635159642304529, WIZARD, 79),
		MakePlayerData("UHK", 10377305745806138400, NINJA, 48),
		MakePlayerData("EBR", 12449407279501168985, FIGHTER, 3),
		MakePlayerData("HMW", 2470749275307519961, NINJA, 8),
		MakePlayerData("VBV", 3769309100747156558, ROBOT, 24),
		MakePlayerData("WXV", 2517700636020086704, NINJA, 61),
		MakePlayerData("XQO", 12681361415236479883, BERSERKER, 92),
		MakePlayerData("DJH", 13508301879012517894, ROBOT, 17),
		MakePlayerData("TVD", 11431715268394443521, ELF, 72),
		MakePlayerData("LAF", 15566383998304875528, CLERIC, 42),
		MakePlayerData("FAU", 14817347023115517682, BERSERKER, 23),
		MakePlayerData("WQF", 5804948562134863701, FIGHTER, 82),
		MakePlayerData("IXF", 14865694359377367359, CLERIC, 95),
		MakePlayerData("OQL", 2614138750675994589, MONK, 1),
		MakePlayerData("YPU", 12838689995200463077, MONK, 42),
		MakePlayerData("TIX", 727437744072883210, MONK, 46),
		MakePlayerData("NSG", 9527324346372186051, WIZARD, 83),
		MakePlayerData("JAM", 5796459542308122914, ELF, 63),
		MakePlayerData("LYJ", 17138393403907025566, NINJA, 53),
		MakePlayerData("PMO", 4539447560056851949, ELF, 3),
		MakePlayerData("PBK", 2161457052206266216, ELF, 30),
		MakePlayerData("PFH", 3806270956473032084, MONK, 56),
		MakePlayerData("ELX", 16209215716144027069, NINJA, 58),
		MakePlayerData("DGM", 1542211239868808906, NINJA, 97),
		MakePlayerData("KYP", 8014315568283739567, ELF, 29),
		MakePlayerData("UPW", 5699260248597685039, ROBOT, 32),
		MakePlayerData("MEA", 13991980863980464325, ELF, 4),
		MakePlayerData("UUR", 15958865660091269874, MONK, 30),
		MakePlayerData("BAJ", 2649319257362141503, ELF, 72),
		MakePlayerData("BNJ", 15139941891780176054, CLERIC, 81),
		MakePlayerData("GOJ", 16063106491627574173, WIZARD, 54),
		MakePlayerData("OUX", 14282728130115353016, MONK, 7),
		MakePlayerData("HYZ", 4763830208273759841, BERSERKER, 73),
		MakePlayerData("JFM", 12662019447324548655, ROBOT, 69),
		MakePlayerData("XMI", 6165996794033609745, BERSERKER, 54),
		MakePlayerData("XTU", 5495801187719540213, WIZARD, 65),
		MakePlayerData("IBM", 10999535577453650816, MONK, 2),
		MakePlayerData("QST", 7824231050607723877, CLERIC, 87),
		MakePlayerData("NHB", 8780086631899429454, ROBOT, 24),
		MakePlayerData("WIG", 16339730375915251332, NINJA, 29),
		MakePlayerData("UQC", 4107359501310954927, CLERIC, 26),
		MakePlayerData("LTI", 12536000639401265765, FIGHTER, 5),
		MakePlayerData("UZQ", 9447355005717775722, BERSERKER, 62),
		MakePlayerData("GGK", 15425452456296253127, FIGHTER, 80),
		MakePlayerData("PJJ", 4491508411766560034, ROBOT, 67),
		MakePlayerData("FHJ", 3573610059955301377, FIGHTER, 53),
		MakePlayerData("RVA", 16475983155548745689, BERSERKER, 49),
		MakePlayerData("XVP", 2931445388319784736, MONK, 30),
		MakePlayerData("CKA", 2569450975512125875, FIGHTER, 69),
		MakePlayerData("BMB", 10333107325194974288, NINJA, 97),
		MakePlayerData("KKR", 4614276159711630329, ROBOT, 83),
		MakePlayerData("KYP", 6483030474696396153, ELF, 20),
		MakePlayerData("KHI", 4839595064395399007, MONK, 83),
		MakePlayerData("LJK", 15376991819256414507, BERSERKER, 38),
		MakePlayerData("KPU", 3559473349865394311, CLERIC, 8),
		MakePlayerData("ICP", 14341290603565026749, FIGHTER, 14),
		MakePlayerData("NGA", 2270319792019391175, MONK, 58),
		MakePlayerData("FHM", 13652429670436297905, ELF, 31),
		MakePlayerData("TCC", 10790922895049274664, MONK, 76),
		MakePlayerData("EIX", 2699597731012090460, FIGHTER, 23),
		MakePlayerData("EIB", 14349802356822588847, ROBOT, 50),
		MakePlayerData("MSG", 5559898500009846212, FIGHTER, 94),
		MakePlayerData("AGH", 8886138844361146081, MONK, 96),
		MakePlayerData("CVB", 10970150556437445835, ELF, 28),
		MakePlayerData("BAX", 2423127409544086402, MONK, 41),
		MakePlayerData("VVH", 3425076827475904651, MONK, 50),
		MakePlayerData("FFF", 10075125674424820895, ROBOT, 20),
		MakePlayerData("XYV", 1041955214382509569, NINJA, 26),
		MakePlayerData("YJI", 6264143790002945485, ROBOT, 69),
		MakePlayerData("NUU", 14479575192209721070, ELF, 75),
		MakePlayerData("XTJ", 4669524960290321328, CLERIC, 28),
		MakePlayerData("DWP", 7795781386093979387, CLERIC, 79),
		MakePlayerData("EIZ", 13889619980893196479, CLERIC, 71),
		MakePlayerData("COO", 1504098990609797405, CLERIC, 53),
		MakePlayerData("GVN", 15128340028184247367, ROBOT, 27),
		MakePlayerData("HIN", 2108935584337772857, BERSERKER, 82),
		MakePlayerData("YSM", 9644524901984931759, ROBOT, 45),
		MakePlayerData("NCH", 6753947886489153033, MONK, 55),
		MakePlayerData("LPJ", 9035036626792680956, BERSERKER, 63),
		MakePlayerData("BFG", 7070952032463841087, WIZARD, 41),
		MakePlayerData("FYI", 1101676777039888790, BERSERKER, 14),
		MakePlayerData("GXP", 10280263171090265065, NINJA, 15),
		MakePlayerData("ZCK", 5332728178891736138, FIGHTER, 51),
		MakePlayerData("XMI", 9699273119037805845, ELF, 68),
		MakePlayerData("DGC", 14400838723565630865, WIZARD, 10),
		MakePlayerData("QCV", 9179862856063111351, ROBOT, 37),
		MakePlayerData("JVW", 6036259690263356176, WIZARD, 6),
		MakePlayerData("OED", 4808808018092271816, ELF, 76),
		MakePlayerData("FRS", 379919168679847776, WIZARD, 67),
		MakePlayerData("EJH", 4179155948399272624, CLERIC, 97),
		MakePlayerData("SIS", 10522017354112615184, WIZARD, 2),
		MakePlayerData("WPM", 9626429165497058022, WIZARD, 3),
		MakePlayerData("AOT", 14669536239019439082, MONK, 28),
		MakePlayerData("CKC", 12302558344641044695, CLERIC, 67),
		MakePlayerData("OGC", 16703401183230217843, WIZARD, 27),
		MakePlayerData("JMZ", 4185412421421168390, ROBOT, 37),
		MakePlayerData("LQB", 1903164893180190134, WIZARD, 66),
		MakePlayerData("ZTX", 15698131071038755698, ROBOT, 55),
		MakePlayerData("TDK", 14856568172534025539, WIZARD, 62),
		MakePlayerData("MLT", 9638820296810466366, FIGHTER, 58),
		MakePlayerData("ZBK", 7228504447027388787, ELF, 10),
		MakePlayerData("QXW", 4570651870109817996, ELF, 45),
		MakePlayerData("PQX", 1765893625253265903, CLERIC, 69),
		MakePlayerData("NLZ", 3578154471174688170, WIZARD, 89),
		MakePlayerData("BKD", 5090484312988274, WIZARD, 100),
		MakePlayerData("TDR", 16222912577953321074, WIZARD, 61),
		MakePlayerData("XOD", 3661096654123390411, BERSERKER, 90),
		MakePlayerData("WLZ", 940750836342483692, WIZARD, 66),
		MakePlayerData("IDF", 14123014554110605095, ROBOT, 29),
		MakePlayerData("FYT", 9950560565508103734, FIGHTER, 39),
		MakePlayerData("KOZ", 7640803456770266509, FIGHTER, 63),
		MakePlayerData("GCB", 3051218892057208998, ELF, 14),
		MakePlayerData("QXC", 12340992735108316587, ELF, 25),
		MakePlayerData("YCL", 5523095805834577967, BERSERKER, 9),
		MakePlayerData("BEZ", 12567711447076093394, WIZARD, 43),
		MakePlayerData("MST", 5030064486256468226, WIZARD, 38),
		MakePlayerData("LFC", 13058160897158668349, WIZARD, 26),
		MakePlayerData("OJS", 5636486881866853494, ELF, 97),
		MakePlayerData("UVV", 11509078631455659926, NINJA, 42),
		MakePlayerData("OGW", 14299116058576147499, ROBOT, 29),
		MakePlayerData("YXH", 17198143680411673020, BERSERKER, 77),
		MakePlayerData("HUG", 15931597798376349035, FIGHTER, 29),
		MakePlayerData("YGY", 9485508836547650751, NINJA, 82),
		MakePlayerData("XIB", 9855085222108891345, WIZARD, 76),
		MakePlayerData("UAI", 8447723431214828078, CLERIC, 31),
		MakePlayerData("YRS", 4628566501544544814, NINJA, 58),
		MakePlayerData("HMD", 9658304037557722259, CLERIC, 52),
		MakePlayerData("SQD", 262809625585155241, WIZARD, 96),
		MakePlayerData("LUP", 14168007699486419773, BERSERKER, 78),
		MakePlayerData("ECZ", 1231886879658129190, BERSERKER, 98),
		MakePlayerData("TUP", 15225376583439530046, BERSERKER, 88),
		MakePlayerData("IFV", 6275588355045319914, MONK, 79),
		MakePlayerData("HNF", 3311428982719673524, FIGHTER, 67),
		MakePlayerData("PEL", 17253976775686456958, MONK, 68),
		MakePlayerData("PWP", 5520899000010280781, BERSERKER, 61),
		MakePlayerData("ZBK", 365537885648699089, FIGHTER, 34),
		MakePlayerData("BXK", 12184720484512569607, ELF, 57),
		MakePlayerData("JUX", 13775377273007755427, MONK, 57),
		MakePlayerData("NVV", 10833474993504240693, WIZARD, 73),
		MakePlayerData("GPR", 9536179054684172319, ROBOT, 64),
		MakePlayerData("EDS", 11525133656327242184, BERSERKER, 69),
		MakePlayerData("NBV", 3464954615912565959, BERSERKER, 90),
		MakePlayerData("DWJ", 4630336378510180557, NINJA, 77),
		MakePlayerData("FOT", 2106361508663981974, ELF, 35),
		MakePlayerData("POW", 4881572205562057779, CLERIC, 59),
		MakePlayerData("TMN", 9205353662303111521, ELF, 17),
		MakePlayerData("CKI", 9793126461779788495, MONK, 94),
		MakePlayerData("CWK", 3493578230358846847, ROBOT, 76),
		MakePlayerData("IYA", 13577112652761367971, MONK, 95),
		MakePlayerData("BOS", 11909781448269264042, FIGHTER, 13),
		MakePlayerData("YYW", 11882782480596826203, ROBOT, 57),
		MakePlayerData("PME", 5191321089807815718, NINJA, 73),
		MakePlayerData("NHW", 134343987742605338, FIGHTER, 55),
		MakePlayerData("TBX", 6281000636422092066, CLERIC, 81),
		MakePlayerData("OQG", 12628746737940016245, WIZARD, 61),
		MakePlayerData("WUT", 1289156287700056156, MONK, 95),
		MakePlayerData("FQF", 11476162430666872509, MONK, 33),
		MakePlayerData("KCK", 14632878012352647689, MONK, 76),
		MakePlayerData("CHE", 1128726399029920642, BERSERKER, 23),
		MakePlayerData("BOK", 2654351904948789748, ELF, 14),
		MakePlayerData("PWI", 12711684004356459093, ROBOT, 58),
		MakePlayerData("CNP", 12657663016428907494, NINJA, 44),
		MakePlayerData("VCX", 3014246661460019150, WIZARD, 59),
		MakePlayerData("QQV", 10329087563482453063, WIZARD, 55),
		MakePlayerData("FUV", 8719558774116212176, ROBOT, 24),
		MakePlayerData("MHL", 15016873508651255044, NINJA, 65),
		MakePlayerData("IAX", 12739248211764725257, FIGHTER, 71),
		MakePlayerData("ADN", 13859701559886274225, ELF, 38),
		MakePlayerData("JJE", 5043558472356117182, ELF, 36),
		MakePlayerData("JFZ", 7161139518872771056, FIGHTER, 15),
		MakePlayerData("LSH", 8446128873520773574, CLERIC, 48),
		MakePlayerData("HIA", 13404913248053198098, MONK, 55),
		MakePlayerData("MYR", 15625112050343536084, WIZARD, 65),
		MakePlayerData("FHB", 6998117329207396860, FIGHTER, 38),
		MakePlayerData("UDR", 4446557441217628666, ELF, 82),
		MakePlayerData("BFN", 9129655844244834381, BERSERKER, 42),
		MakePlayerData("XOP", 7845879575711925100, WIZARD, 20),
		MakePlayerData("RZD", 459618382339592127, NINJA, 29),
		MakePlayerData("AXT", 8729749653232686467, FIGHTER, 87),
		MakePlayerData("GNI", 13225420362654349004, ELF, 71),
		MakePlayerData("LQF", 16617410932598729731, MONK, 26),
		MakePlayerData("MRA", 17292468748006590867, CLERIC, 33),
		MakePlayerData("OWR", 12320662518682636191, BERSERKER, 41),
		MakePlayerData("LQW", 2698909807862983930, CLERIC, 55),
		MakePlayerData("LQA", 15925017779053646010, BERSERKER, 20),
		MakePlayerData("GFG", 13879382438790271509, MONK, 54),
		MakePlayerData("AWR", 12793140773249674352, ELF, 38),
		MakePlayerData("VUF", 11231783888323796423, MONK, 65),
		MakePlayerData("NTH", 8994425372840882134, ROBOT, 61),
		MakePlayerData("OSY", 8117408625635223720, BERSERKER, 78),
		MakePlayerData("HPO", 11986624996449852799, ROBOT, 45),
		MakePlayerData("UFI", 400582958598404904, NINJA, 55),
		MakePlayerData("XUL", 10804031488564238656, WIZARD, 39),
		MakePlayerData("DDK", 14408300733671434739, MONK, 6),
		MakePlayerData("FNY", 8678832529931886133, MONK, 84),
		MakePlayerData("BPB", 11776524586424907413, CLERIC, 4),
		MakePlayerData("OJV", 12871634890960356101, NINJA, 21),
		MakePlayerData("WMG", 7039898639530858590, NINJA, 5),
		MakePlayerData("VBU", 16190335164423983204, MONK, 14),
		MakePlayerData("SRK", 7304504561663873417, MONK, 75),
		MakePlayerData("PXG", 14477495869798673879, BERSERKER, 82),
		MakePlayerData("RSF", 6497524401186763929, BERSERKER, 66),
		MakePlayerData("LTU", 13840199514804431036, FIGHTER, 37),
		MakePlayerData("IRX", 6703504930448670213, ELF, 70),
		MakePlayerData("SBH", 3880240144219347338, ROBOT, 68),
		MakePlayerData("LYN", 9698704087356728854, CLERIC, 70),
		MakePlayerData("ZOJ", 10955664056319355734, BERSERKER, 8),
		MakePlayerData("KEB", 1920564175509540621, FIGHTER, 18),
		MakePlayerData("VAU", 2510259936450070936, ROBOT, 67),
		MakePlayerData("GWE", 7417653144153994122, CLERIC, 84),
		MakePlayerData("GLF", 11298260777031514157, CLERIC, 58),
		MakePlayerData("VXA", 11809076088996608297, MONK, 67),
		MakePlayerData("WBM", 1509777620763941278, WIZARD, 85),
		MakePlayerData("RCI", 10500757441195608943, CLERIC, 50),
		MakePlayerData("WXM", 5880492554253297568, MONK, 36),
		MakePlayerData("IYI", 3788633712516932326, FIGHTER, 76),
		MakePlayerData("OGW", 15438209857801380316, NINJA, 43),
		MakePlayerData("YTT", 15697654197704400005, ELF, 100),
		MakePlayerData("KTS", 1090876727516280562, ROBOT, 58),
		MakePlayerData("JPP", 1845886214904055123, NINJA, 42),
		MakePlayerData("KFA", 13335475871356545977, FIGHTER, 89),
		MakePlayerData("WCS", 2189063453317578316, NINJA, 71),
		MakePlayerData("TTD", 7187245995686708426, BERSERKER, 70),
		MakePlayerData("TVV", 10788500208017661060, BERSERKER, 27),
		MakePlayerData("XZR", 14220986012258842173, BERSERKER, 2),
		MakePlayerData("AOO", 4927737712435865995, ROBOT, 50),
		MakePlayerData("OZX", 5605050686165851571, FIGHTER, 22),
		MakePlayerData("WOG", 12700799473901709330, ELF, 95),
		MakePlayerData("REE", 16139205467413170837, CLERIC, 44),
		MakePlayerData("YKW", 4233008590632730829, FIGHTER, 11),
		MakePlayerData("QOS", 16137701867756263580, FIGHTER, 57),
		MakePlayerData("MEQ", 14848531264879229849, WIZARD, 66),
		MakePlayerData("YAB", 12395403736806989971, MONK, 87),
		MakePlayerData("QZG", 6337434286118627551, ROBOT, 83),
		MakePlayerData("WAD", 2275175629261456217, NINJA, 13),
		MakePlayerData("DES", 9529327428843413305, BERSERKER, 87),
		MakePlayerData("URD", 9664378175755129643, FIGHTER, 77),
		MakePlayerData("ZVU", 10515044733418991955, CLERIC, 15),
		MakePlayerData("VTP", 12459642022005038881, NINJA, 33),
		MakePlayerData("JZQ", 1513157973650915349, ELF, 2),
		MakePlayerData("ERJ", 703390288209175798, NINJA, 3),
		MakePlayerData("NCB", 14976611925030898944, CLERIC, 70),
		MakePlayerData("TFB", 8466708441920859406, MONK, 91),
		MakePlayerData("HQN", 5074343595853459671, FIGHTER, 56),
		MakePlayerData("DKB", 2348758840828155059, FIGHTER, 82),
		MakePlayerData("ZVV", 14262093353097386624, ROBOT, 11),
		MakePlayerData("PWZ", 13866500116098796192, ROBOT, 32),
		MakePlayerData("ACJ", 15227165058465380906, NINJA, 82),
		MakePlayerData("GXB", 11223177566736662977, FIGHTER, 42),
		MakePlayerData("MPK", 13494486481635114160, CLERIC, 19),
		MakePlayerData("IPJ", 1492350492127223354, ELF, 19),
		MakePlayerData("OAO", 15422549173306096446, FIGHTER, 31),
		MakePlayerData("BJI", 13197650575499616423, CLERIC, 41),
		MakePlayerData("GIL", 13378269260151276340, WIZARD, 15),
		MakePlayerData("WSE", 7602510944392565667, FIGHTER, 1),
		MakePlayerData("JIK", 9023589297010727496, MONK, 94),
		MakePlayerData("PJG", 2797591250650201996, BERSERKER, 79),
		MakePlayerData("BYP", 8600768625749418736, NINJA, 89),
		MakePlayerData("BZJ", 5801775992195475590, BERSERKER, 84),
		MakePlayerData("TJR", 6537953771706811648, ROBOT, 43),
		MakePlayerData("KTL", 5635238932748491455, CLERIC, 32),
		MakePlayerData("FGR", 14623943983140486800, ELF, 51),
		MakePlayerData("RNO", 14574114259302500595, CLERIC, 16),
		MakePlayerData("KNI", 12417742101464547617, MONK, 87),
		MakePlayerData("ZUJ", 13940516041521299760, NINJA, 96),
		MakePlayerData("GIJ", 16798229444752555777, ROBOT, 95),
		MakePlayerData("NDN", 6080142808745278428, WIZARD, 98),
		MakePlayerData("CWJ", 17191853766138416364, ROBOT, 36),
		MakePlayerData("GJL", 6976052037042882805, CLERIC, 42),
		MakePlayerData("CIV", 2054488791213521909, ROBOT, 60),
		MakePlayerData("YVY", 15130672496403505049, CLERIC, 85),
		MakePlayerData("PTY", 6459521782674001830, BERSERKER, 80),
		MakePlayerData("DRV", 4454132438207878087, FIGHTER, 90),
		MakePlayerData("AAO", 13221813071077340259, ROBOT, 27),
		MakePlayerData("YXE", 5614642471507900004, CLERIC, 20),
		MakePlayerData("PXJ", 8650977003616229909, WIZARD, 80),
		MakePlayerData("KMT", 14605942358082134914, ELF, 82),
		MakePlayerData("HGE", 1151819266926057221, WIZARD, 10),
		MakePlayerData("BYI", 13195487583814206174, BERSERKER, 70),
		MakePlayerData("UHH", 546637268222637830, ELF, 74),
		MakePlayerData("MWQ", 4876406357827370664, CLERIC, 86),
		MakePlayerData("AJS", 1534507300939651800, ELF, 67),
		MakePlayerData("TGE", 6262669358876554376, ELF, 69),
		MakePlayerData("YIH", 289738449066609610, NINJA, 87),
		MakePlayerData("GCP", 4773047217692370142, FIGHTER, 95),
		MakePlayerData("FZM", 4604725996803598240, NINJA, 17),
		MakePlayerData("NWO", 9316511447375684682, WIZARD, 94),
		MakePlayerData("NAZ", 12392918015805343925, WIZARD, 66),
		MakePlayerData("OKW", 16897428203166552942, CLERIC, 50),
		MakePlayerData("BWS", 1965857751504277089, CLERIC, 87),
		MakePlayerData("JLM", 1101739391006433520, NINJA, 27),
		MakePlayerData("WME", 11511753413214235038, BERSERKER, 17),
		MakePlayerData("CCR", 15166007056873331139, FIGHTER, 72),
		MakePlayerData("WTQ", 699852493831492071, NINJA, 47),
		MakePlayerData("LKV", 14068992542416653224, BERSERKER, 56),
		MakePlayerData("YHZ", 3031805638708086370, BERSERKER, 61),
		MakePlayerData("YSP", 12927247616884375024, FIGHTER, 2),
		MakePlayerData("XMJ", 151010115339468607, NINJA, 77),
		MakePlayerData("RES", 514554580789920901, NINJA, 62),
		MakePlayerData("ZGH", 15200706576911020896, BERSERKER, 98),
		MakePlayerData("ZGX", 15354099092825370922, BERSERKER, 65),
		MakePlayerData("LDH", 15547273785906828860, CLERIC, 34),
		MakePlayerData("LMK", 3811906398360971008, BERSERKER, 42),
		MakePlayerData("GCE", 900168561903333886, BERSERKER, 86),
		MakePlayerData("PSF", 2532803665633645641, ELF, 29),
		MakePlayerData("VND", 3191346806298296819, BERSERKER, 45),
		MakePlayerData("STY", 11003969226938308209, WIZARD, 32),
		MakePlayerData("FIO", 7616328152243990819, FIGHTER, 32),
		MakePlayerData("GLX", 6659313352355826763, NINJA, 97),
		MakePlayerData("XIP", 7732333510108717374, MONK, 96),
		MakePlayerData("YYG", 7877965468055657114, CLERIC, 40),
		MakePlayerData("JIZ", 17242268797038009787, NINJA, 99),
		MakePlayerData("PAF", 12405988458536527569, CLERIC, 93),
		MakePlayerData("GZR", 23166481096613693, FIGHTER, 41),
		MakePlayerData("IBD", 8243278224948775965, ROBOT, 4),
		MakePlayerData("QIJ", 16032292147335676727, CLERIC, 12),
		MakePlayerData("EUS", 11347647586499710624, MONK, 74),
		MakePlayerData("TCQ", 13151447510949354448, BERSERKER, 36),
		MakePlayerData("ABO", 2072429164317888768, BERSERKER, 49),
		MakePlayerData("PUB", 11089810832753916938, FIGHTER, 2),
		MakePlayerData("UZV", 1767965777704144930, BERSERKER, 47),
		MakePlayerData("HNL", 8254913444640416799, ROBOT, 59),
		MakePlayerData("TVW", 3513780560260196656, ELF, 64),
		MakePlayerData("YTY", 1458539182603153964, MONK, 33),
		MakePlayerData("ORK", 16856463521579298274, BERSERKER, 1),
		MakePlayerData("MBD", 1617390553720060690, ROBOT, 69),
		MakePlayerData("ZIL", 3932895737367579387, MONK, 93),
		MakePlayerData("EJU", 14498220385668083611, MONK, 62),
		MakePlayerData("LWO", 7205080232825242899, FIGHTER, 52),
		MakePlayerData("MRL", 1100676154052768218, BERSERKER, 57),
		MakePlayerData("ESA", 2919552166026989461, ELF, 28),
		MakePlayerData("CJI", 16159287005207445205, FIGHTER, 2),
		MakePlayerData("MYP", 8796939686373229065, NINJA, 95),
		MakePlayerData("HRF", 9013139290706088038, BERSERKER, 23),
		MakePlayerData("YUQ", 6580963103718469524, CLERIC, 33),
		MakePlayerData("ZCB", 16822047221062130527, WIZARD, 7),
		MakePlayerData("TYX", 427030288304432318, WIZARD, 79),
		MakePlayerData("LYQ", 10711098120686767598, CLERIC, 63),
		MakePlayerData("OLG", 9261581081225613172, ELF, 59),
		MakePlayerData("BCQ", 1459250097632277057, BERSERKER, 55),
		MakePlayerData("SCG", 3956453889868879183, ROBOT, 89),
		MakePlayerData("TNS", 12343656755490330988, WIZARD, 60),
		MakePlayerData("NDY", 5959581587357293588, ELF, 85),
		MakePlayerData("BIU", 11552264944133223048, MONK, 56),
		MakePlayerData("MPX", 16961665244974975101, CLERIC, 31),
		MakePlayerData("HYU", 3225092028168401437, ROBOT, 96),
		MakePlayerData("BVS", 13534412806164313911, WIZARD, 71),
		MakePlayerData("EGZ", 8672368130955931075, ROBOT, 66),
		MakePlayerData("NGE", 7208076013333361709, WIZARD, 54),
		MakePlayerData("IGE", 785071509013139461, ROBOT, 79),
		MakePlayerData("ISA", 13083095589071434756, CLERIC, 20),
		MakePlayerData("LYO", 5107618519950231687, FIGHTER, 17),
		MakePlayerData("VNU", 15066537170828481512, CLERIC, 52),
		MakePlayerData("NMZ", 5159215931319278696, FIGHTER, 70),
		MakePlayerData("UCS", 6023516761029714822, MONK, 53),
		MakePlayerData("DQV", 13048961821224336107, MONK, 60),
		MakePlayerData("ELR", 8772313455034891611, CLERIC, 8),
		MakePlayerData("CVE", 16757264352525548114, ELF, 37),
		MakePlayerData("UTX", 570302102791467309, NINJA, 93),
		MakePlayerData("NGH", 15249044952578220300, BERSERKER, 12),
		MakePlayerData("QVQ", 4423381221231346903, CLERIC, 36),
		MakePlayerData("LRI", 11894392472037564176, CLERIC, 11),
		MakePlayerData("HVE", 3542603833030381387, MONK, 70),
		MakePlayerData("IUX", 6730139192359394185, WIZARD, 29),
		MakePlayerData("SOG", 3384787405023457747, BERSERKER, 31),
		MakePlayerData("JSC", 17140911418442971829, CLERIC, 60),
		MakePlayerData("PMR", 4822299384098272152, ROBOT, 37),
		MakePlayerData("VIV", 6083833408258605064, ROBOT, 81),
		MakePlayerData("JAY", 7659575930842563429, ROBOT, 46),
		MakePlayerData("XFJ", 3384398419203403926, MONK, 70),
		MakePlayerData("JEN", 13261035228497044398, WIZARD, 86),
		MakePlayerData("OVQ", 12150547642787142064, NINJA, 40),
		MakePlayerData("KQC", 12921793990070469837, NINJA, 70),
		MakePlayerData("CST", 13501812861770424270, ROBOT, 49),
		MakePlayerData("NHC", 14909540598525219213, WIZARD, 94),
		MakePlayerData("QCN", 8690112404510112173, ELF, 94),
		MakePlayerData("PKI", 14951863366289371627, WIZARD, 22),
		MakePlayerData("XIA", 12898919329474722746, BERSERKER, 32),
		MakePlayerData("NXZ", 14977553186227243032, WIZARD, 68),
		MakePlayerData("RXT", 3207159364724430768, CLERIC, 49),
		MakePlayerData("MXV", 1805115489692740547, ROBOT, 60),
		MakePlayerData("YMY", 3635831073698755710, ELF, 3),
		MakePlayerData("TRG", 962741384967939990, WIZARD, 12),
		MakePlayerData("NIZ", 5392287814115770988, MONK, 71),
		MakePlayerData("QZA", 3193567980488523008, CLERIC, 96),
		MakePlayerData("CXY", 17063595074308943742, ROBOT, 32),
		MakePlayerData("SSP", 3345965102947496499, NINJA, 2),
		MakePlayerData("FBB", 3978355495644799001, ROBOT, 14),
		MakePlayerData("TXB", 14920048618305138286, NINJA, 31),
		MakePlayerData("KID", 16100734491735216102, BERSERKER, 22),
		MakePlayerData("URB", 9432742091123501181, ROBOT, 70),
		MakePlayerData("BXP", 15797849727351570165, ROBOT, 88),
		MakePlayerData("DPR", 3584589041010748286, MONK, 0),
		MakePlayerData("STH", 11520533161803271971, NINJA, 67),
		MakePlayerData("WCL", 2430922681286407655, FIGHTER, 8),
		MakePlayerData("ZNY", 13351687150101968318, NINJA, 8),
		MakePlayerData("DRA", 7703637360434149062, BERSERKER, 25),
		MakePlayerData("WOB", 11785563721614714216, ELF, 25),
		MakePlayerData("AQB", 8166755357259803977, MONK, 81),
		MakePlayerData("NYD", 15660350042136302649, MONK, 91),
		MakePlayerData("UEU", 6659777558564925055, BERSERKER, 87),
		MakePlayerData("USH", 5884246369458930218, BERSERKER, 4),
		MakePlayerData("PQO", 5323451403444032862, BERSERKER, 48),
		MakePlayerData("STB", 2285515327440872367, CLERIC, 30),
		MakePlayerData("LUG", 7121412825352691973, CLERIC, 5),
		MakePlayerData("WKK", 5687653199693124772, BERSERKER, 60),
		MakePlayerData("EQY", 7032994046025565508, BERSERKER, 7),
		MakePlayerData("HUN", 4469676362004232043, MONK, 11),
		MakePlayerData("VUO", 9125989779054408026, BERSERKER, 34),
		MakePlayerData("DOX", 8321367201006855251, ELF, 1),
		MakePlayerData("UDL", 226050265465147647, ELF, 40),
		MakePlayerData("ROW", 15986965124831137085, FIGHTER, 19),
		MakePlayerData("WOD", 5166062543991802788, BERSERKER, 68),
		MakePlayerData("BJG", 4222880625609898363, MONK, 18),
		MakePlayerData("DLC", 10218231723342251511, ELF, 78),
		MakePlayerData("PHG", 8478334604779666937, BERSERKER, 73),
		MakePlayerData("VTF", 3053733952656026026, NINJA, 60),
		MakePlayerData("UKK", 3212692578103891360, ROBOT, 12),
		MakePlayerData("FPR", 1148949767579727734, CLERIC, 73),
		MakePlayerData("IJL", 9764379076011769336, NINJA, 64),
		MakePlayerData("QCO", 16071481199564236635, WIZARD, 57),
		MakePlayerData("MFZ", 14016557092922899470, BERSERKER, 45),
		MakePlayerData("NQL", 14642786279965563691, FIGHTER, 93),
		MakePlayerData("HVG", 717559201419259286, NINJA, 9),
		MakePlayerData("ETZ", 13491027716251051847, NINJA, 38),
		MakePlayerData("QRB", 10672936839460407890, ROBOT, 0),
		MakePlayerData("EPO", 8278756237450638761, ROBOT, 15),
		MakePlayerData("WCV", 7698460278726965683, ROBOT, 20),
		MakePlayerData("SLP", 7857708836865771300, CLERIC, 78),
		MakePlayerData("OAR", 12647872385493508287, ELF, 54),
		MakePlayerData("GXY", 2610473815400306409, BERSERKER, 43),
		MakePlayerData("ENG", 17010008713568914483, CLERIC, 76),
		MakePlayerData("TAL", 6950433978318447690, CLERIC, 7),
		MakePlayerData("DGS", 3107866319629589226, ELF, 57),
		MakePlayerData("DBV", 13310366356861658819, BERSERKER, 30),
		MakePlayerData("YLY", 9975964575432838270, ELF, 18),
		MakePlayerData("JVZ", 9120571109842563128, CLERIC, 5),
		MakePlayerData("KNU", 7016867207348066350, CLERIC, 38),
		MakePlayerData("GZC", 7092933971395117433, BERSERKER, 17),
		MakePlayerData("UEQ", 6810515389398511616, BERSERKER, 57),
		MakePlayerData("OMF", 10880450688010753293, FIGHTER, 85),
		MakePlayerData("TQK", 7392007274657584169, BERSERKER, 6),
		MakePlayerData("JUB", 16308765216069313581, CLERIC, 87),
		MakePlayerData("GBZ", 1680836965947136682, CLERIC, 80),
		MakePlayerData("UPZ", 5989183457845963279, ELF, 1),
		MakePlayerData("QAW", 1782965098957393724, CLERIC, 82),
		MakePlayerData("XUW", 12145828266158844262, CLERIC, 19),
		MakePlayerData("ESW", 10323761252072607599, BERSERKER, 86),
		MakePlayerData("JTR", 14081920264865380391, MONK, 77),
		MakePlayerData("XCA", 4132204185595905807, MONK, 22),
		MakePlayerData("LGO", 16053031633239840143, CLERIC, 75),
		MakePlayerData("DNF", 6549286721241002184, MONK, 25),
		MakePlayerData("QLT", 1645422635152297571, MONK, 23),
		MakePlayerData("YQC", 85587965000564491, ELF, 77),
		MakePlayerData("ZRZ", 4358283289064813431, WIZARD, 76),
		MakePlayerData("EZJ", 15109529636240675120, WIZARD, 53),
		MakePlayerData("IIM", 14698391627855734387, ROBOT, 95),
		MakePlayerData("FPX", 3328027528299026654, ROBOT, 39),
		MakePlayerData("BXV", 2686643894970237138, ROBOT, 12),
		MakePlayerData("HOL", 67981525473563561, ELF, 94),
		MakePlayerData("XMC", 8974337984198564140, MONK, 37),
		MakePlayerData("RLX", 6570684967201914106, BERSERKER, 29),
		MakePlayerData("AMU", 11619493669049592502, CLERIC, 53),
		MakePlayerData("JCW", 4073243132506283061, ELF, 19),
		MakePlayerData("NUI", 12866320375279641352, FIGHTER, 65),
		MakePlayerData("LGN", 3587250316492755735, BERSERKER, 67),
		MakePlayerData("MEE", 6860798457138190880, BERSERKER, 66),
		MakePlayerData("EDU", 16263898812611563613, ELF, 46),
		MakePlayerData("IMC", 8626389818990098596, NINJA, 34),
		MakePlayerData("NLQ", 11970341438777821868, CLERIC, 78),
		MakePlayerData("ZPV", 6994177496342597741, FIGHTER, 65),
		MakePlayerData("MME", 13832408045265217454, MONK, 29),
		MakePlayerData("RVJ", 9129630183118056257, MONK, 43),
		MakePlayerData("WDA", 4375432375338414970, ELF, 99),
		MakePlayerData("ITK", 150537920038048737, ELF, 54),
		MakePlayerData("ZQF", 13532597493906000951, CLERIC, 86),
		MakePlayerData("ZWU", 466611145901972804, WIZARD, 37),
		MakePlayerData("EXD", 9964154724175734274, BERSERKER, 1),
		MakePlayerData("XEJ", 3982649677022735395, ELF, 92),
		MakePlayerData("BXU", 6310091772172765965, WIZARD, 20),
		MakePlayerData("VKA", 7466312085943895734, CLERIC, 26),
		MakePlayerData("EQW", 17113146067859618825, WIZARD, 28),
		MakePlayerData("CPL", 1844461624290963134, NINJA, 66),
		MakePlayerData("XWE", 12556773999183697095, NINJA, 67),
		MakePlayerData("DYQ", 5448179879348390340, ROBOT, 17),
		MakePlayerData("VIM", 7123362486549879067, BERSERKER, 36),
		MakePlayerData("RHC", 11124436520766495650, FIGHTER, 85),
		MakePlayerData("HVO", 11264848214178139650, FIGHTER, 41),
		MakePlayerData("UWN", 16008969617687795520, ELF, 62),
		MakePlayerData("VKO", 1332565347962749701, ROBOT, 60),
		MakePlayerData("UCZ", 12795542070911977889, CLERIC, 74),
		MakePlayerData("BKF", 10557589034016305118, NINJA, 8),
		MakePlayerData("SWE", 14451461102679434015, FIGHTER, 5),
		MakePlayerData("XPK", 13255023944764197466, ROBOT, 83),
		MakePlayerData("JXU", 5048880721901183731, BERSERKER, 88),
		MakePlayerData("HJE", 16330833661626169471, BERSERKER, 60),
		MakePlayerData("XCM", 15483421113841707256, BERSERKER, 65),
		MakePlayerData("TPC", 7207567083741594351, NINJA, 21),
		MakePlayerData("NDY", 9224492888893600178, ROBOT, 40),
		MakePlayerData("XKR", 9685434394904177549, ELF, 5),
		MakePlayerData("VZX", 12063158233682313355, BERSERKER, 3),
		MakePlayerData("PFC", 4076155376091122783, ROBOT, 24),
		MakePlayerData("CFV", 10747164766001118302, NINJA, 40),
		MakePlayerData("AJZ", 11549404315930862260, MONK, 77),
		MakePlayerData("QPR", 12963985906787025464, ELF, 30),
		MakePlayerData("EAT", 2183304943797267080, ROBOT, 79),
		MakePlayerData("HJK", 14921095427539022427, FIGHTER, 38),
		MakePlayerData("YBG", 6211227893560929829, ROBOT, 42),
		MakePlayerData("RTM", 9578378836542323588, FIGHTER, 101),
		MakePlayerData("REW", 16473979388333567500, NINJA, 35),
		MakePlayerData("NMY", 7753433452929165422, NINJA, 97),
		MakePlayerData("SSL", 7383343271619511571, NINJA, 38),
		MakePlayerData("ERO", 16076412671760774039, NINJA, 40),
		MakePlayerData("YDP", 11200045498863955152, NINJA, 43),
		MakePlayerData("JDX", 7425567564739798340, ELF, 57),
		MakePlayerData("IKP", 470330553861326391, ELF, 5),
		MakePlayerData("ZCT", 14592615809383890881, WIZARD, 78),
		MakePlayerData("UMM", 5111176443430226560, MONK, 95),
		MakePlayerData("OMT", 9264001355163450859, ELF, 36),
		MakePlayerData("PIW", 14685170429707520077, ELF, 48),
		MakePlayerData("ELX", 15272243453528844272, CLERIC, 85),
		MakePlayerData("JGF", 14803769830973772041, ROBOT, 88),
		MakePlayerData("EUG", 2912202775066182230, ROBOT, 18),
		MakePlayerData("OSA", 2194953695043074708, ROBOT, 96),
		MakePlayerData("NFB", 13275583646436842299, ELF, 32),
		MakePlayerData("MWE", 5708502896012859678, ROBOT, 99),
		MakePlayerData("YLK", 6364673885558634973, BERSERKER, 76),
		MakePlayerData("PIY", 15441447064111207761, MONK, 82),
		MakePlayerData("DEJ", 15677817233031050331, ELF, 64),
		MakePlayerData("RRG", 7512969845752857268, MONK, 62),
		MakePlayerData("RRD", 10793055099144531899, MONK, 89),
		MakePlayerData("BNB", 3564517615712918093, NINJA, 39),
		MakePlayerData("AAD", 11443769970912546348, BERSERKER, 78),
		MakePlayerData("MEX", 11249884023630790022, BERSERKER, 75),
		MakePlayerData("WCG", 13715442230454144910, BERSERKER, 84),
		MakePlayerData("SCL", 2692933596170495315, ROBOT, 98),
		MakePlayerData("GDG", 11297937131201914929, FIGHTER, 12),
		MakePlayerData("VUB", 12374110743866204562, BERSERKER, 100),
		MakePlayerData("FKQ", 12533292366979158491, FIGHTER, 88),
		MakePlayerData("RQE", 12207928505597643955, NINJA, 4),
		MakePlayerData("PDM", 13375788749735893773, CLERIC, 65),
		MakePlayerData("OJI", 15360843742475179930, ELF, 92),
		MakePlayerData("PCK", 3401475689454286983, BERSERKER, 88),
		MakePlayerData("YNV", 14689717656221782466, FIGHTER, 49),
		MakePlayerData("ZYA", 7641748232586566923, NINJA, 1),
		MakePlayerData("AFI", 17118712650952511907, NINJA, 31),
		MakePlayerData("XTC", 6182406689385519967, MONK, 7),
		MakePlayerData("OMG", 9266344584869568855, WIZARD, 8),
		MakePlayerData("LWE", 12156561542359553687, WIZARD, 93),
		MakePlayerData("YTC", 8417680400877884281, ELF, 73),
		MakePlayerData("EUK", 15181315316624704658, FIGHTER, 95),
		MakePlayerData("QZX", 11096271915491419215, BERSERKER, 97),
		MakePlayerData("QQA", 5149671279342716610, BERSERKER, 56),
		MakePlayerData("GXS", 4859925263018658602, WIZARD, 1),
		MakePlayerData("NAL", 15165456429124951345, CLERIC, 21),
		MakePlayerData("PAG", 8912471709658095626, MONK, 34),
		MakePlayerData("IRK", 8844606733493188546, NINJA, 16),
		MakePlayerData("EAT", 16314316035093313926, WIZARD, 9),
		MakePlayerData("WQU", 409988893422332342, NINJA, 97),
		MakePlayerData("JQI", 5145261876528522988, FIGHTER, 22),
		MakePlayerData("AQY", 7984327981953106800, WIZARD, 70),
		MakePlayerData("EHL", 4388521773103409830, CLERIC, 78),
		MakePlayerData("XZA", 1381647375326975610, BERSERKER, 94),
		MakePlayerData("BAB", 3731017033470892253, WIZARD, 40),
		MakePlayerData("MXD", 8580933845182916552, ROBOT, 67),
		MakePlayerData("OIF", 17019536497982635617, ROBOT, 74),
		MakePlayerData("BCW", 15297764016407368764, FIGHTER, 87),
		MakePlayerData("IKJ", 9017682883867128777, ELF, 42),
		MakePlayerData("AEQ", 14374898185604819820, BERSERKER, 10),
		MakePlayerData("MKA", 3271691156227326168, FIGHTER, 62),
		MakePlayerData("SFD", 5428309584750986401, WIZARD, 99),
		MakePlayerData("HDS", 5791486830674129953, ELF, 77),
		MakePlayerData("YHR", 14392165221306421793, ELF, 57),
		MakePlayerData("VOI", 10473108373715931884, ROBOT, 27),
		MakePlayerData("NXC", 11798662748804314799, WIZARD, 70),
		MakePlayerData("DCK", 17269617891872654680, BERSERKER, 15),
		MakePlayerData("HEP", 15031751656619974993, CLERIC, 40),
		MakePlayerData("PAR", 5066631488146924227, CLERIC, 7),
		MakePlayerData("YYR", 11276634630601339729, MONK, 27),
		MakePlayerData("IZK", 12781192061833396839, BERSERKER, 93),
		MakePlayerData("KUL", 3699659213588460679, WIZARD, 98),
		MakePlayerData("WJT", 6623835725216117831, ELF, 41),
		MakePlayerData("MGZ", 893255589509863895, FIGHTER, 77),
		MakePlayerData("KTT", 12164820480541394506, FIGHTER, 61),
		MakePlayerData("WKR", 1937478936620605943, CLERIC, 22),
		MakePlayerData("LQF", 16205966686790318502, ELF, 26),
		MakePlayerData("RJU", 15015011731857411187, FIGHTER, 2),
		MakePlayerData("LIW", 9878715030897757312, BERSERKER, 54),
		MakePlayerData("QUI", 7438788017261653747, WIZARD, 9),
		MakePlayerData("EJH", 4244983700183743443, WIZARD, 99),
		MakePlayerData("TRM", 15262585858821631164, CLERIC, 78),
		MakePlayerData("GJB", 16276199954099544463, FIGHTER, 68),
		MakePlayerData("VSR", 7114022082951074934, ELF, 36),
		MakePlayerData("VXM", 5213469330897965827, ROBOT, 6),
		MakePlayerData("QUN", 204842410884573231, CLERIC, 92),
		MakePlayerData("WDC", 6214986001586037630, ROBOT, 51),
		MakePlayerData("HJR", 16240095374185304990, WIZARD, 35),
		MakePlayerData("QBR", 15866069719785860913, NINJA, 16),
		MakePlayerData("VJU", 13163554766456263849, MONK, 90),
		MakePlayerData("CMB", 10196581110767112792, WIZARD, 7),
		MakePlayerData("TCY", 9956437211686066730, ELF, 93),
		MakePlayerData("HLB", 2575538559103725723, ELF, 18),
		MakePlayerData("VQI", 15847107938236669166, MONK, 18),
		MakePlayerData("JWZ", 3312096589716500656, CLERIC, 14),
		MakePlayerData("LEN", 11125484179409450752, ROBOT, 52),
		MakePlayerData("AGF", 15458913875865653315, ELF, 52),
		MakePlayerData("EIV", 12888290725481659685, MONK, 95),
		MakePlayerData("GWD", 15116429932890551319, BERSERKER, 48),
		MakePlayerData("OTA", 10587635169337745065, CLERIC, 37),
		MakePlayerData("PDT", 5769506631822006382, WIZARD, 36),
		MakePlayerData("DQT", 12235244872054312794, ELF, 75),
		MakePlayerData("VJW", 11944816345802933672, MONK, 27),
		MakePlayerData("FUO", 6887244343035903954, NINJA, 83),
		MakePlayerData("RAI", 12945441845818469672, FIGHTER, 19),
		MakePlayerData("YIY", 15086032194850039802, ELF, 49),
		MakePlayerData("JBA", 1310748746158903933, CLERIC, 65),
		MakePlayerData("ZJH", 12132112254790526925, NINJA, 75),
		MakePlayerData("IFN", 11113029523837985983, MONK, 40),
		MakePlayerData("XYC", 13460888850388746843, ROBOT, 35),
		MakePlayerData("ZEF", 9050120741161839671, NINJA, 35),
		MakePlayerData("JTO", 10150361526263904015, FIGHTER, 12),
		MakePlayerData("GSG", 730813784121094211, ROBOT, 99),
		MakePlayerData("TEO", 13860036743254801714, ELF, 97),
		MakePlayerData("JDJ", 1845928815062284666, MONK, 53),
		MakePlayerData("IAP", 11060483302172091824, WIZARD, 73),
		MakePlayerData("SJU", 13331675385758602253, ELF, 38),
		MakePlayerData("VHB", 2842269283094259482, CLERIC, 67),
		MakePlayerData("YNH", 705462382739813998, CLERIC, 63),
		MakePlayerData("BOA", 1646283045549496301, ELF, 96),
		MakePlayerData("BJN", 7458323501378023477, BERSERKER, 48),
		MakePlayerData("ATT", 9017411616520313620, BERSERKER, 84),
		MakePlayerData("VAK", 12996335714864481600, MONK, 43),
		MakePlayerData("XPV", 8285862983424033449, CLERIC, 77),
		MakePlayerData("ZWE", 15503017666042304794, CLERIC, 71),
		MakePlayerData("DAG", 5502392438797905153, ROBOT, 44),
		MakePlayerData("XPQ", 11320312229076045470, FIGHTER, 22),
		MakePlayerData("HCX", 4956148455261839336, MONK, 74),
		MakePlayerData("APU", 16872763083638652893, NINJA, 23),
		MakePlayerData("WGX", 13707152739890898474, NINJA, 66),
		MakePlayerData("PRM", 3035129085978047039, ELF, 8),
		MakePlayerData("WPO", 8732122769601147731, MONK, 25),
		MakePlayerData("FTF", 10177444310073436902, ROBOT, 31),
		MakePlayerData("QIY", 6079062348218552760, WIZARD, 61),
		MakePlayerData("SNW", 16334124862695901017, NINJA, 27),
		MakePlayerData("RKC", 7679399036519800503, WIZARD, 85),
		MakePlayerData("ZVI", 1975445382599655006, WIZARD, 80),
		MakePlayerData("QCG", 17094350584104950076, ELF, 25),
		MakePlayerData("QYU", 8647363045170048296, MONK, 82),
		MakePlayerData("RNS", 148434922225534598, NINJA, 59),
		MakePlayerData("VWW", 13533426546338075828, NINJA, 70),
		MakePlayerData("BIO", 7091641923500962089, ELF, 41),
		MakePlayerData("MPF", 16724141417871666030, CLERIC, 8),
		MakePlayerData("ZCF", 9424421306588464724, ELF, 74),
		MakePlayerData("PKS", 15672908014304388888, ROBOT, 93),
		MakePlayerData("KIP", 9292051731417879893, CLERIC, 84),
		MakePlayerData("KAZ", 7676385059972883181, CLERIC, 66),
		MakePlayerData("UUD", 1099951176318235995, WIZARD, 33),
		MakePlayerData("HAD", 381749394454824451, ELF, 12),
		MakePlayerData("VEM", 8909011026796107132, FIGHTER, 53),
		MakePlayerData("GIB", 16667293535904026095, ROBOT, 19),
		MakePlayerData("VIA", 9846850061222804216, FIGHTER, 10),
		MakePlayerData("TQJ", 2418155830041731501, BERSERKER, 66),
		MakePlayerData("VNZ", 14363746897880396221, CLERIC, 9),
		MakePlayerData("SIS", 16689646761485766847, CLERIC, 47),
		MakePlayerData("JBI", 15571452528625251115, MONK, 34),
		MakePlayerData("QEW", 15566204589923017365, WIZARD, 71),
		MakePlayerData("REL", 9317330863236286234, WIZARD, 22),
		MakePlayerData("OPV", 11578787885663541805, NINJA, 55),
		MakePlayerData("EXG", 12828948417325409051, MONK, 100),
		MakePlayerData("JCI", 7610244692293636605, WIZARD, 48),
		MakePlayerData("OAD", 9186489264198749180, BERSERKER, 37),
		MakePlayerData("CUJ", 12147876375648523278, MONK, 6),
		MakePlayerData("SEW", 14088158445281885584, ROBOT, 86),
		MakePlayerData("GLD", 9816888355313734250, NINJA, 57),
		MakePlayerData("DYK", 781683881129794733, BERSERKER, 44),
		MakePlayerData("GWF", 10738265465404089274, FIGHTER, 63),
		MakePlayerData("DPD", 7600592939250415479, ROBOT, 91),
		MakePlayerData("WOQ", 1917660510268155944, CLERIC, 41),
		MakePlayerData("ZAH", 1417074876338132956, NINJA, 74),
		MakePlayerData("QVP", 1453835668647459468, BERSERKER, 34),
		MakePlayerData("ALG", 10529232050561805409, ELF, 69),
		MakePlayerData("JIY", 4185107619367293381, MONK, 91),
		MakePlayerData("JPZ", 6907255933946989341, MONK, 92),
		MakePlayerData("UDG", 4575376828989470309, BERSERKER, 52),
		MakePlayerData("MZZ", 5200244698851808004, NINJA, 94),
		MakePlayerData("XGG", 14584942966790320274, WIZARD, 13),
		MakePlayerData("XSR", 1389761300972416871, MONK, 15),
		MakePlayerData("MPB", 5225909912034759209, ELF, 40),
		MakePlayerData("ARI", 2896336682964894137, FIGHTER, 26),
		MakePlayerData("QUA", 5448156048645472549, ELF, 18),
		MakePlayerData("QQE", 11678888892277962132, FIGHTER, 35),
		MakePlayerData("EEO", 9201731441623950912, BERSERKER, 11),
		MakePlayerData("FVY", 10372597550378205948, ROBOT, 93),
		MakePlayerData("ZWM", 5026198086436873177, WIZARD, 14),
		MakePlayerData("ZJD", 15919248555311162084, NINJA, 58),
		MakePlayerData("OBV", 8443415371746539799, FIGHTER, 68),
		MakePlayerData("NGC", 7469010225919752687, BERSERKER, 82),
		MakePlayerData("AJE", 7982569594187937100, NINJA, 20),
		MakePlayerData("ZKX", 10910101626071580026, ROBOT, 52),
		MakePlayerData("KRE", 11790929140619206557, ELF, 17),
		MakePlayerData("JAM", 2689528079920043736, FIGHTER, 53),
		MakePlayerData("AWQ", 9966463365876774624, ELF, 86),
		MakePlayerData("NOX", 2904399274807199965, ELF, 11),
		MakePlayerData("DCH", 366334881972153798, ROBOT, 4),
		MakePlayerData("DKG", 6490926610152119198, FIGHTER, 50),
		MakePlayerData("BJD", 11778740509165684539, WIZARD, 27),
		MakePlayerData("WZU", 8351984332821467768, MONK, 34),
		MakePlayerData("CHE", 4734040026728855307, MONK, 48),
		MakePlayerData("LWD", 9904625170884620368, WIZARD, 38),
		MakePlayerData("YJS", 2352978225329220295, BERSERKER, 75),
		MakePlayerData("ADC", 12705137050435391003, NINJA, 8),
		MakePlayerData("KSP", 6384638067926359784, CLERIC, 98),
		MakePlayerData("SFT", 6536410642414618526, BERSERKER, 47),
		MakePlayerData("VXB", 8173271838558037042, ELF, 30),
		MakePlayerData("SHP", 9496320203807277688, CLERIC, 10),
		MakePlayerData("GVF", 14030230548286781788, ELF, 98),
		MakePlayerData("GAA", 4910940492329849694, BERSERKER, 24),
		MakePlayerData("MOY", 2756510273484676335, MONK, 44),
		MakePlayerData("VNI", 15925012187289863591, WIZARD, 20),
		MakePlayerData("FZI", 5728794830377483117, MONK, 65),
		MakePlayerData("HVP", 3490381331953273097, MONK, 6),
		MakePlayerData("IAE", 169469619654319540, NINJA, 95),
		MakePlayerData("RLA", 14225574508487543202, NINJA, 49),
		MakePlayerData("SHF", 7773658648612256674, MONK, 83),
		MakePlayerData("HVN", 12530206517960783419, ELF, 3),
		MakePlayerData("RLM", 5151650808326149882, NINJA, 89),
		MakePlayerData("TQL", 7894506348843333833, ROBOT, 71),
		MakePlayerData("QLC", 4824234547746374279, NINJA, 4),
		MakePlayerData("IMF", 5174687106665588022, WIZARD, 90),
		MakePlayerData("EZN", 3157641752616450432, WIZARD, 70),
		MakePlayerData("EPD", 9463235492548878943, BERSERKER, 78),
		MakePlayerData("YDW", 13319090894615980844, WIZARD, 65),
		MakePlayerData("NGZ", 5317665496971841887, ROBOT, 9),
		MakePlayerData("LKB", 3001844190174502599, CLERIC, 44),
		MakePlayerData("SHZ", 10544639954840876812, ROBOT, 42),
		MakePlayerData("OZF", 3424246094773375910, ELF, 13),
		MakePlayerData("QRK", 10163036830400015295, MONK, 99),
		MakePlayerData("JDI", 12201200993360789911, ROBOT, 7),
		MakePlayerData("LIF", 4481758394190239065, MONK, 92),
		MakePlayerData("LGN", 16545355216458866060, MONK, 53),
		MakePlayerData("ZEE", 17268922064537841540, FIGHTER, 16),
		MakePlayerData("IZL", 3815096590853293690, CLERIC, 0),
		MakePlayerData("GZB", 15481442103534855735, FIGHTER, 96),
		MakePlayerData("LBJ", 9165651426638036604, NINJA, 96),
		MakePlayerData("XJQ", 13113125414402000510, ROBOT, 89),
		MakePlayerData("QHX", 7368790569298859068, ELF, 66),
		MakePlayerData("JNF", 8564997147805862265, WIZARD, 68),
		MakePlayerData("QEO", 1775322779541987476, FIGHTER, 23),
		MakePlayerData("IHZ", 10801629039000141218, NINJA, 64),
		MakePlayerData("ADH", 1041413175235345521, NINJA, 18),
		MakePlayerData("IJR", 2000353480958382677, ELF, 78),
		MakePlayerData("SUA", 15231131731673567993, ELF, 74),
		MakePlayerData("IYX", 13557946173299313035, ROBOT, 82),
		MakePlayerData("IPC", 2877602902736532462, WIZARD, 31),
		MakePlayerData("XSK", 9822959493383132134, ROBOT, 78),
		MakePlayerData("IHB", 3225034516640991827, ROBOT, 25),
		MakePlayerData("ROD", 1991519896201501428, FIGHTER, 58),
		MakePlayerData("DJO", 5922153504092134222, ELF, 34),
		MakePlayerData("XCO", 8135018478892219821, ROBOT, 37),
		MakePlayerData("QGJ", 12598670988917240143, CLERIC, 5),
		MakePlayerData("CNH", 3943988312181277503, ELF, 77),
		MakePlayerData("XZY", 2850056448233428847, WIZARD, 23),
		MakePlayerData("TBD", 3582890439224543243, FIGHTER, 98),
		MakePlayerData("HRO", 3114091643723385116, WIZARD, 1),
		MakePlayerData("WZS", 8258740632008845553, ROBOT, 95),
		MakePlayerData("QVJ", 6705684001170225651, CLERIC, 80),
		MakePlayerData("KBS", 4556500027208429769, NINJA, 33),
		MakePlayerData("UGQ", 10362083350836828654, ELF, 95),
		MakePlayerData("HXO", 2633760139692215347, CLERIC, 64),
		MakePlayerData("MXJ", 15869293351641119168, CLERIC, 10),
		MakePlayerData("CMH", 16167860490379806609, NINJA, 25),
		MakePlayerData("AOO", 2714694063151051646, BERSERKER, 95),
		MakePlayerData("MAU", 14500609034901609298, WIZARD, 63),
		MakePlayerData("PNH", 7414156832712473472, NINJA, 91),
		MakePlayerData("PPD", 14044704537654839922, ELF, 51),
		MakePlayerData("TVY", 3016729111340449160, MONK, 100),
		MakePlayerData("GXS", 13121019312978745152, ROBOT, 37),
		MakePlayerData("TTT", 7277232513277848098, CLERIC, 24),
		MakePlayerData("DXA", 3007648728492577376, FIGHTER, 87),
		MakePlayerData("IGD", 10389943416761276492, FIGHTER, 60),
		MakePlayerData("ELB", 14164588429165725713, NINJA, 63),
		MakePlayerData("THJ", 13576216248372420439, CLERIC, 18),
		MakePlayerData("AKG", 3834446360876896177, NINJA, 15),
		MakePlayerData("RCW", 10379461384804683021, BERSERKER, 54),
		MakePlayerData("LEU", 13222419608916132223, NINJA, 7),
		MakePlayerData("GYO", 5963752436511120405, CLERIC, 67),
		MakePlayerData("RFG", 16532765404545668769, ROBOT, 56),
		MakePlayerData("XIR", 7726707918013943445, ELF, 82),
		MakePlayerData("KTN", 7662884267288131431, ROBOT, 17),
		MakePlayerData("GZL", 665706407542739223, MONK, 47),
		MakePlayerData("QYS", 14310244774572800975, NINJA, 99),
		MakePlayerData("XPY", 11439794368788875695, ELF, 70),
		MakePlayerData("URL", 6526320801189126730, WIZARD, 10),
		MakePlayerData("GWY", 2477433565820942171, FIGHTER, 30),
		MakePlayerData("UPL", 13204289864161853630, NINJA, 59),
		MakePlayerData("ZQK", 15972939554542519724, MONK, 84),
		MakePlayerData("ECB", 8857807905358394316, CLERIC, 83),
		MakePlayerData("CSW", 15126457817578600611, CLERIC, 2),
		MakePlayerData("OHY", 12152951720029278293, CLERIC, 37),
		MakePlayerData("TGL", 3879339665560343775, WIZARD, 58),
		MakePlayerData("OOK", 8125635736546581304, CLERIC, 27),
		MakePlayerData("FGJ", 1627395238782920693, WIZARD, 1),
		MakePlayerData("NJF", 2961254615461317471, ROBOT, 95),
		MakePlayerData("ZSA", 1065558448313783584, CLERIC, 39),
		MakePlayerData("JJA", 1971199037133543930, CLERIC, 64),
		MakePlayerData("WTF", 14836743338506930716, ELF, 63),
		MakePlayerData("QKU", 10964168632141407499, FIGHTER, 41),
		MakePlayerData("WMZ", 7098405570519236685, MONK, 36),
		MakePlayerData("XKI", 13056432616604051492, NINJA, 88),
		MakePlayerData("OBJ", 8216107561845550628, CLERIC, 81),
		MakePlayerData("ILU", 365600970063695725, CLERIC, 40),
		MakePlayerData("CPJ", 11548657043534924996, ROBOT, 88),
		MakePlayerData("SOX", 16091911958154848490, ELF, 22),
		MakePlayerData("KWT", 7027552755784111585, NINJA, 78),
		MakePlayerData("ZHD", 6270336191701612540, ELF, 11),
		MakePlayerData("ENA", 16819576468058054122, NINJA, 41),
		MakePlayerData("KFF", 15465329777246695206, ROBOT, 91),
		MakePlayerData("DVZ", 10465973261684846185, MONK, 92),
		MakePlayerData("ZDW", 12640833933447631890, BERSERKER, 68),
		MakePlayerData("YBB", 9814900756405732519, MONK, 44),
		MakePlayerData("DRA", 14063221058844692948, WIZARD, 95),
		MakePlayerData("JIG", 7498034140065847445, ROBOT, 73),
		MakePlayerData("SAO", 10830854247438138735, ELF, 46),
		MakePlayerData("DIR", 6961320247709224936, WIZARD, 46),
		MakePlayerData("UUG", 8177154530635534487, NINJA, 12),
		MakePlayerData("YRM", 6104251495469908356, BERSERKER, 94),
		MakePlayerData("WOB", 40033228827664054, MONK, 59),
		MakePlayerData("EVZ", 7717678054021819676, MONK, 14),
		MakePlayerData("OWY", 8495305332609566770, WIZARD, 0),
		MakePlayerData("TBJ", 983610415610481432, FIGHTER, 69),
		MakePlayerData("NWX", 5783213389615378414, FIGHTER, 57),
		MakePlayerData("SVD", 13368741036968270007, FIGHTER, 66),
		MakePlayerData("NOG", 7626839908086167312, MONK, 19),
		MakePlayerData("WIY", 10001132025712575607, ROBOT, 95),
		MakePlayerData("VMO", 12611206355641238275, NINJA, 8),
		MakePlayerData("CPE", 1575119847582949042, CLERIC, 4),
		MakePlayerData("LTK", 3687113461406978493, BERSERKER, 58),
		MakePlayerData("TUP", 11052096264244195979, CLERIC, 91),
		MakePlayerData("INY", 10538548891160322114, WIZARD, 82),
		MakePlayerData("UAN", 16936377183167343369, ROBOT, 77),
		MakePlayerData("MXJ", 14227717156775176042, MONK, 83),
		MakePlayerData("YMZ", 15691125147735926915, WIZARD, 64),
		MakePlayerData("UTW", 2991980346742141665, ELF, 48),
		MakePlayerData("HED", 15548986607102077500, BERSERKER, 35),
		MakePlayerData("KHS", 5570069894650107501, ROBOT, 92),
		MakePlayerData("FDM", 7437531042481565775, CLERIC, 79),
		MakePlayerData("WEY", 16633196002764834165, ELF, 95),
		MakePlayerData("WCG", 7739288423014414799, BERSERKER, 22),
		MakePlayerData("MGQ", 3868899961695031487, ROBOT, 88),
		MakePlayerData("LTF", 8775426731430855464, ELF, 44),
		MakePlayerData("FEF", 12750157307147800185, WIZARD, 50),
		MakePlayerData("JRS", 6672552714048923850, WIZARD, 55),
		MakePlayerData("XAI", 15511126159589830780, MONK, 52),
		MakePlayerData("HTR", 2453208813250236934, MONK, 6),
		MakePlayerData("JNJ", 10354321473011862733, ROBOT, 20),
		MakePlayerData("QOX", 16882657582489831481, WIZARD, 32),
		MakePlayerData("ZCV", 13951774651754716262, CLERIC, 49),
		MakePlayerData("EWB", 5689330420548771368, MONK, 65),
		MakePlayerData("ZTM", 1319166501461767221, NINJA, 44),
		MakePlayerData("JMN", 10839437113029020588, FIGHTER, 50),
		MakePlayerData("VGU", 5669244025216187490, ELF, 22),
		MakePlayerData("CMF", 7901241512075043216, BERSERKER, 88),
		MakePlayerData("DEN", 17271943281439828438, BERSERKER, 47),
		MakePlayerData("RIS", 1929974164653619640, ROBOT, 29),
		MakePlayerData("SHT", 5760211087992765946, WIZARD, 22),
		MakePlayerData("ETF", 235215787816290180, NINJA, 54),
		MakePlayerData("TUM", 13892192481481429181, WIZARD, 28),
		MakePlayerData("MPV", 8632226877612502133, ELF, 58),
		MakePlayerData("BDA", 1573226739065088218, FIGHTER, 32),
		MakePlayerData("JJJ", 12734779558487127311, ROBOT, 93),
		MakePlayerData("DKB", 11878395451128866392, ELF, 38),
		MakePlayerData("BXE", 15950576567364805118, FIGHTER, 45),
		MakePlayerData("EZD", 4684574693505549139, ELF, 16),
		MakePlayerData("KSH", 10420059218460303741, BERSERKER, 49),
		MakePlayerData("TYX", 17275156192671411963, NINJA, 84),
		MakePlayerData("TPW", 9053511434240335093, ELF, 84),
		MakePlayerData("GRC", 2137934787766453978, CLERIC, 72),
		MakePlayerData("XZD", 13754516135895334626, WIZARD, 55),
		MakePlayerData("UQR", 2472283773992251996, ROBOT, 43),
		MakePlayerData("XMT", 7631362865942942972, ROBOT, 33),
		MakePlayerData("MYB", 6402708232558421130, MONK, 4),
		MakePlayerData("IMQ", 16918217818310226580, FIGHTER, 13),
		MakePlayerData("JOQ", 15899129113522391063, ROBOT, 85),
		MakePlayerData("QUR", 2976681205997827900, CLERIC, 6),
		MakePlayerData("IBV", 14972300260597946782, ROBOT, 3),
		MakePlayerData("LHY", 4799876555238219180, CLERIC, 95),
		MakePlayerData("JHU", 12279400573704664106, MONK, 12),
		MakePlayerData("DBM", 12229841613048690402, CLERIC, 77),
		MakePlayerData("UMU", 16799808112279540728, MONK, 40),
		MakePlayerData("OLR", 2991340911675487625, CLERIC, 46),
		MakePlayerData("TOS", 14494855784195691386, NINJA, 11),
		MakePlayerData("QKR", 759855367011036573, MONK, 94),
		MakePlayerData("DPN", 4065799455938406045, BERSERKER, 74),
		MakePlayerData("EAE", 6292988059415342157, ROBOT, 17),
		MakePlayerData("TCB", 15609700788962826317, NINJA, 10),
		MakePlayerData("ZPO", 11638007146137291810, WIZARD, 8),
		MakePlayerData("CSF", 15795692506935316022, WIZARD, 68),
		MakePlayerData("LRT", 15161114907381655902, ELF, 94),
		MakePlayerData("LRQ", 8436653576065373902, ELF, 63),
		MakePlayerData("WQG", 12214051017259588264, BERSERKER, 17),
		MakePlayerData("BZJ", 36259498666460561, CLERIC, 25),
		MakePlayerData("GRA", 8312833079730885095, FIGHTER, 39),
		MakePlayerData("CCB", 7753172579329794444, NINJA, 26),
		MakePlayerData("QCA", 14654714848513116524, ROBOT, 16)
		});
	return players;
}




bool testForCycle(const vector<std::pair<int, int>>& edges) {
	CycleCheck cc;
	int maxId = -1;
	for (auto& edge : edges) {
		cc.addEdge(edge.first, edge.second);
		maxId = max(maxId, max(edge.first, edge.second));
	}
	if (maxId == -1) { return false; }
	return cc.checkCycle(maxId+1, 0);
}

void testCycleChecker() {

	{
		vector<std::pair<int, int>> edges = {
			{0, 1},
			{1, 2}
		};
		bool cycle = testForCycle(edges);
		assert(cycle == false);
	}

	{
		vector<std::pair<int, int>> edges = {
			{0, 1},
			{1, 0}
		};
		bool cycle = testForCycle(edges);
		assert(cycle == true);
	}

	{
		vector<std::pair<int, int>> edges = {
			{0, 1},
			{1, 2},
			{2, 3},
			{4, 5},
			{5, 6},
			{6, 7},
			{7, 8},
			{3, 8}
		};
		bool cycle = testForCycle(edges);
		assert(cycle == false);
	}

	{
		vector<std::pair<int, int>> edges = {
			{0, 1},
			{1, 2},
			{2, 3},
			{4, 5},
			{5, 6},
			{6, 7},
			{2, 7},
			{7, 8},
			{3, 8}
		};
		bool cycle = testForCycle(edges);
		assert(cycle == false);
	}

	{
		vector<std::pair<int, int>> edges = {
			{0, 1},
			{1, 2},
			{2, 3},
			{4, 5},
			{5, 6},
			{6, 7},
			{2, 7},
			{7, 8},
			{3, 8},
			{8, 0}
		};
		bool cycle = testForCycle(edges);
		assert(cycle);
	}

	cout << " cycles checked !!!!" << std::endl;
}


void testTasker() {

	{
		std::vector<std::set<int>> dependencies = {
			{},
			{0},
			{1},
			{1,2}
		};

		std::vector<Task*> tasks;
		for (size_t i = 0; i < dependencies.size(); i++)
		{
			tasks.push_back(new Task(i, dependencies[i]));
		}

		TaskManager taskManager(tasks);
		taskManager.doTasks();
	}

	{
		std::vector<std::set<int>> dependencies = {
			{},
			{0},
			{1, 0},
			{1,2, 0}
		};

		std::vector<Task*> tasks;
		for (size_t i = 0; i < dependencies.size(); i++)
		{
			tasks.push_back(new Task(i, dependencies[i]));
		}
		TaskManager taskManager(tasks);
		taskManager.doTasks();
	}

	{
		std::vector<std::set<int>> dependencies = {
			{1},
			{},
			{0},
			{2, 0, 1}
		};

		std::vector<Task*> tasks;
		for (size_t i = 0; i < dependencies.size(); i++)
		{
			tasks.push_back(new Task(i, dependencies[i]));
		}
		TaskManager taskManager(tasks);
		taskManager.doTasks();
	}

	{
		std::vector<std::set<int>> dependencies = {
			{},    // 0
			{0},   // 1
			{1},   // 2
			{2},   // 3
			{},    // 4
			{4},   // 5
			{5}    // 6
		};

		std::vector<Task*> tasks;
		for (size_t i = 0; i < dependencies.size(); i++)
		{
			tasks.push_back(new Task(i, dependencies[i]));
		}
		TaskManager taskManager(tasks);
		taskManager.doTasks();
	}

	{
		std::vector<std::set<int>> dependencies = {
			{},    // 0
			{0},   // 1
			{1},   // 2
			{2},   // 3
			{0},    // 4
			{4},   // 5
			{5},    // 6
			{3,6}    // 7
		};

		std::vector<Task*> tasks;
		for (size_t i = 0; i < dependencies.size(); i++)
		{
			tasks.push_back(new Task(i, dependencies[i]));
		}
		TaskManager taskManager(tasks);
		taskManager.doTasks();
	}
	cout << " tasks checked !!!!" << std::endl;


}

bool canSegmentString(string s, unordered_set<string>& dictionary) {
	int n = s.length();
	vector<bool> segmentableAt(n + 1, false);
	segmentableAt[0] = true; // Empty string can be segmented
	cout << "segment " << s << std::endl;
	cout << "dictionary:";
	std::copy(
		dictionary.begin(),
		dictionary.end(),
		std::ostream_iterator<string>(std::cout, " ")
	);

	cout << std::endl;

	for (int substringEnd = 1; substringEnd <= n; substringEnd++) {
		for (int substringStart = 0; substringStart < substringEnd; substringStart++) {
			auto substring = s.substr(substringStart, substringEnd - substringStart);
			cout << "check " << substringStart << "-" << substringEnd << " " << substring << std::endl;
			if (segmentableAt[substringStart] && dictionary.find(substring) != dictionary.end()) {
				segmentableAt[substringEnd] = true;
				break;
			}
		}
	}

	return segmentableAt[n];
}

void testSegmentString() {

	{
		string s = "hellonow";
		unordered_set <string> dictonary = { "hello", "hell", "on", "now" };
		if (canSegmentString(s, dictonary))
			cout << "String Can be Segmented" << endl;
		else
			cout << endl << "String Can NOT be Segmented" << endl;
	}

}

//bool getWheelsRemaining(int numberOfWheels, int wheelsOnVehicle, int& vehiclesUsed, int& remainingWheels) {
//
//	if (wheelsOnVehicle == 0) {
//		vehiclesUsed = 0;
//		remainingWheels = numberOfWheels;
//	}
//	else {
//		vehiclesUsed = numberOfWheels / wheelsOnVehicle;
//		remainingWheels = numberOfWheels % wheelsOnVehicle;
//	}
//	return remainingWheels != 0;
//}
//
//int  getMinimumVehicles(int numberOfWheels, std::vector<int>& minimumVehicles, const std::vector<int>& numberOfWheelsPerVehicle) {
//
//	int vehiclesUsed = 0;
//	int remainingWheels = numberOfWheels;
//
//	for (size_t i = 0; i < numberOfWheelsPerVehicle.size(); i++) {
//
//		int wheelsOnVehicle = numberOfWheelsPerVehicle[i];
//		bool done = !getWheelsRemaining(remainingWheels, wheelsOnVehicle, vehiclesUsed, remainingWheels);
//		minimumVehicles.push_back(vehiclesUsed);
//		if (done) {
//			break;
//		}
//
//	}
//	return remainingWheels;
//}
//
//void testWheelCount()
//{
//	//Based on the number of wheels, I should return how many ways it can be arranged or not arranged,
//	//	The vehicles available are 4 wheelers and 2 wheelers and if the wheelcount is 6, the output will be 2 as it can be arranged in 2 ways 3 2W and 1 4W and 1 2W.
//
//	std::vector<int> numberOfWheelsPerVehicle = { 4, 2 }; // sorted
//
//	// keep index aligned -- calculate
//	std::vector<std::vector<int> > wheelConversion= {
//		{1, 2}, // one 4 wheeler is worth 2 2wheelers 
//		{0, 1}, // one 2 wheeler is worth 1 2 wheelers, and you can't replace a two wheeler with a four wheeler
//	}; 
//	int numberOfWheels = 6;
//	std::vector<int> minimumVehicles;
//	int remainingWheels = getMinimumVehicles( numberOfWheels, minimumVehicles, numberOfWheelsPerVehicle);
//
//	// so: if there are no remaining wheels (what if there are?)
//	// with the minimum vehicles, each vehicle counts as (wheelConversion) extra combinations
//	// the minimum is one combo
//	// replacing one 4x with 2 2x is another combo
//	// so each 4x gives one extra combo
//	std::vector<std::vector<int>> vehicleCombos;
//
//	// if we only need the combo count, then no need spending time to construct the combos
//	int combos = 0;
//	if (!remainingWheels) {
//		vehicleCombos.push_back(minimumVehicles);
//		combos++;
//		for (size_t i = 0; i < numberOfWheelsPerVehicle.size(); i++) {
//			if (i + 1 >= numberOfWheelsPerVehicle.size()) {
//				continue;
//			}
//
//			for (size_t i = 0; i < numberOfWheelsPerVehicle.size(); i++) {
//
//				int vehiclesUsed = 0;
//				int remainingWheels = 0;
//				int wheelsOnNextVehicle = numberOfWheelsPerVehicle[i+1];
//				// still don't know what to do with the extra
//				bool extra = getWheelsRemaining(numberOfWheelsPerVehicle[i], wheelsOnNextVehicle, vehiclesUsed, remainingWheels);
//
//			}
//
//		}
//	}
//
//
//}

int groupSizeSolutions(vector<int>& groupSizes, int amount) {

	vector<int> solution(amount + 1); // solutions for amounts from 0-N
	solution[0] = 1; // only one way to answer amount == 0

	for (int groupSize : groupSizes) {
		for (int tgtAmt = groupSize; tgtAmt < (amount + 1); ++tgtAmt) {
			auto sum = tgtAmt - groupSize;
			solution[tgtAmt] += solution[sum];
		}
	}

	return solution.back();
}



void testCoinChange() {
	vector<int> denominations = { 1, 5, 10 };
	int amount = 13;
	int result = groupSizeSolutions(denominations, amount);
	// printing result
	cout << endl << "testCoinChange([";
	for (int den : denominations) {
		cout << den << " ";
	}
	cout << "], " << amount << ") = " << result << endl;

}

void testWheelCount() {
	{
		vector<int> wheelsOnVehicles = { 2, 4, 18 };
		int amount = 18;
		int result = groupSizeSolutions(wheelsOnVehicles, amount);
		// printing result
		cout << endl << "wheelsOnVehicles([";
		for (int den : wheelsOnVehicles) {
			cout << den << " ";
		}
		cout << "], " << amount << ") = " << result << endl;
	}
	{
		vector<int> wheelsOnVehicles = {18, 2, 4 };
		int amount = 36;
		int result = groupSizeSolutions(wheelsOnVehicles, amount);
		// printing result
		cout << endl << "wheelsOnVehicles([";
		for (int den : wheelsOnVehicles) {
			cout << den << " ";
		}
		cout << "], " << amount << ") = " << result << endl;
	}
}