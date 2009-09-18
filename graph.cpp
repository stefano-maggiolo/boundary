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

#ifdef USE_NAUTY
void
Graph::print_matrix_simple(void) const
{
  for (int i = 0; i < aSimple.size(); i++)
    {
      fprintf(stderr, "    ");
      for (int j = 0; j < aSimple[i].size(); j++)
        fprintf(stderr, "%d ", aSimple[i][j]);
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
#ifdef USE_NAUTY
  aSimple = graph.aSimple;
  partitions = graph.partitions;
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
#ifdef USE_NAUTY
  return EqualNauty(g2);
#endif
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

#ifdef USE_NAUTY
void
Graph::ComputeDreadnaut(void)
{
  vector< int > translate(K, -1);
  vector< vector< int > > translateEdges(K, vector< int >(K, -1));
  
  int maxg=0, maxm=0, maxl=0, maxa=0;
  int simpleK = 0;
  int npart;
  
  for (int i = 0; i < K; ++i)
    {
      if (maxg < g[i]) maxg = g[i];
      if (maxm < m[i]) maxm = m[i];
      if (maxl < l[i]) maxl = l[i];
      for (int j = i+1; j < K; ++j)
        if (maxa < a[i][j]) maxa = a[i][j];
    }
  // Resulting simple graph of simpleK >= (maxg+1)(maxm+1)(maxa+1) +
  // maxa-2 and that number of partitions
  npart = (maxg+1)*(maxm+1)*(maxl+1) + (maxa-1);

  partitions.assign(1, -1);
  // Build translation table for vertices and edges
  for (int ig = 0; ig <= maxg; ++ig)
    for (int im = 0; im <= maxm; ++im)
      for (int il = 0; il <= maxl; ++il)
        {
          bool found = false;
          for (int i = 0; i < K; ++i)
            if (g[i] == ig && m[i] == im && l[i] == il)
              {
                found = true;
                translate[i] = simpleK;
                ++simpleK;
              }
          // Add a fake vertex to preserve colouring
          if (!found) ++simpleK;
          partitions.push_back(simpleK-1);
        }
  for (int ia = 2; ia <= maxa; ++ia)
    {
      bool found = false;
      for (int i = 0; i < K; ++i)
        for (int j = i+1; j < K; ++j)
          if (ia == a[i][j])
            {
              found = true;
              translateEdges[i][j] = simpleK;
              ++simpleK;
            }
      if (!found) ++simpleK;
      partitions.push_back(simpleK-1);
    }
  aSimple.assign(simpleK, vector< int >(simpleK, 0));
  
  for (int i = 0; i < K; ++i)
    for (int j = i+1; j < K; ++j)
      if (a[i][j] == 1)
        {
          aSimple[translate[i]][translate[j]] = 1;
          aSimple[translate[j]][translate[i]] = 1;
        }
      else if (a[i][j] > 1)
        {
          aSimple[translate[i]][translateEdges[i][j]] = 1;
          aSimple[translateEdges[i][j]][translate[i]] = 1;
          
          aSimple[translate[j]][translateEdges[i][j]] = 1;
          aSimple[translateEdges[i][j]][translate[j]] = 1;
        }
}

bool
Graph::aresame(graph* g1, graph* g2, int n_n, int n_m)
{
  for (int i = 0; i < n_n; ++i)
    {
      set* gv1 = GRAPHROW(g1, i, n_m);
      set* gv2 = GRAPHROW(g2, i, n_m);

      int intersection = setinter(gv1, gv2, n_m);
      if (intersection != setsize(gv1, n_m) || intersection != setsize(gv2, n_m))
        return false;
    }
  return true;
}

bool
Graph::EqualNauty(Graph& g2)
{
  // Assume that we already called ComputeDreadnaut
  
  graph n_g1[MAXN*MAXM];
  graph n_g2[MAXN*MAXM];
  int n_lab1[MAXN], n_lab2[MAXN];
  int n_ptn1[MAXN], n_ptn2[MAXN];
  int n_o1[MAXN], n_o2[MAXN];
  static DEFAULTOPTIONS_GRAPH(n_options);
  n_options.getcanon = TRUE;
  n_options.defaultptn = FALSE;
  setword n_ws[50*MAXM];
  statsblk n_stats;
  graph n_rg1[MAXN*MAXM];
  graph n_rg2[MAXN*MAXM];

  assert(aSimple.size() < MAXN);
  assert(g2.aSimple.size() < MAXN);
  if (aSimple.size() != g2.aSimple.size()) return false;
  
  int n_n = aSimple.size(), n_m = (n_n + WORDSIZE - 1) / WORDSIZE;
  nauty_check(WORDSIZE, n_m, n_n, NAUTYVERSIONID);

  for (int i = 0; i < n_n; ++i)
    {
      set* gv1 = GRAPHROW(n_g1, i, n_m);
      EMPTYSET(gv1, n_m);
      set* gv2 = GRAPHROW(n_g2, i, n_m);
      EMPTYSET(gv2, n_m);
      for (int j = 0; j < n_n; ++j)
        if (i != j)
          {
            if (aSimple[i][j] != 0)
              ADDELEMENT(gv1, j);
            if (g2.aSimple[i][j] != 0)
              ADDELEMENT(gv2, j);
          }
    }
  
  int cp1 = 1, cp2 = 1;
  for (int i = 0; i < n_n; ++i)
    {
      n_lab1[i] = n_lab2[i] = i;
      if (cp1 < partitions.size())
        if (partitions[cp1] == i) { n_ptn1[i] = 0; ++cp1; }
        else n_ptn1[i] = 1;
      if (cp2 < g2.partitions.size())
        if (partitions[cp2] == i) { n_ptn2[i] = 0; ++cp2; }
        else n_ptn2[i] = 1;
    }
  
  nauty((graph*)&n_g1,n_lab1,n_ptn1,NULL,n_o1,&n_options,&n_stats,
        n_ws,50*n_m,n_m,n_n,(graph*)&n_rg1);
  nauty((graph*)&n_g2,n_lab2,n_ptn2,NULL,n_o2,&n_options,&n_stats,
        n_ws,50*n_m,n_m,n_n,(graph*)&n_rg2);

  return aresame((graph*)&n_rg1, (graph*)&n_rg2, n_n, n_m);
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
