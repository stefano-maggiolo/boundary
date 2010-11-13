/*
  Binary format specifications (for small graph, with chars instead of
  ints):

  A graph is composed of:

  Type    | B    | Repetitions   | Meaning
  uchar   | 1    | 1             | K (valid from 1)
  uchar   | 1    | K             | g (from 0 to K-1)
  uchar   | 1    | K             | m (from 0 to K-1)
  uchar   | 1    | K             | l (from 0 to K-1)
  uchar   | 1    | (K-2)*(K-1)/2 | a (row by row)
  Tot: (K^2 + 3 K + 4) / 2 bytes

  A file is composed of:

  Type    | B    | Repetitions   | Meaning
  uchar   | 1    | 1             | Magic (0)
  Graph   | ...  | #             | the graphs
  uchar   | 1    | 1             | Endoffile (0)
*/

#ifndef GRAPH_H
#define GRAPH_H

#include "constants.h"
#include <cstdio>
#include <cassert>

#define MAXN 32
#include "nauty/nauty.h"
#include "nauty/naututil.h"

using namespace std;

class Graph
{
 public:
  Graph(int g = -1, int n = -1, int k = -1);
  Graph(const Graph& graph);
  Graph(FILE* f); // Binary constructor

  bool Equal(Graph& g2);
  void Clear(uchar K);
  bool EqualNauty(Graph& g2);

  // FUNDAMENTAL
  // Arithmetic genus of the curve.
  int G;
  // Number of marked points.
  int M;
  // Number of vertices.
  int K;
  // Geometric genera of the components.
  uchar g[MAX_K];
  // Number of marked points on the components.
  uchar m[MAX_K];
  // Loops on the components.
  uchar l[MAX_K];
  // Adjacency matrix for the graph.
  uchar a[MAX_K][MAX_K];

  // DERIVED
  // The current value of the summation SUM(g[i]) + SUM(a[i][j]) - K + 1
  // that eventually has to be G.
  uchar sum;
  // Marked points already placed.
  uchar msum;
  // How many connections already placed in the graph.
  uchar connections;
  // How many genus 0 components are there
  uchar p1;
  // Stabilizing (m_C <= 2) marked points on genus 0 components
  uchar stab_he_2;
  // Stabilizing (m_C <= 3) marked points on genus 0 components
  uchar stab_he_3;
  // Total # of half edges on the i-th genus 0 component
  uchar he[MAX_K];
  // Min # of half edges on one of the first i components
  uchar min_he_2;
  // Total number of edges
  uchar total_edges;
  // If it is rational tail
  bool is_rational_tail;
  // If it is compact type
  bool is_compact_type;
  // Counting sort of edges multiplicities
  uchar gDegrees[MAX_DEG];
  // These are the main tools to ensure we don't generate two isomorphic
  // graphs. Roughly speaking, if d[i] = true, then we could order the
  // terms with index < i and >= i separetely. Note that d[i] is the
  // division *before* the i-th row and column. Moreover, we don't need
  // horizontal and vertical divisions since the adjacency matrix can be
  // permuted only with permutation induces by S_K (i.e., by a
  // permutation of {1,... , K}, not by a generic permutation matrix.
  unsigned int divisions;//[MAX_K+1];

  int nautyK, nautyM;
  graph nautyGraph[MAXN*MAXM];

  void PrintNormal(FILE* f = stderr) const;
  void PrintBinary(FILE* f) const;
  void PrintLaTeX(FILE* f = stdout) const;
  void PrintMatrix(FILE* f = stderr) const;

  void PrintMatrixSimple(FILE* f, graph* g) const;
  void ComputeDreadnaut(void);
};

class GraphClass
{
 public:
  int G;
  int M;
  int K;
  uchar g[MAX_K];
  uchar m[MAX_K];
  uchar l[MAX_K];
  uchar gDegrees[MAX_DEG];

  GraphClass(const Graph& o);
  bool operator<(const GraphClass& o) const;
};

#endif
