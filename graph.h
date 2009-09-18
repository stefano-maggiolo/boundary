#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <vector>
#include <map>
#include <cstdio>
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

class Graph
{
 public:
  Graph(void);
  Graph(int G, int M, int K);
  Graph(const Graph& graph);

  const vector< int >& GetDivisions(void) const;
  bool Equal(Graph& g2);
#ifdef START_LAPACK_COMPUTATION
  bool EqualLapack(Graph& g2);
#endif
#ifdef USE_NAUTY
  bool EqualNauty(Graph& g2);
#endif
  bool EqualPermutations(Graph& g2);
  bool operator==(const Graph& g2);

  // FUNDAMENTAL
  // Arithmetic genus of the curve.
  int G;
  // Number of marked points.
  int M;
  // Number of vertices.
  int K;
  // Geometric genera of the components.
  vector< int > g;
  // Number of marked points on the components.
  vector< int > m;
  // Loops on the components.
  vector< int > l;
  // Adjacency matrix for the graph.
  vector< vector < int > > a;

  // DERIVED
  // The current value of the summation SUM(g[i]) + SUM(a[i][j]) - K + 1
  // that eventually has to be G.
  int sum;
  // Marked points already placed.
  int msum;
  // How many connections already placed in the graph.
  int connections;
  // How many edges connecting the vertices.
  vector< int > edges;
  // How many genus 0 components are there
  int p1;
  // Total number of edges
  int total_edges;
  // If it is rational tail
  bool is_rational_tail;
  // If it is compact type
  bool is_compact_type;
#ifdef USE_DEGREES_MAP
  // Counting sort of the number of edges between two nodes,
  // splitted by simple_divisions
  vector< vector< map< int, int > > > degrees;
#endif
#ifdef USE_DEGREES_NO_MAP
  // Counting sort of the number of edges between two nodes,
  // splitted by simple_divisions
  vector< vector< int > > aSortedDiv;
#endif
#ifdef USE_LINES_MAP
  vector< map< map< int, int >, int > > lines;
#endif
#ifdef USE_LINES_NO_MAP
  vector< vector < int > > aSorted;
#endif
#ifdef START_LAPACK_COMPUTATION
  LaVectorDouble eigenvalues;
  LaGenMatDouble laMatrix;
#endif
#ifdef USE_NAUTY
  vector< vector< int > > aSimple;
  vector< int > partitions;
#endif
  void normal_print(void) const;
  void print_matrix(void) const;
#ifdef USE_LINES_NO_MAP
  void print_matrix_sorted(void) const;
#endif
  void pretty_print(int d, int r, const vector< bool >& divis,
                    int start, int end) const;

  void ComputeDivisions(void);
#ifdef START_LAPACK_COMPUTATION
  void ComputeEigenvalues(void);
  bool PermutationOk(Graph& g2, vector< int >& perm);
#endif
#ifdef USE_NAUTY
  void print_matrix_simple(void) const;
  void ComputeDreadnaut(void);
  bool aresame(graph* g1, graph* g2, int n_n, int n_m);
#endif
  vector< int > simple_divisions;

  
 private:
  bool NextSpecialPerm(vector< int >& perm);
};

class GraphCollection
{
 public:
  GraphCollection(vector< Graph >);
  vector< Graph > GetAllGraphs(void);
  vector< Graph > GetGraphsOfCodimension(int c);
  vector< Graph > GetRTGraphs(int c);
  vector< Graph > GetRTGraphsOfCodimension(int c);
  vector< Graph > GetCTGraphs(int c);
  vector< Graph > GetCTGraphsOfCodimension(int c);

 private:
  map< int, vector< Graph > > graphs;
};

#endif
