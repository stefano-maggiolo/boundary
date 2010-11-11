#ifndef BOUNDARY_COMPUTER_H
#define BOUNDARY_COMPUTER_H

#include "graph.h"
#include "graph_printer.h"
#include "computer.h"

#include <map>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#if HAVE_GETRUSAGE
#include <sys/time.h>
#include <sys/resource.h>
#endif

using namespace std;

class BoundaryComputer: public Computer
{
 public:
  BoundaryComputer(int g, int n);
  vector< Graph* >& GetAllResults(void);
  map< uchar, vector< Graph* > >& GetAllResultsByCodimension(void);
  void Compute(GraphPrinter &printer, enum Statistics stat, int computeOnlyCodim);
  void Compute(GraphPrinter &printer, enum Statistics stat);
  void Compute(GraphPrinter &printer);
  void Statistics(FILE* file);
  void TerseStatistics(FILE* file);

 private:
  //Backtracking functions: g for genera, m for marked points, l for loops, a for edges
  void bt_g(int i);
  void bt_m(int i);
  void bt_l(int i);
  void bt_a(int i, int j);

  // Check if a graph satisfy the rules (visit does a depth first search to ensure the graph is connected, duplicate ensure that the graph is not already in the store).
  bool correct();
  int visit(int i);
  bool duplicate();
  void add_to_store(void);

  int G;
  int M;
  bool computed;

  // Store all graphs for later computation.
  map< uchar, vector< Graph* > > store;
  // Link graphs in a different way
  map< GraphClass, vector< Graph* > > divstore;

  // Counters for checked graphs and graphs that satisfies all the
  // rules; both are subdivided by K.
  vector< int > tried;
  vector< int > found;
  vector< int > duplicated;
  vector< int > unconnected;

  // Used by the depth first search.
  bool v[MAX_K];

  // Current graph
  Graph graph;

  // Compute only this codimension
  int computeOnlyCodim;

  // Statistics
  vector< int > statistics; // # graph, by codimension
  vector< int > statisticsTime; // in ms, by K
  vector< int > statisticsMemory; // in kB, by K

#ifdef BLADEBUG
  // Statistics part 2
  map< pair< vector< int >, pair< vector < int >, vector< int > > >, int > bla_gnl;
  map< pair< vector< int >, vector < int > >, int > bla_gn;
  map< vector< int >, int > bla_g;
#endif
};

#endif
