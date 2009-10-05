#include "graph_ordered_points.h"

using namespace std;

void
Graph2GraphOrderedPointsBT(int x,
                           map< int, int >& c,
                           GraphOrderedPoints& h,
                           vector< GraphOrderedPoints >& ret)
{
  if (x < h.m.size())
    {
      for (map< int, int >::iterator i = c.begin(); i != c.end(); ++i)
        {
          if (i->second > 0)
            {
              h.m[x] = i->first;
              --c[i->first];
              Graph2GraphOrderedPointsBT(x+1, c, h, ret);
              ++c[i->first];
            }
          
        }
    }
  else
    {
      for (vector< GraphOrderedPoints >::iterator i = ret.begin(); i != ret.end(); ++i)
        if (i->Equal(h))
          return;
      GraphOrderedPoints* r = new GraphOrderedPoints(h);
      ret.push_back(*r);
    }
}

vector< GraphOrderedPoints >&
Graph2GraphOrderedPoints(Graph& g)
{
  GraphOrderedPoints h(g);
  vector< GraphOrderedPoints >* ret = new vector< GraphOrderedPoints >(0);
  map< int, int > c; // c[i] = # of marked points remaining in component i

  for (int i = 0; i < g.M; ++i)
    c[i] = g.m[i];

  Graph2GraphOrderedPointsBT(0, c, h, *ret);
  return *ret;
}

GraphOrderedPoints::GraphOrderedPoints(int g, int n, int k)
  : Graph(g, n, k)
{
}

GraphOrderedPoints::GraphOrderedPoints(const Graph& g2)
  : Graph(g2)
{
  mo.assign(M, -1);
}

GraphOrderedPoints::GraphOrderedPoints(const GraphOrderedPoints& g2)
  : Graph((Graph&) g2)
{
  mo = g2.mo;
}

GraphOrderedPoints::GraphOrderedPoints(FILE* f)
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

bool
GraphOrderedPoints::Equal(GraphOrderedPoints& g2)
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
#else
  return EqualPermutations(g2);
#endif
}

#ifdef USE_NAUTY
void
GraphOrderedPoints::ComputeDreadnaut(void)
{
  vector< vector< int > > translateEdges(K, vector< int >(K, -1));
  vector< int > translateMarkedPoints(M, -1);
  
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

  for (int i = 0; i <= M; ++i)
    {
      translateMarkedPoints[i] = nautyK++;
      ptn[nautyK-1] = 0;
    }
  
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
      for (int j = 0; j < M; ++j)
        if (mo[j] == i)
          ADDELEMENT(nautyRow, translateMarkedPoints[j]);
    }
  for (int i = 0; i < M; ++i)
    {
      set* nautyRow = GRAPHROW(simple, translateMarkedPoints[i], nautyM);
      ADDELEMENT(nautyRow, mo[i]);
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
GraphOrderedPoints::EqualNauty(GraphOrderedPoints& g2)
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
GraphOrderedPoints::EqualPermutations(GraphOrderedPoints& g2)
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
      if (!different)
        {
          for (int i = 0; i < M; ++i)
            if (perm[mo[i]] != mo[i])
              {
                different = true;
                break;
              }
          if (!different) return true;
        }
    }
    return false;
}

void
GraphOrderedPoints::PrintBinary(FILE* f) const
{
  unsigned char tmp;
  tmp = (unsigned char) K;
  fwrite(&tmp, sizeof(unsigned char), 1, f);
  tmp = (unsigned char) M;
  fwrite(&tmp, sizeof(unsigned char), 1, f);

  for (int i = 0; i < K; ++i)
    {
      tmp = (unsigned char) g[i];
      fwrite(&tmp, sizeof(unsigned char), 1, f);
    }
  for (int i = 0; i < M; ++i)
    {
      tmp = (unsigned char) mo[i];
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
