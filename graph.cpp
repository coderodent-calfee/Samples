#include <cmath>
#include <matplot/matplot.h>
#include <type_traits>
#include <numeric>
#include <iterator> //for std::ostream_iterator
#include <algorithm> //for std::copy
#include <iostream> //for std::cout

#include "FiniteStateMachine.h"

std::vector<std::pair<size_t, size_t>> get_edges();

int doGraphOld() {
   using namespace matplot;
   std::vector<std::pair<size_t, size_t>> edges = {
       {0, 1}, {0, 2}, {0, 9},  {1, 3},  {1, 11}, {2, 3},
       {2, 4}, {3, 5}, {4, 5},  {4, 6},  {5, 8},  {6, 7},
       {6, 9}, {7, 8}, {7, 10}, {8, 11}, {9, 10}, {10, 11} };
   auto g = digraph(edges);
   g->marker("o");
   g->node_color("yellow");
   g->marker_size(20);
   g->line_style("-");
   auto axes = g->parent();
   //   axes->text({ 2 }, { 3 }, { "foo" }); just makes it dissapear
   //axes->text({ 0 }, { 0}, { "foo" }); //just makes it dissapear

   g->x_data({ 2, 4, 1.5, 3.5, 1, 3, 1, 2.1, 3, 2, 3.1, 4 });
   g->y_data({ 3, 3, 3.5, 3.5, 4, 4, 2, 2, 2, 1, 1, 1 });
   g->node_labels({ "foo" });
   show();
   return 0;
//   auto edges = get_edges();
//   
////   auto axes = graph(edges, "-:gs");
//   auto axes = graph(edges, "-.oy");
//   axes->marker_size(30.0);
//
//   // - solid line
//   // . gnuplot linetype 7
//   // d diamond, o circle 
//   // r 
//
//   axes->show_labels(true);   
//   
//
//   show();
//   return 0;
}

std::vector<std::pair<size_t, size_t>> get_edges() {
   return {
       {0, 1},   {0, 4},   {0, 5},   {1, 2},   {1, 10},  {2, 3},   {2, 15},
       {3, 4},   {3, 20},  {4, 25},  {5, 6},   {5, 9},   {6, 7},   {6, 29},
       {7, 8},   {7, 41},  {8, 9},   {8, 37},  {9, 11},  {10, 11}, {10, 14},
       {11, 12}, {12, 13}, {12, 36}, {13, 14}, {13, 32}, {14, 16}, {15, 16},
       {15, 19}, {16, 17}, {17, 18}, {17, 31}, {18, 19}, {18, 52}, {19, 21},
       {20, 21}, {20, 24}, {21, 22}, {22, 23}, {22, 51}, {23, 24}, {23, 47},
       {24, 26}, {25, 26}, {25, 29}, {26, 27}, {27, 28}, {27, 46}, {28, 29},
       {28, 42}, {30, 31}, {30, 34}, {30, 53}, {31, 32}, {32, 33}, {33, 34},
       {33, 35}, {34, 55}, {35, 36}, {35, 39}, {36, 37}, {37, 38}, {38, 39},
       {38, 40}, {39, 56}, {40, 41}, {40, 44}, {41, 42}, {42, 43}, {43, 44},
       {43, 45}, {44, 57}, {45, 46}, {45, 49}, {46, 47}, {47, 48}, {48, 49},
       {48, 50}, {49, 58}, {50, 51}, {50, 54}, {51, 52}, {52, 53}, {53, 54},
       {54, 59}, {55, 56}, {55, 59}, {56, 57}, {57, 58}, {58, 59} };
}

#define ENTERED_COLOR 2.0
#define NEUTRAL_COLOR 1.0
#define EXITED_COLOR 0.0

void FSMGraphAdapter::setEnteredState(const std::string& state) {

   auto stateToIndex = [&](const std::string& stateName) { return std::find(nodeNames_.begin(), nodeNames_.end(), stateName) - nodeNames_.begin(); };
   currentState_ = (int)stateToIndex(state);
   colors_[currentState_] = ENTERED_COLOR;
   graph_->marker_colors(colors_);
   markerSizes_[currentState_] = 5.0;
   graph_->marker_sizes(markerSizes_);

}

void FSMGraphAdapter::setExitedState(const std::string& state) {
   
   if (previousState_ != -1) {
      colors_[previousState_] = NEUTRAL_COLOR;
      markerSizes_[previousState_] = 3.0;
   }

   previousState_ = currentState_;
   colors_[previousState_] = EXITED_COLOR;
   graph_->marker_colors(colors_);
   markerSizes_[previousState_] = 4.0;
   graph_->marker_sizes(markerSizes_);

}

void FSMGraphAdapter::setTransitionEdge(int edgeIndex) {

   std::vector<std::string> activeEdges(edgeNames_.size());
   std::generate(activeEdges.begin(), activeEdges.end(), []() { return std::string(""); });
   activeEdges[edgeIndex] = edgeNames_[edgeIndex];
   graph_->edge_labels(activeEdges); // only show edge name when transition was active

}



FSMGraphAdapter::FSMGraphAdapter(const FSMContextPtr& ctx) : ctx_(ctx) {
   using namespace matplot;

   // todo: get edges from ctx
   auto[ edges, edgeNames, nodeNames] = ctx->getEdgesWithNames();
   
   edgeNames_ = edgeNames;
   nodeNames_ = nodeNames;
   auto stateToIndex = [&](const std::string& stateName) { return std::find(nodeNames_.begin(), nodeNames_.end(), stateName) - nodeNames_.begin(); };

   colors_ = std::vector<double>(nodeNames_.size());

   currentState_ = (int)stateToIndex(ctx->getState());
   transition_ = previousState_ = -1;

   graph_ = digraph(edges);
   graph_->node_labels(nodeNames_);// array of labels for the nodes

   //std::cout << "nodes:" << std::endl;
   //std::copy(nodeNames.begin(), nodeNames.end(), std::ostream_iterator<std::string>(std::cout, " "));
   //std::cout << std::endl;

   graph_->line_style("-");
   graph_->marker("o");

   auto lineSpec = graph_->line_spec();

   std::fill_n(colors_.begin(), nodeNames_.size(), NEUTRAL_COLOR);
   colors_[currentState_] = ENTERED_COLOR;
   graph_->marker_colors(colors_);

   markerSizes_ = std::vector<float>(nodeNames_.size());
   std::generate(markerSizes_.begin(), markerSizes_.end(), []() { return 3.0f; });
   markerSizes_[currentState_] = 5.0;
   graph_->marker_sizes(markerSizes_);
   auto marker_sizes = graph_->marker_sizes();

   //std::vector<double> lineWidths(edgeNames_.size());
   //std::generate(lineWidths.begin(), lineWidths.end(), []() { return 10.0; });
   //lineWidths[1] = 30.0;
   //g->line_widths(lineWidths);


   entryObserver_ = ctx->getProperty("FiniteStateMachine::EnterState")->getObserver();
   entryObserver_->setUpdate([=](const MessageType& m) { setEnteredState(m.get<std::string>()); });

   exitObserver_ = ctx->getProperty("FiniteStateMachine::ExitState")->getObserver();
   exitObserver_->setUpdate([=](const MessageType& m) { setExitedState(m.get<std::string>()); });

   edgeObserver_ = ctx->getProperty("FiniteStateMachine::Transition")->getObserver();
   edgeObserver_->setUpdate([=](const MessageType& m) { setTransitionEdge(m.get<int>()); });
}
