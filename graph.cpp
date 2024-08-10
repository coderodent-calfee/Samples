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




FSMGraphAdapter::FSMGraphAdapter(const FSMContextPtr& ctx) {
   using namespace matplot;

   // todo: get edges from ctx
   auto[ edges, edgeNames, nodeNames] = ctx->getEdgesWithNames();
   
   auto stateToIndex = [&nodeNames](const std::string& stateName) { return std::find(nodeNames.begin(), nodeNames.end(), stateName) - nodeNames.begin(); };

   edgeNames_ = edgeNames;
   nodeNames_ = nodeNames;
   currentState_ = (int)stateToIndex(ctx->getState());

   auto g = digraph(edges);
   graph_ = g;
   g->node_labels(nodeNames);// array of labels for the nodes

   std::cout << "nodes:" << std::endl;
   std::copy(nodeNames.begin(), nodeNames.end(), std::ostream_iterator<std::string>(std::cout, " "));
   std::cout << std::endl;

   g->line_style("-");
   g->marker("o");
   g->marker_size(20);
   g->node_color("yellow");

   auto lineSpec = g->line_spec();


   std::vector<double> colors(nodeNames.size());
   std::fill_n(colors.begin(), nodeNames.size(), 1.0);
   colors[currentState_] = 2.0;
   g->marker_colors(colors);
   std::cout << "colors:" << std::endl;
   std::copy(colors.begin(), colors.end(), std::ostream_iterator<double>(std::cout, " "));
   std::cout << std::endl;

   std::vector<float> markerSizes(nodeNames.size());
   std::generate(markerSizes.begin(), markerSizes.end(), []() { return 3.0f; });
   markerSizes[currentState_] = 5.0;
   g->marker_sizes(markerSizes);
   auto marker_sizes = g->marker_sizes();


   //std::vector<double> lineWidths(edgeNames_.size());
   //std::generate(lineWidths.begin(), lineWidths.end(), []() { return 10.0; });
   //lineWidths[1] = 30.0;
   //g->line_widths(lineWidths);

   std::vector<std::string> edgesA(edgeNames_.size());

   std::generate(edgesA.begin(), edgesA.end(), []() { return std::string(""); });
   edgesA[1] = edgeNames_[1];
   g->edge_labels(edgesA); // only show edge name when transition was active


}
