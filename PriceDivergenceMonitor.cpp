#include "PriceDivergenceMonitor.h"

using namespace std;
void defaultReportCallback(const std::string& updatedStockName, int
   updatedStockPrice, const std::string& otherStockName, int otherStockPrice) {};

PriceDivergenceMonitor::PriceDivergenceMonitor(int threshold)
{
   // todo: complete this constructor as needed
   _threshold = threshold;
   _reportCallback = &_default;
}
/*
The method RegisterPair will be called by the owner of this class each time
it wants your class to start monitoring a new pair of correlated stocks.
*/
void PriceDivergenceMonitor::RegisterPair(const string& stockOne, const
   string& stockTwo)
{
   if (stockOne.compare(stockTwo) > 0) {
      _stockPairs.insert(std::pair<std::string, std::string>(stockTwo, stockOne));
   }
   else {
      _stockPairs.insert(std::pair<std::string, std::string>(stockOne, stockTwo));
   }
}


void PriceDivergenceMonitor::ReportDivergence(const std::string& updatedStockName, int
   updatedStockPrice, const std::string& otherStockName, int otherStockPrice) {
   (*_reportCallback)(updatedStockName, updatedStockPrice, otherStockName, otherStockPrice);
}

void PriceDivergenceMonitor::TestAndReportDivergence(const string& updatedStockName, int
   updatedStockPrice, const string& otherStockName, int otherStockPrice) {
   if (abs(updatedStockPrice - otherStockPrice) > _threshold) {
      ReportDivergence(updatedStockName, updatedStockPrice, otherStockName, otherStockPrice);
   }
}

/*
The method UpdatePrice will be called by the owner of this class whenever
the price of a stock changes. When the price of a stock that is part of a
registered pair changes, you should check whether that new price differs
from the price of the other stock in the pair by more than the threshold. If
it does, call the ReportDivergence method with the appropriate parameters.
*/
void PriceDivergenceMonitor::UpdatePrice(const string& stockName, int
   newPrice)
{
   auto oldPrice = _stocks[stockName]; // will return 0 if the stock does not exist, and that behavior will be wrong if the input is 0
   if (newPrice != oldPrice) {
      _stocks[stockName] = newPrice;
      for (auto& pair : _stockPairs) {
         if (!pair.first.compare(stockName)) {
            TestAndReportDivergence(stockName, newPrice, pair.second, _stocks[pair.second]);
         }
         else if(!pair.second.compare(stockName)) {
            TestAndReportDivergence(stockName, newPrice, pair.first, _stocks[pair.first]);
         }
      }
   }
}
int timeStamp = 0;