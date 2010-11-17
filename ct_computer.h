/*
  Copyright (c) 2009, Stefano Maggiolo and Nicola Pagani.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef CT_COMPUTER_H
#define CT_COMPUTER_H

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

class CTComputer
{
 public:
  CTComputer(int g, int n);
  vector< Graph* >& GetAllResults(void);
  map< int, vector< Graph* > >& GetAllResultsByCodimension(void);
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
  map< int, vector< Graph* > > store;

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

  // Helper for updating degrees in the correct way
  int currentDivisionI;
  int currentDivisionJ;

  // Statistics
  vector< int > statistics; // # graph, by codimension
  vector< int > statisticsTime; // in ms, by K
  vector< int > statisticsMemory; // in kB, by K
};

#endif
