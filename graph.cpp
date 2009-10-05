#include "graph.h"

using namespace std;

void
Graph::PrintMatrix(FILE* f) const
{
  for (int i = 0; i < a.size(); i++)
    {
      fprintf(f, "    ");
      for (int j = 0; j < a[i].size(); j++)
        fprintf(f, "%d ", a[i][j]);
      fprintf(f, "\n");
    }
}

#ifdef USE_NAUTY
void
Graph::PrintMatrixSimple(FILE* f, graph* g) const
{
  for (int i = 0; i < nautyK; i++)
    {
      fprintf(f, "    ");
      set *row = GRAPHROW(g, i, nautyM);
      for (int j = 0; j < nautyK; j++)
        if (ISELEMENT(row, j))
          fprintf(f, "1 ");
        else
          fprintf(f, "0 ");
      fprintf(f, "\n");
    }
}
#endif


#ifdef USE_LINES_NO_MAP
void
Graph::PrintMatrixSorted(FILE* f) const
{
  for (int i = 0; i < aSorted.size(); i++)
    {
      fprintf(f, "    ");
      for (int j = 0; j < aSorted[i].size(); j++)
        fprintf(f, "%d ", aSorted[i][j]);
      fprintf(f, "\n");
    }
}
#endif

void
Graph::PrintNormal(FILE* f) const
{
  int c = 0;
  for (int i = 0; i < K; i++)
    for (int j = i; j < K; j++)
      c += a[i][j];
  fprintf(f, "Genus: %d,\n", G);
  fprintf(f, "  codimension: %d,\n", c);
  fprintf(f, "  irreducible components: %d,\n", K);
  fprintf(f, "  with genus ");
  for (int i = 0; i < K; i++)
    fprintf(f, "%d ", g[i]);
  fprintf(f, "\n");
  fprintf(f, "  with marked points ");
  for (int i = 0; i < K; i++)
    fprintf(f, "%d ", m[i]);
  fprintf(f, "\n");
  fprintf(f, "  with loops ");
  for (int i = 0; i < K; i++)
    fprintf(f, "%d ", l[i]);
  fprintf(f, "\n");
  fprintf(f, "  with divisions ");
  for (int i = 0; i < simple_divisions.size(); i++)
    fprintf(f, "%d ", simple_divisions[i]);
  fprintf(f, "\n");
  fprintf(f, "  with adjacency matrix\n");
  PrintMatrix(f);
  fprintf(f, "\n");
}

void
Graph::PrintBinary(FILE* f) const
{
  unsigned char tmp;
  tmp = (unsigned char) K;
  fwrite(&tmp, sizeof(unsigned char), 1, f);

  for (int i = 0; i < K; ++i)
    {
      tmp = (unsigned char) g[i];
      fwrite(&tmp, sizeof(unsigned char), 1, f);
    }
  for (int i = 0; i < K; ++i)
    {
      tmp = (unsigned char) m[i];
      fwrite(&tmp, sizeof(unsigned char), 1, f);
    }
  for (int i = 0; i < K; ++i)
    {
      tmp = (unsigned char) l[i];
      fwrite(&tmp, sizeof(unsigned char), 1, f);
    }
  for (int i = 0; i < K; ++i)
    for (int j = i+1; j < K; ++j)
      {
        tmp = (unsigned char) a[i][j];
        fwrite(&tmp, sizeof(unsigned char), 1, f);
      }
}

void
Graph::PrintLaTeX(FILE* f) const
{
  fprintf(f, "    \\begin{tikzpicture}[baseline]\n");
  fprintf(f, "      \\path(0,0) ellipse (2 and %d);\n", (K>2? 2: 1));
  
  for (int i = 0; i < K; i++)
    {
      // The following parameter is decorative: it is used to center the position of the marked point when it is only one.
      int param=0;
      int param2=0;
      int inutile=0;
      (m[i]==1)?param=30:param=0;
      K==1?param2=30:param2=0;
      if (m[i]!=0 && a[i][i] !=0)
        fprintf(f, "      \\tikzstyle{level 1}=[counterclockwise from=%d,level distance=9mm,sibling angle=%d]\n", -120+360*i/K+2*param-param2, (m[i]==1?0: (60+2*param2)/(m[i]-1)));
      (m[i]==1 && K==1)? inutile = 30:inutile=0;
      if (m[i]!=0 && a[i][i] ==0)
        fprintf(f, "      \\tikzstyle{level 1}=[counterclockwise from=%d,level distance=9mm,sibling angle=%d]\n", -60+360*i/K+2*param-param2+inutile, (m[i]==1?0: (120+8*param2)/(m[i]-1)));;
      fprintf(f, "      \\node (A%d) at (%d:1) {$\\scriptstyle{%d_{%d}}$}", i, (360 * i / K), g[i], m[i]);	
      for (int j= 0; j< m[i]; j++)
        fprintf(f, " child");
      fprintf(f, ";\n");
    }
  fprintf(f, "\n");	

  for (int i = 0; i < K; i++)
    for (int j = 0; j < a[i][i]; j++)
      {
        int param=0;
        K==1?param=2:param=0;
        if (m[i]!=0)
          fprintf(f, "      \\draw (A%d) .. controls +(%lf:1.2) and +(%lf:1.2) .. (A%d);\n", i, ((j - a[i][i]/2.0+1+param) * 30 + (360*i/K)), ((j - a[i][i]/2.0 + 2+param) * 30 + (360*i/K)), i);
        else
          fprintf(f, "      \\draw (A%d) .. controls +(%lf:1.2) and +(%lf:1.2) .. (A%d);\n", i, ((j - a[i][i]/2.0) * 30 + (360*i/K)), ((j - a[i][i]/2.0 + 1) * 30 + (360*i/K)), i);
      }

  for (int i = 0; i < K; i++)
    for (int j = i+1; j < K; j++)
      for (int k = 0; k < a[i][j]; k++)
        fprintf(f, "      \\path (A%d) edge [bend left=%lf] (A%d);\n", i, ((k - a[i][j]/2.0) * 30+15), j);

  
  fprintf(f, "    \\end{tikzpicture}\n");
}

void
Graph::PrintPretty(FILE* f, int d, int r, const vector< bool >& divis,
                   int start, int end) const
{
  fprintf(f, "Diagonale = %d, Riga = %d\n", d, r);
  fprintf(f, "Prossimo numero tra %d e %d.\n", start, end);
  for (int j = 0; j < K; j++)
    fprintf(f, "  %d  ", g[j]);
  fprintf(f, "\n");

  for (int i = 0; i < K; i++)
    {
      if (divis[i])
        for (int j = 0; j < K; j++)
          if (divis[j])
            fprintf(f, "+-");
          else
            fprintf(f, "--");
      else
        for (int j = 0; j < K; j++)
          if (divis[j])
            fprintf(f, "| ");
          else
            fprintf(f, "  ");
      fprintf(f, "\n");

      for (int j = 0; j < K; j++)
        if (divis[j])
          fprintf(f, "|%d", a[i][j]);
        else
          fprintf(f, " %d", a[i][j]);
      fprintf(f, "\n");
    }
  fprintf(f, "\n");
}

Graph::Graph(int g, int n, int k)
  : G(g),
    M(n),
    K(k)
{
#ifdef USE_NAUTY
  this->nautyK = -1;
#endif
}

Graph::Graph(const Graph& g2)
{
  G = g2.G;
  M = g2.M;
  K = g2.K;  

  g = g2.g;
  m = g2.m;
  l = g2.l;
  a = g2.a;

  edges = g2.edges;
  total_edges = g2.total_edges;
  p1 = g2.p1;
  sum = g2.sum;
  msum = g2.msum;
  connections = g2.connections;
  is_rational_tail = g2.is_rational_tail;
  is_compact_type = g2.is_compact_type;

  simple_divisions = g2.simple_divisions;
#ifdef USE_DEGREES_MAP
  degrees = g2.degrees;
#endif
#ifdef USE_DEGREES_NO_MAP
  aSortedDiv = g2.aSortedDiv;
#endif
#ifdef USE_LINES_MAP
  lines = g2.lines;
#endif
#ifdef USE_LINES_NO_MAP
  aSorted = g2.aSorted;
#endif
#ifdef START_LAPACK_COMPUTATION
  laMatrix = g2.laMatrix;
  eigenvalues = g2.eigenvalues;
#endif
#ifdef USE_NAUTY
  nautyK = g2.nautyK;
  nautyM = g2.nautyM;
  memcpy(nautyGraph, g2.nautyGraph, MAXN*MAXM*sizeof(graph));
#endif
}

Graph::Graph(FILE* f)
{
  unsigned char tmp;
  
  assert(fread(&tmp, sizeof(unsigned char), 1, f) == 1);
  K = tmp;

  G = -K+1;
  for (int i = 0; i < K; ++i)
    {
      assert(fread(&tmp, sizeof(unsigned char), 1, f) == 1);
      g[i] = tmp;
      G += tmp;
    }

  M = 0;
  for (int i = 0; i < K; ++i)
    {
      assert(fread(&tmp, sizeof(unsigned char), 1, f) == 1);
      m[i] = tmp;
      M += tmp;
    }

  for (int i = 0; i < K; ++i)
    {
      assert(fread(&tmp, sizeof(unsigned char), 1, f) == 1);
      a[i][i] = l[i] = tmp;
      G += tmp;
    }

  for (int i = 0; i < K; ++i)
    for (int j = i+1; j < K; ++j)
      {
        assert(fread(&tmp, sizeof(unsigned char), 1, f) == 1);
        a[i][j] = a[j][i] = tmp;
        G += tmp;
      }

  nautyK = -1;
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

bool
Graph::NextSpecialPerm(vector< int >& perm)
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
  vector< vector< int > > translateEdges(K, vector< int >(K, -1));

  graph simple[MAXN*MAXM];
  int ptn[MAXN];
  
  int maxa = 0;
  
  for (int i = 0; i < K; ++i)
    for (int j = i+1; j < K; ++j)
      if (maxa < a[i][j]) maxa = a[i][j];

  // Build translation table for vertices and edges
  nautyK = K;
  ptn[K-1] = 0;
  for (int i = 0; i < K-1; ++i)
    if (g[i] != g[i+1] || m[i] != m[i+1] || l[i] != l[i+1])
      ptn[i] = 0;
    else
      ptn[i] = 1;

  for (int ia = 2; ia <= maxa; ++ia)
    {
      for (int i = 0; i < K; ++i)
        for (int j = i+1; j < K; ++j)
          if (ia == a[i][j])
            {
              translateEdges[i][j] = nautyK;
              translateEdges[j][i] = nautyK;
              ptn[nautyK] = 1;
              ++nautyK;
            }
      ptn[nautyK-1] = 0;
    }
  assert(nautyK < MAXN);
  nautyM = (nautyK + WORDSIZE - 1) / WORDSIZE;

  // Computing adjacency matrix of simple graph.
  for (int i = 0; i < nautyK; ++i)
    {
      set* nautyRow = GRAPHROW(simple, i, nautyM);
      EMPTYSET(nautyRow, nautyM);
    }
  
  for (int i = 0; i < K; ++i)
    {
      set* nautyRow = GRAPHROW(simple, i, nautyM);
      for (int j = 0; j < K; ++j)
        if (i != j)
          {
            if (a[i][j] == 1)
              ADDELEMENT(nautyRow, j);
            else if (a[i][j] > 1)
              ADDELEMENT(nautyRow, translateEdges[i][j]);
          }
    }
  for (int i = 0; i < K; ++i)
    for (int j = i+1; j < K; ++j)
      if (a[i][j] > 1)
        {
          set* nautyRow = GRAPHROW(simple, translateEdges[i][j], nautyM);
          ADDELEMENT(nautyRow, i);
          ADDELEMENT(nautyRow, j);
        }

  // Computing canonical labelling
  int lab[MAXN];
  int orb[MAXN];
  static DEFAULTOPTIONS_GRAPH(options);
  options.getcanon = TRUE;
  options.defaultptn = FALSE;
  setword ws[50*MAXM];
  statsblk stats;

  assert(nautyK < MAXN);
  nauty_check(WORDSIZE, nautyM, nautyK, NAUTYVERSIONID);
  for (int i = 0; i < nautyK; ++i)
    lab[i] = i;

  nauty((graph*)&simple, lab, ptn, NULL, orb, &options, &stats,
        ws, 50*nautyM, nautyM, nautyK, (graph*)&nautyGraph);
}

bool
Graph::EqualNauty(Graph& g2)
{
  // We don't assume that we already called ComputeDreadnaut
  if (nautyK == -1) ComputeDreadnaut();
  if (g2.nautyK == -1) g2.ComputeDreadnaut();

  if (nautyK != g2.nautyK) return false;
  
  bool ret = (memcmp(nautyGraph, g2.nautyGraph, nautyK*nautyM*sizeof(graph)) == 0);

  return ret;
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
