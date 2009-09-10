#include "graph.h"

using namespace std;

//==== NOT USED ======================================================

void
Graph::print_matrix(void) const
{
  for (int i = 0; i < a.size(); i++)
    {
      fprintf(stderr, "    ");
      for (int j = 0; j < a[i].size(); j++)
        fprintf(stderr, "%d ", a[i][j]);
      fprintf(stderr, "\n");
    }
}

#ifdef USE_LINES_NO_MAP
void
Graph::print_matrix_sorted(void) const
{
  for (int i = 0; i < aSorted.size(); i++)
    {
      fprintf(stderr, "    ");
      for (int j = 0; j < aSorted[i].size(); j++)
        fprintf(stderr, "%d ", aSorted[i][j]);
      fprintf(stderr, "\n");
    }
}
#endif

void
Graph::normal_print(void) const
{
  int c = 0;
  for (int i = 0; i < K; i++)
    for (int j = i; j < K; j++)
      c += a[i][j];
  fprintf(stderr, "Genus: %d,\n", G);
  fprintf(stderr, "  codimension: %d,\n", c);
  fprintf(stderr, "  irreducible components: %d,\n", K);
  fprintf(stderr, "  with genus ");
  for (int i = 0; i < K; i++)
    fprintf(stderr, "%d ", g[i]);
  fprintf(stderr, "\n");
  fprintf(stderr, "  with marked points ");
  for (int i = 0; i < K; i++)
    fprintf(stderr, "%d ", m[i]);
  fprintf(stderr, "\n");
  fprintf(stderr, "  with loops ");
  for (int i = 0; i < K; i++)
    fprintf(stderr, "%d ", l[i]);
  fprintf(stderr, "\n");
  fprintf(stderr, "  with divisions ");
  for (int i = 0; i < simple_divisions.size(); i++)
    fprintf(stderr, "%d ", simple_divisions[i]);
  fprintf(stderr, "\n");
  fprintf(stderr, "  with adjacency matrix\n");
  print_matrix();
  fprintf(stderr, "\n");
}

void
Graph::pretty_print(int d, int r, const vector< bool >& divis,
             int start, int end) const
{
  fprintf(stderr, "Diagonale = %d, Riga = %d\n", d, r);
  fprintf(stderr, "Prossimo numero tra %d e %d.\n", start, end);
  for (int j = 0; j < K; j++)
    fprintf(stderr, "  %d  ", g[j]);
  fprintf(stderr, "\n");

  for (int i = 0; i < K; i++)
    {
      if (divis[i])
        for (int j = 0; j < K; j++)
          if (divis[j])
            fprintf(stderr, "+-");
          else
            fprintf(stderr, "--");
      else
        for (int j = 0; j < K; j++)
          if (divis[j])
            fprintf(stderr, "| ");
          else
            fprintf(stderr, "  ");
      fprintf(stderr, "\n");

      for (int j = 0; j < K; j++)
        if (divis[j])
          fprintf(stderr, "|%d", a[i][j]);
        else
          fprintf(stderr, " %d", a[i][j]);
      fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}

Graph::Graph()
{
}

Graph::Graph(int g, int n, int k)
{
  this->G = g;
  this->M = n;
  this->K = k;
}

Graph::Graph(const Graph& graph)
{
  G = graph.G;
  M = graph.M;
  K = graph.K;  

  g = graph.g;
  m = graph.m;
  l = graph.l;
  a = graph.a;

  edges = graph.edges;
  total_edges = graph.total_edges;
  p1 = graph.p1;
  sum = graph.sum;
  msum = graph.msum;
  connections = graph.connections;
  is_rational_tail = graph.is_rational_tail;
  is_compact_type = graph.is_compact_type;

  simple_divisions = graph.simple_divisions;
#ifdef USE_DEGREES_MAP
  degrees = graph.degrees;
#endif
#ifdef USE_DEGREES_NO_MAP
  aSortedDiv = graph.aSortedDiv;
#endif
#ifdef USE_LINES_MAP
  lines = graph.lines;
#endif
#ifdef USE_LINES_NO_MAP
  aSorted = graph.aSorted;
#endif
#ifdef START_LAPACK_COMPUTATION
  laMatrix = graph.laMatrix;
  eigenvalues = graph.eigenvalues;
#endif
}

void
Graph::ComputeDivisions(void)
{
  simple_divisions.clear();
  simple_divisions.push_back(0);
  for (int i = 1; i < K; ++i)
    {
      if (g[i-1] != g[i] || 
          m[i-1] != m[i] ||
          l[i-1] != l[i])
        simple_divisions.push_back(i);
    }
}

#ifdef START_LAPACK_COMPUTATION
void
Graph::ComputeEigenvalues(void)
{
  if (K >= START_LAPACK_COMPUTATION)
    {
      laMatrix.resize(K, K);
      eigenvalues.resize(K);
      LaVectorDouble zero(K);
      for (int itI = 0; itI < K; ++itI)
        for (int itJ = 0; itJ < K; ++itJ)
          laMatrix(itI, itJ) = a[itI][itJ];
      LaEigSolve(laMatrix, eigenvalues, zero, laMatrix);
    }
}
#endif

inline bool
Graph::NextSpecialPerm(vector< int >&perm)
{
  vector< int >::iterator i = simple_divisions.end() - 1;
  vector< int >::iterator b = perm.end(), e;
  do
    {
      e = b;
      b = perm.begin() + *i;
      if (next_permutation(b, e)) return true;
      --i;
    } while (i >= simple_divisions.begin());
  return false;
}

bool
Graph::Equal(Graph& g2)
{
  // These tests should be here, but we put it out because of efficiency reasons.
  /*
  if (K != g2.K)
    {
      return false;
    }
  if (simple_divisions != g2.simple_divisions)
    {
      return false;
    }
  if (g != g2.g || m != g2.m || l != g2.l)
    {
      return false;
    }
#ifdef USE_DEGREES_MAP
  if (degrees != g2.degrees)
    {
      return false;
    }
#endif
#ifdef USE_DEGREES_NO_MAP
  if (aSortedDiv != g2.aSortedDiv)
    {
      return false;
    }
#endif
#ifdef USE_LINES_MAP
  if (lines != g2.lines)
    {
      return false;
    }
#endif
#ifdef USE_LINES_NO_MAP
  if (aSorted != g2.aSorted)
    {
      return false;
    }
#endif
  */

#ifdef START_LAPACK_COMPUTATION
  if (K < START_LAPACK_COMPUTATION) return EqualPermutations(g2);
  else return EqualLapack(g2);
#else
  return EqualPermutations(g2);
#endif
}

#ifdef START_LAPACK_COMPUTATION
bool
Graph::EqualLapack(Graph& g2)
{
  // If the sorted vector of eigenvalues differ, the graphs are not
  // isomorphic.
  vector< bool > taken(K, false);
  for (int i = 0; i < K; ++i)
    {
      int j;
      for (j = 0; j < K; ++j)
        if (!taken[j] && fabs(eigenvalues(i) - g2.eigenvalues(j)) < EPSILON)
          {
            taken[j] = true;
            break;
          }
      if (j==K) return false;
    }

  // Find a eigenvalue with one-dimensional eigenspace and with all
  // coefficients of the eigenvector distinct.
  int column, column2;
  bool goodColumn;
  for (column = 0; column < K; ++column)
    {
      bool goodEigenvalue = true;
      for (int j = 0; j < K; ++j)
        if (column != j && fabs(eigenvalues(column) - eigenvalues(j)) < EPSILON)
          {
            goodEigenvalue = false;
            break;
          }
      if (goodEigenvalue)
        {
          // Finding columns relative to a good eigenvalue with all
          // different values. TODO: can we test here also for
          // equality of the sorted vectors of coefficient of the
          // eigenvectors?
          goodColumn = true;
          for (int i = 0; i < K; ++i)
            {
              for (int j = i+1; j < K; ++j)
                if (fabs(laMatrix(i, column) - laMatrix(j, column)) < EPSILON)
                  {
                    goodColumn = false;
                    break;
                  }
              if (!goodColumn) break;
            }
          if (goodColumn)
            break;
        }
    }
  // If there are no such eigenvalues, we use the usual method.
  if (!goodColumn) return EqualPermutations(g2);

  // Find the good eigenvalue in the spectrum of g2
  int index2 = -1;
  for (int i = 0; i < K; ++i)
    if (fabs(eigenvalues(column) - g2.eigenvalues(i)) < EPSILON)
      {
        index2 = i;
        break;
      }
  // This should not happen (we tested for equality before...)
  if (index2 == -1) return false;

  vector< int > permutation1(K, -1);
  vector< int > permutation2(K, -1);
  vector< bool > taken1(K, false);
  vector< bool > taken2(K, false);
  bool ok1 = true;
  bool ok2 = true;

  // First case: permutation of the eigenvectors' coefficients if the
  // two are with the same signs.
  for (int j = 0; j < K; ++j)
    for (int i = 0; i < K; ++i)
      if (!taken1[i] && fabs(laMatrix(j,column) - g2.laMatrix(i, index2)) < EPSILON)
        {
          permutation1[j] = i;
          taken1[i] = true;
          break;
        }
  // We test that we got indeed a permutation.
  for (int j = 0; j < K; ++j)
    if (permutation1[j] == -1)
      {
        ok1 = false;
        break;
      }

  // Second case: if the two are of different signs.
  for (int j = 0; j < K; ++j)
    for (int i = 0; i < K; ++i)
      if (!taken2[i] && fabs(laMatrix(j,column) + g2.laMatrix(i, index2)) < EPSILON)
        {
          permutation2[j] = i;
          taken2[i] = true;
          break;
        }
  // We test that we got indeed a permutation.
  for (int j = 0; j < K; ++j)
    if (permutation2[j] == -1)
      {
        ok2 = false;
        break;
      }

  if (ok1 && PermutationOk(g2, permutation1)) return true; 
  else if (ok2 && PermutationOk(g2, permutation2)) return true;
  else return false;
}

bool
Graph::PermutationOk(Graph& g2, vector< int >& perm)
{
  for (int i = 0; i < K; ++i)
    {
      if (g2.g[perm[i]] != g[i] ||
          g2.m[perm[i]] != m[i] || 
          g2.l[perm[i]] != l[i])
        return false;
      
      for (int j = i+1; j < K; ++j)
        if (g2.a[perm[i]][perm[j]] != a[i][j])
          return false;
    }
  return true;
}
#endif

bool
Graph::EqualPermutations(Graph& g2)
{
  vector< int > perm;
  for (int i = 0; i < g2.K; i++)
    perm.push_back(i);
      
  // Sure they're not trivially isomorphic, so we run a
  // NextSpecialPerm in any case.
  while(NextSpecialPerm(perm))
    {
      bool different = false;
      for (int i = 0; i < K; ++i)
        {
          for (int j = i+1; j < K; ++j)
            if (a[perm[i]][perm[j]] != g2.a[i][j])
              {
                different = true;
                break;
              }
          if (different) break;
        }
      if (!different) return true;
    }
    return false;
}
