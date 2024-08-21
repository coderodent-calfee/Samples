#include "SimpleZoneQueueManager.h"
#include "test.h"
#include <iostream>

//static std::deque<PlayerDataPtr> players = InitPlayerData();
static std::shared_ptr<SimpleZoneQueueManager> zoneQueueMgr = std::make_shared<SimpleZoneQueueManager>();


int main()
{
   //testSimpleZoneQueueManager();
   //testPriceDivergenceMonitor();

   //testFiniteStateMachine();
   //testAirlineSeatPricer();
   //testIslandExplorer();
   //testIslandExplorerNoRecurse();

   //testCycleChecker();
   //testTasker();

//   testSegmentString();
   //testCoinChange();
   //testWheelCount();
   //doGraph();
   doVolunteerJobs();
}
