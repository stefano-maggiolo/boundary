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

#include "graph.h"

using namespace std;

void
Graph::PrintMatrix(FILE* f) const
{
  for (int i = 0; i < K; i++)
    {
      fprintf(f, "    ");
      for (int j = 0; j < K; j++)
        fprintf(f, "%d ", a[i][j]);
      fprintf(f, "\n");
    }
}

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
        fprintf(f, "      \\tikzstyle{level 1}=[counterclockwise from=%d,level distance=9mm,sibling angle=%d]\n", -60+360*i/K+2*param-param2+inutile, (m[i]==1?0: (120+8*param2)/(m[i]-(K==1?0:1))));;
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

Graph::Graph(int g, int n, int k)
  : G(g),
    M(n),
    K(k)
{
  this->nautyK = -1;
}

Graph::Graph(const Graph& g2)
{
  G = g2.G;
  M = g2.M;
  K = g2.K;

  total_edges = g2.total_edges;
  p1 = g2.p1;
  stab_he_2 = g2.stab_he_2;
  stab_he_3 = g2.stab_he_3;
  memmove(he, g2.he, sizeof(uchar)*K);
  min_he_2 = g2.min_he_2;
  sum = g2.sum;
  msum = g2.msum;
  connections = g2.connections;
  is_rational_tail = g2.is_rational_tail;
  is_compact_type = g2.is_compact_type;

  memmove(g, g2.g, sizeof(uchar)*K);
  memmove(m, g2.m, sizeof(uchar)*K);
  memmove(l, g2.l, sizeof(uchar)*K);
  for (int i = 0; i < K; i++)
    memmove(a[i], g2.a[i], sizeof(uchar)*K);
  memmove(gDegrees, g2.gDegrees, sizeof(uchar)*(G+2));
  divisions = g2.divisions;
  //  memmove(divisions, g2.divisions, sizeof(bool)*(K+1));
  nautyK = g2.nautyK;
  nautyM = g2.nautyM;
  memmove(nautyGraph, g2.nautyGraph, MAXN*MAXM*sizeof(graph));
}

Graph::Graph(FILE* f)
{
  uchar tmp;

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
Graph::Clear(uchar newK)
{
  K = newK;
  // Every time, we rebuild the adjacency matrix with the right
  // dimension.
  memset(a, 0, sizeof(uchar) * MAX_K * MAX_K);
  // We rebuild the genera and the marked points vectors.
  memset(g, 0, sizeof(uchar)*K);
  memset(m, 0, sizeof(uchar)*K);
  memset(l, 0, sizeof(uchar)*K);
  connections = 0;
  total_edges = 0;

  // And we start with the formula corresponding to no edge and
  // no genera assigned.
  sum = -K+1;
  // No marked points assigned, by now.
  msum = 0;
  // Nor genus 0 curves.
  p1 = 0;
  stab_he_2 = 0;
  stab_he_3 = 0;
  memset(he, 0, sizeof(uchar)*K);
  min_he_2 = 2;

  // We clear divisions, but divisions[0] exists by definition.
  divisions = 1;
  //  memset(divisions, 0, sizeof(bool)*(K+1));
  //  divisions[0] = true;
}

bool
Graph::Equal(Graph& g2)
{
  // EqualNauty assumes that K, g, m, l and gDegrees are equal.
  return EqualNauty(g2);
}

void
Graph::ComputeDreadnaut(void)
{
  int translateEdges[MAX_K][MAX_K];

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




GraphClass::GraphClass(const Graph& o)
{
  G = o.G;
  M = o.M;
  K = o.K;
  memmove(g, o.g, sizeof(uchar)*K);
  memmove(m, o.m, sizeof(uchar)*K);
  memmove(l, o.l, sizeof(uchar)*K);
  memmove(gDegrees, o.gDegrees, sizeof(uchar)*(G+2));
}

bool
GraphClass::operator<(const GraphClass& o) const
{
  int t;
  t = o.K - K;
  if (t) return t < 0;

  t = memcmp(g, o.g, sizeof(uchar)*K);
  if (t) return t < 0;

  t = memcmp(m, o.m, sizeof(uchar)*K);
  if (t) return t < 0;

  t = memcmp(l, o.l, sizeof(uchar)*K);
  if (t) return t < 0;

  t = memcmp(gDegrees, o.gDegrees, sizeof(uchar)*(G+2));
  return t < 0;
}
