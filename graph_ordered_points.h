/*
  Binary format specifications (for small graph, with chars instead of
  ints):

  A graph is composed of:

  Type    | B    | Repetitions   | Meaning
  uchar   | 1    | 1             | K (valid from 1)
  uchar   | 1    | 1             | M (valid from 0)
  uchar   | 1    | K             | g (from 0 to K-1)
  uchar   | 1    | M             | mo (from 0 to M-1)
  uchar   | 1    | K             | l (from 0 to K-1)
  uchar   | 1    | (K-2)*(K-1)/2 | a (row by row)
  Tot: (K^2 + K + 2M + 6) / 2 bytes

  A file is composed of:

  Type    | B    | Repetitions   | Meaning
  uchar   | 1    | 1             | Magic (1)
  Graph   | ...  | #             | the graphs
  uchar   | 1    | 1             | Endoffile (0)
*/

#ifndef GRAPH_ORDERED_POINTS_H
#define GRAPH_ORDERED_POINTS_H

#include <algorithm>
#include <vector>
#include <map>
#include <cstdio>
#include "graph.h"

#ifdef START_LAPACK_COMPUTATION
#include <lapackpp/laslv.h>
#include <cmath>

#define EPSILON 0.00000000001
#endif

#ifdef USE_NAUTY
#define MAXN 32
#include <cassert>
#include "nauty/nauty.h"
#include "nauty/naututil.h"
#endif

using namespace std;

class GraphOrderedPoints : public Graph
{
 public:
  GraphOrderedPoints(int G = -1, int M = -1, int K = -1);
  GraphOrderedPoints(const Graph& graph);
  GraphOrderedPoints(const GraphOrderedPoints& graph);
  GraphOrderedPoints(FILE* f);

  bool Equal(GraphOrderedPoints& g2);
#ifdef USE_NAUTY
  bool EqualNauty(GraphOrderedPoints& g2);
  void ComputeDreadnaut(void);
#endif
  bool EqualPermutations(GraphOrderedPoints& g2);

  // In which component are the marked points.
  vector< int > mo;

  void PrintBinary(FILE* f) const;

 protected:
};

void Graph2GraphOrderedPointsBT(int x,
                                map< int, int >& c,
                                GraphOrderedPoints& h,
                                vector< GraphOrderedPoints >& ret);
vector< GraphOrderedPoints >& Graph2GraphOrderedPoints(Graph& g);

#endif
