#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <iostream>
using namespace std;


struct pair_hash
{
   template <class T1, class T2>
   std::size_t operator () (std::pair<T1, T2> const& pair) const
   {
      std::size_t h1 = std::hash<T1>()(pair.first);
      std::size_t h2 = std::hash<T2>()(pair.second);
      auto h3 = h1 ^ h2;
//      cout << pair.first << " " << pair.second << " " << h1 << " " << h2 << " = " << h3 << endl;

      return h3;
   }
};

struct report_callback
{
   virtual void operator () (const std::string& updatedStockName, int
      updatedStockPrice, const std::string& otherStockName, int otherStockPrice)
   {
   }
};

typedef std::unordered_map<std::string, int> Stocks;
typedef std::shared_ptr<Stocks> StocksPtr;
typedef std::unordered_set<std::pair<std::string, std::string>, pair_hash> StockPairsSet;
typedef std::shared_ptr<StockPairsSet> StockPairsSetPtr;


class PriceDivergenceMonitor
{
public:
   PriceDivergenceMonitor(int threshold);
   void RegisterPair(const std::string & stockOne, const std::string& stockTwo);
   void UpdatePrice(const std::string& stockName, int newPrice);
   StockPairsSetPtr getStockPairs() {
      return std::make_shared<StockPairsSet>(_stockPairs);
   }
   StocksPtr getStocks() {
      return std::make_shared<Stocks>(_stocks);
   }
   void SetReportCallback(report_callback *cb) { _reportCallback = cb; };
private:
   report_callback* _reportCallback;
   report_callback _default;
   void ReportDivergence(const std::string& updatedStockName, int
      updatedStockPrice, const std::string& otherStockName, int otherStockPrice);
   void TestAndReportDivergence(const std::string& updatedStockName, int
      updatedStockPrice, const std::string& otherStockName, int otherStockPrice);

   int _threshold;
   std::unordered_map<std::string, int> _stocks;
   std::unordered_set<std::pair<std::string, std::string>, pair_hash> _stockPairs;
};

