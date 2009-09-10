#ifndef BOUNDARY_COMPUTER_H
#define BOUNDARY_COMPUTER_H

#include "graph.h"
#include "latex_graph_printer.h"
#include <map>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <ctime>

using namespace std;

class BoundaryComputer
{
 public:
  BoundaryComputer(int g, int n);
  vector< Graph >& GetAllResults(void);
  map< int, vector< Graph > >& GetAllResultsByCodimension(void);
  void SaveAllResults(const char *filename);
  void Compute(bool statisticsOnCerr, int computeOnlyCodim);
  void Compute(bool statisticsOnCerr);
  void Compute();
  void Statistics(FILE* file, int printOnlyCodim);
  void Statistics(FILE* file);
  
 private:
  //Backtracking functions: g for genera, m for marked points, l for loops, a for edges
  void bt_g(int i);
  void bt_m(int i);
  void bt_l(int i);
  void bt_a(int i, int j);

  // Check if a graph satisfy the rules (visit to a depth first search
  // to ensure the graph is connected.
  bool correct();
  int visit(int i);
  bool duplicate();
  void add_to_store(void);

  int G;
  int M;
  bool computed;

  // Store all graphs for later computation.
  map< int, vector< Graph > > store;

  // These are the main tools to ensure we don't generate two isomorphic
  // graphs. Roughly speaking, if d[i] = true, then we could order the
  // terms with index < i and >= i separetely. Note that d[i] is the
  // division *before* the i-th row and column. Moreover, we don't need
  // horizontal and vertical divisions since the adjacency matrix can be
  // permuted only with permutation induces by S_K (i.e., by a
  // permutation of {1,... , K}, not by a generic permutation matrix.
  vector< bool > divisions;

  // Counters for checked graphs and graphs that satisfies all the
  // rules; both are subdivided by K.
  vector< int > tried;
  vector< int > found;
  vector< int > duplicated;
  vector< int > unconnected;

  // Used by the depth first search.
  vector<bool> v ;

  // Current graph
  Graph graph;

  // Compute only this codimension
  int computeOnlyCodim;

  // Print only this codimension
  int printOnlyCodim;

  // Helper for updating degrees in the correct way
  int currentDivisionI;
  int currentDivisionJ;

  // Statistics
  vector< int > statistics;
};

#endif
