#pragma once

#include <deque>

void doVolunteerJobs();
int doGraph();
unsigned roll(unsigned max);

void testFiniteStateMachine();
void testIslandExplorer();
void testIslandExplorerNoRecurse();
void testCycleChecker();
void testTasker();
void  testWheelCount();
void    testSegmentString();
void testCoinChange();

#include "SimpleZoneQueueManager.h"

void utilMakeZoneQueues(std::shared_ptr<SimpleZoneQueueManager>& zoneQueueMgr);
void utilAddPlayersToAllZones(std::shared_ptr<SimpleZoneQueueManager>& zoneQueueMgr, std::deque<const PlayerDataPtr>& players);

void testPriceDivergenceMonitor();

void testPriceDivergenceMonitorRegisterPair();
void testPriceDivergenceMonitorUpdatePrice();
void testPriceDivergenceMonitorReportDivergence();


void testSimpleZoneQueueManager();

void testPlayerClassEnum();
void testInitialZones();
void testAddPlayersToZoneQueues();
void testRemovingPlayerFromZone();

void testNinjaZones();
void testCompleteZone();

void testReadyZones();

std::deque<PlayerDataPtr> InitRandomPlayerData(int numPlayers);
PlayerDataPtr MakePlayerData(std::string name, PlayerIdType playerID, PlayerClassEnum clazz, unsigned int level);
std::shared_ptr<PlayerData> RandomPlayer();
void dumpPlayerData();
void CheckZoneData(const ZoneQueuePtr& zone);
std::deque<PlayerDataPtr> InitPlayerData();
void DisplayPlayerData(const std::deque<PlayerDataPtr>& players);
std::shared_ptr<PlayerData> RandomPlayer();
std::deque<PlayerDataPtr> InitRandomPlayerData(int numPlayers);
std::deque<PlayerDataPtr> InitPlayerData();
