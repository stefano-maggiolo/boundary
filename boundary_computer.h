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
  void Compute(GraphPrinter &graph_printer, enum Statistics stat, int computeOnlyCodim);
  void Compute(GraphPrinter &graph_printer, enum Statistics stat);
  void Compute(GraphPrinter &graph_printer);
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
  void empty_store_to_statistics(void);

  GraphPrinter *printer;
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

#ifdef DEBUG_GNL_COUNT
  // Statistics part 2
  map< pair< vector< int >, pair< vector < int >, vector< int > > >, int > count_gnl;
  map< pair< vector< int >, vector < int > >, int > count_gn;
  map< vector< int >, int > count_g;
#endif
};

#endif
