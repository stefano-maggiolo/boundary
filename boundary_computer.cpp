#include "boundary_computer.h"

using namespace std;

BoundaryComputer::BoundaryComputer(int g, int n)
{
  G = g;
  M = n;
  computed = false;
}

vector< Graph* >&
BoundaryComputer::GetAllResults(void)
{
  vector< Graph* > *ret = new vector< Graph* >();

  for (map< uchar, vector< Graph* > >::iterator i = store.begin(); i != store.end(); ++i)
    for (vector< Graph* >::iterator j = i->second.begin(); j != i->second.end(); ++j)
      ret->push_back(*j);
  return *ret;
}

map< uchar, vector< Graph* > >&
BoundaryComputer::GetAllResultsByCodimension(void)
{
  return store;
}

void
BoundaryComputer::Compute(GraphPrinter &printer, enum Statistics stats, int computeOnlyCodim)
{
  if (2*G-3+M < 0)
    {
      computed = true;
      if (stats== Full) Statistics(stderr);
      else if (stats== Terse) TerseStatistics(stderr);
      return;
    }

  statistics.assign(3*G-3+M+1, 0);
  statisticsTime.assign(2*G-2+M+1, 0);
  statisticsMemory.assign(2*G-2+M+1, 0);

  printer.BeginPrint();
  this->computeOnlyCodim = computeOnlyCodim;

  // Initialization.
  tried.assign(2*G-2+M+1,0);
  found.assign(2*G-2+M+1,0);
  duplicated.assign(2*G-2+M+1,0);
  unconnected.assign(2*G-2+M+1,0);

  // We do the case with one component separately to not speed down
  // the rest of the computation.
  graph.G = G;
  graph.M = M;

  graph.K = 1;

  if (stats == Full) fprintf(stderr, "K = %2d... ", graph.K);
#if HAVE_GETRUSAGE
  struct rusage rusageBegin, rusageEnd;
  getrusage(RUSAGE_SELF, &rusageBegin);
#endif

  for (int n = 0; n <= graph.G; n++)
    {
      tried[1]++;
      graph.a[0][0] = graph.G-n;
      graph.g[0] = n;
      graph.m[0] = graph.M;
      graph.l[0] = graph.G-n;
      graph.edges[0] =  2*(graph.G-n);
      graph.total_edges = graph.G-n;
      if (computeOnlyCodim == -1 || computeOnlyCodim == graph.total_edges)
        {
          found[1]++;
          add_to_store();
        }
    }

#if HAVE_GETRUSAGE
  getrusage(RUSAGE_SELF, &rusageEnd);
  statisticsTime[graph.K] = ((rusageEnd.ru_utime.tv_sec -
                              rusageBegin.ru_utime.tv_sec)*1000 +
                             (rusageEnd.ru_utime.tv_usec -
                              rusageBegin.ru_utime.tv_usec)/1000);
#if HAVE_MAXRSS
  statisticsMemory[graph.K] = rusageEnd.ru_maxrss;
#else
  statisticsMemory[graph.K] = 0;
  pid_t memoryPid = getpid();
  char s[1000];
  snprintf(s, 1000, "/proc/%d/status", memoryPid);
  FILE* memoryFile = fopen(s, "rt");
  while(fgets(s, 1000, memoryFile))
    {
      if (strncmp(s, "VmData:", 7) == 0)
        statisticsMemory[graph.K] += atoi(s+8);
      else if (strncmp(s, "VmStk:", 6) == 0)
        statisticsMemory[graph.K] += atoi(s+7);
      else if (strncmp(s, "VmExe:", 6) == 0)
        statisticsMemory[graph.K] += atoi(s+7);
    }
  fclose(memoryFile);
#endif
  if (stats == Full)
    fprintf(stderr, "done in %3d.%01d s, using %4d.%01d MB.\n",
            statisticsTime[graph.K] / 1000,
            statisticsTime[graph.K] / 100 % 10,
            statisticsMemory[graph.K] / 1024,
            statisticsMemory[graph.K] / 102 % 10);
#else
  if (stats == Full)
    fprintf(stderr, "done.\n"),
#endif

  printer.PrintSomeGraph(store);
  for (map< uchar, vector< Graph* > >::iterator s = store.begin(); s != store.end(); ++s)
    {
      statistics[s->first] += s->second.size();
      for (vector< Graph* >::iterator t = s->second.begin(); t != s->second.end(); ++t)
        delete *t;
    }
  store.clear();
  divstore.clear();

  // We have at most 2G-2+M components for a curve of genus G and M
  // marked points.
  for (int K = 2; K <= 2*graph.G-2+graph.M; K++)
    {
#ifdef BLADEBUG
      bla_g.clear();
      bla_gn.clear();
      bla_gnl.clear();
#endif
      if (computeOnlyCodim != -1 && K > computeOnlyCodim + 1) break;

      if (stats == Full) fprintf(stderr, "K = %2d... ", K);
#if HAVE_GETRUSAGE
      getrusage(RUSAGE_SELF, &rusageBegin);
#endif

      // Every time, we rebuild the graph from scratch
      graph.Clear(K);

      // Let's start.
      bt_g(0);

#if HAVE_GETRUSAGE
      getrusage(RUSAGE_SELF, &rusageEnd);
      statisticsTime[graph.K] = ((rusageEnd.ru_utime.tv_sec -
                                  rusageBegin.ru_utime.tv_sec)*1000 +
                                 (rusageEnd.ru_utime.tv_usec -
                                  rusageBegin.ru_utime.tv_usec)/1000);
#if HAVE_MAXRSS
      statisticsMemory[graph.K] = rusageEnd.ru_maxrss;
#else
      statisticsMemory[graph.K] = 0;
      pid_t memoryPid = getpid();
      char s[1000];
      snprintf(s, 1000, "/proc/%d/status", memoryPid);
      FILE* memoryFile = fopen(s, "rt");
      while(fgets(s, 1000, memoryFile))
        {
          if (strncmp(s, "VmData:", 7) == 0)
            statisticsMemory[graph.K] += atoi(s+8);
          else if (strncmp(s, "VmStk:", 6) == 0)
            statisticsMemory[graph.K] += atoi(s+7);
          else if (strncmp(s, "VmExe:", 6) == 0)
            statisticsMemory[graph.K] += atoi(s+7);
        }
      fclose(memoryFile);
#endif
      if (stats == Full)
        fprintf(stderr, "done in %3d.%01d s, using %4d.%01d MB.\n",
                statisticsTime[graph.K] / 1000,
                statisticsTime[graph.K] / 100 % 10,
                statisticsMemory[graph.K] / 1024,
                statisticsMemory[graph.K] / 102 % 10);
#else
      if (stats == Full)
        fprintf(stderr, "done.\n");
#endif

      printer.PrintSomeGraph(store);
      for (map< uchar, vector< Graph* > >::iterator s = store.begin(); s != store.end(); ++s)
        {
          statistics[s->first] += s->second.size();
          for (vector< Graph* >::iterator t = s->second.begin(); t != s->second.end(); ++t)
            delete *t;
        }
      store.clear();
      divstore.clear();
#ifdef BLADEBUG
      for (map< pair< vector< int >, pair< vector< int >, vector< int > > >, int >::iterator s = bla_gnl.begin(); s != bla_gnl.end(); ++s)
        {
          fprintf(stderr, "%6d     (", s->second);
          for (int i = 0; i < K-1; i++)
            fprintf(stderr, "%d,", s->first.first[i]);
          fprintf(stderr, "%d) (", s->first.first[K-1]);
          for (int i = 0; i < K-1; i++)
            fprintf(stderr, "%d,", s->first.second.first[i]);
          fprintf(stderr, "%d) (", s->first.second.first[K-1]);
          for (int i = 0; i < K-1; i++)
            fprintf(stderr, "%d,", s->first.second.second[i]);
          fprintf(stderr, "%d)\n", s->first.second.second[K-1]);
        }
#endif
    }

  printer.EndPrint();
  computed = true;

  if (stats== Full) Statistics(stderr);
  else if (stats== Terse) TerseStatistics(stderr);
}

void
BoundaryComputer::Compute(GraphPrinter &printer, enum Statistics stat)
{
  Compute(printer, stat, -1);
}

void
BoundaryComputer::Compute(GraphPrinter &printer)
{
  Compute(printer, Full, -1);
}

void
BoundaryComputer::bt_g(int i)
{
  if (i < graph.K) // We have to assign g_i.
    {
      // We impose the vector g to be non-decreasing.
      int start = (graph.divisions & (1 << i))? 0: graph.g[i-1];
      // Here, we have:
      //   $sum = \sum_{j<i} g_j - K+1$,
      //   $G-sum = g_i + \sum_{j>i} g_j + |E|$,
      //   $|E| >= K-1$,
      //   $\sum_{j>=i} g_j >= (K-i) g_i$, hence:
      //   $(K-i)g_i <= G - sum - (K-1)$.
      int end = (graph.G-graph.sum - (graph.K - 1)) / (graph.K-i);
      // Let introduce a convention: whenever we want to use a
      // condition of the form "we need at least ... to stabilize
      // ...", we mean:

      // a. we split the edges in K-1 connection edges and the
      //    remaining. Connection edges contribute to one
      //    stabilization for every vertex, plus K-2 stabilization
      //    that we can choose how to distribute; the remaining
      //    $|E|-(K-1)$ edges give us 2 stabilization each, hence the
      //    total number of stabilizations are $2(|E|-(K-1)) + K-2 =
      //    2|E|-K$.

      // b. hence, before beginning to assign non-loop edges, we
      //    consider a genus 0 curve stabilized when it has 2
      //    stabilization, because we know for certain that one more
      //    will arrive to connect it to the rest of the graph.

      // Consider: p1 genus 0 curves to stabilize, before deciding g_i.
      // Max n. of edges: (G-sum) - (K-i)g_i;
      // Max n. of stab. h/edges from edges: 2((G-sum) - (K-i)g_i) - K.
      // Max n. of stab. h/edges from marked pts: M
      // H/edges needed to stabilize: 2p1
      // => 2p1 <= 2((G-sum) - (K-i)g_i) - K + M
      // => 2(K-i)g_i <= 2(G-sum) - K + M - 2p1
      end = min(end,
                (2*(graph.G - graph.sum) -
                 graph.K + graph.M -
                 2*graph.p1) / (2*(graph.K-i)));
      for (int n = start; n <= end; n++)
        {
          // We do the changes induced by g[i] = n.
          graph.g[i] = n;
          if (n == 0) graph.p1++;
          graph.sum += n;
          // If we are increasing the genus with respect to the
          // previous one, we cannote exchange anymore components < i
          // with components >= i, so we put a division.
          if (n > start) graph.divisions |= 1 << i;

          bt_g(i+1);

          // We go back to the previous situation
          graph.sum -= n;
          if (n == 0) graph.p1--;
          graph.divisions &= ~(1<<i);
        }
    }
  else
    {
      // If we decided all the genera, we go to assign values to the
      // marked points.
#ifdef BLADEBUG
      bla_g[vector<int>(graph.g,graph.g+graph.K)] = 0;
#endif
      bt_m(0);
    }
}

void
BoundaryComputer::bt_m(int i)
{
  if (i < graph.K) // We have to assign m[i].
	{
      bool is_p1 = i < graph.p1;
      // We impose the vector m to be non-decreasing for i's such that
      // the genus is equal.
      int start = (graph.divisions & (1 << i))? 0: graph.m[i-1];
      // For m, start may change from the one we get with divisions;
      // but we have to use the latter to compute further divisions,
      // hence we save it in start_div.
      int start_div = start;
      // m_i is always smaller or equal than M-msum.
      int end = graph.M-graph.msum;
      if (is_p1)
        {
          // Up to now, end is the remaining number of marked pts. In
          // the range [0,p1-1], m is increasing, hence we can't waste
          // all marked pts on i.
          end /= graph.p1-i;

          // Here, $sum = \sum g_j - K + 1$, hence $G-sum =
          // |E|$. Therefore we have available $2(G-sum - (K-1)) + K-2
          // = 2(G-sum) - K$ stabilizing half edges.

          // Consider the first i+1 genus 0 curves. We have stab_he_2
          // + min(m_i, 2) stabilizing half edges on them; the
          // remaining stabilizations needed are 2(i+1) - stab_he_2 -
          // min(m_i,2), so this amount has to be smaller or equal to
          // the half edges we have: $2(G-sum) - K$; therefore we
          // obtain $min(m,2) >= -(2(G-sum) - K) + 2(i+1) -
          // stab_he_2$. If the RHS is greater or equal than 2, the
          // inequality cannot be satisfied.
          int tmp = -(2*(graph.G-graph.sum)-graph.K) + 2*(i+1)-graph.stab_he_2;
          if (tmp > 2)
            return;
          start = max(start, tmp);

          // Consider: the p1 genus 0 curves, after deciding m_i.
          // Max n. of edges: G-sum
          // Max n. of stab. h/edges from edges: 2(G-sum) - K
          // Max n. of stab. h/edges from marked points: M-msum-m
          // H/edges needed to stabilize: 2p1-stab_he_2-min(2,m_i)
          // => 2p1-stab_he_2-min(2,m_i) <= 2((G-sum)-K + M-msum-m
          // => m-min(2,m_i) <= 2(G-sum)-K + M-msum - (2p1-stab_he_2)$.
          int lim_to_wasted = 2*(graph.G-graph.sum)-graph.K +
            graph.M-graph.msum - (2*graph.p1-graph.stab_he_2);
          if (lim_to_wasted < 0)
            return;
          else
            end = min(end, 2+lim_to_wasted);
        }
      for (int n = start; n <= end; n++)
		{
		  if (i == graph.K-1 && n != end) continue;

		  graph.m[i] = n;
		  unsigned int tmp = graph.divisions;
		  if (n > start_div) graph.divisions |= 1 << i;
		  graph.msum += n;
          if (is_p1)
            {
              graph.stab_he_2 += min(n, 2);
              graph.he[i] += n;
            }

		  bt_m(i+1);

          if (is_p1)
            {
              graph.he[i] -= n;
              graph.stab_he_2 -= min(n, 2);
            }
		  graph.msum -= n;
		  graph.divisions = tmp;
		}

	}
  else
    {
      // If we decided all the marked points, we go to assign values
      // to the diagonal of the adjacency matrix.
#ifdef BLADEBUG
      bla_gn[make_pair(vector<int>(graph.g,graph.g+graph.K), vector<int>(graph.m,graph.m+graph.K))] = 0;
#endif
      bt_l(0);
    }
}

void
BoundaryComputer::bt_l(int i)
{
  if (i < graph.K) // We have to assign l[i].
    {
      bool is_p1 = i < graph.p1;
      // We may permute i with the elements inside all the range
      // around i that does not meet divisions. In particular, if i is
      // not the first element after a division, we may assume to have
      // permuted the components in such a way that they are
      // non-decreasing.
      int start = (graph.divisions & (1 << i))? 0: graph.l[i-1];
      // See the comment in bt_m.
      int start_div = start;

      // Here, $sum = \sum g_j + \sum_{j<i} l_j - K + 1$, hence $G-sum
      // = l_i + \sum_{j>i} l_j + |E~|$, where $E~$ is the set of
      // non-loop edges. Note that connecting edges are contained in
      // E~, hence $|E~| >= K-1$. Therefore, $l_i <= G-sum - (K-1)

      // After deciding l_i, we have $stab_he_2 + min(2,m_i+2l_i)$
      // stabilizing half edges, $G-sum-l_i$ edges to place.
      int end = graph.G - graph.sum - (graph.K-1);

      // Consider the genus 0 curve with the minimum number of
      // stabilizing half edges amongst the ones with index less than
      // i, after deciding l_i.
      // Max n. of edges: G-sum-l_i
      // Max n. of stab. h/edges from edges: G-sum-l_i-1 (since we can't use loops anymore)
      // Max n. of stab. h/edges from marked points: 0
      // H/edges needed to stabilize: 2-min_he_2
      // => 2-min_he_2 <= G-sum-l_i-1
      // => l_i <= G-sum - 3 + min_he_2
      end = min(end,
                (graph.G-graph.sum) - 3 + graph.min_he_2);

      // Stabilization half edges gained if l_i > 0
      int stab_gained = max(2-graph.m[i],0);
      if (!is_p1) stab_gained = 0;

      // Consider the p1 genus 0 curves, after deciding l_i
      // Max n. of edges: G-sum-l_i
      // Max n. of stab. h/edges from edges: 2(G-sum-l_i)-K
      // Max n. of stab. h/edges from marked points: 0
      // H/edges needed to stabilize: 2p1-stab_he_2-stab_gained
      // => 2p1-stab_he_2-stab_gained <= 2(G-sum-l_i)-K
      // => 2l_i <= 2(G-sum) - K - 2p1 + stab_he_2 + stab_gained
      end = min(end,
                (2*(graph.G - graph.sum) - graph.K - 2*graph.p1 + graph.stab_he_2 + stab_gained)/2);

      if (is_p1)
        {
          // We have $2-min(2,m_i+2l_i)$ half edges to add to curve
          // i. Note that the $G-sum-l_i$ edges to place cannot give 2
          // stabilizing points anymore to the i curve because we
          // already decided its loops. Hence, the maximum number of
          // stabilizing half edges for i is $G-sum-l_i-(K-1) + K-2 =
          // G-sum-l_i-1$. Therefore, we need to ensure that $m + 2l_i
          // + G-sum-l_i-1 >= 2$.
          start = max(start, 3 - (graph.G - graph.sum) - graph.m[i]);
        }
      for (int n = start; n <= end; n++)
        {
          // We do the changes induced by l[i] = n.
          graph.l[i] = n;
          graph.a[i][i] = n;
          unsigned int tmp;
          int tmp_min_he_2;
          if (n > 0)
            {
              tmp = graph.divisions;
              if (n > start_div) graph.divisions |= 1 << i;
              graph.edges[i] += 2*n;
              graph.total_edges += n;
              graph.sum += n;
              if (is_p1)
                {
                  graph.stab_he_2 -= min((int)graph.he[i],2);
                  graph.stab_he_2 += min((int)graph.he[i]+2*n,2);
                  graph.he[i] += 2*n;
                }
            }
          if (is_p1)
            {
              tmp_min_he_2 = graph.min_he_2;
              graph.min_he_2 = min((int)graph.min_he_2, min((int)graph.he[i], 2));
            }

          bt_l(i+1);

          // We go back to the previous situation.
          if (is_p1)
            {
              graph.min_he_2 = tmp_min_he_2;
            }
          if (n > 0)
            {
              if (is_p1)
                {
                  graph.he[i] -= 2*n;
                  graph.stab_he_2 += min((int)graph.he[i],2);
                  graph.stab_he_2 -= min((int)graph.he[i]+2*n,2);
                }
              graph.sum -= n;
              graph.total_edges -= n;
              graph.edges[i] -= 2*n;
              graph.divisions = tmp;
            }
        }
    }
  else // If we decided all diagonal elements, we go to assign values
       // to the rest of the upper triangle of the matrix, row by row,
       // from left to right.
    {
#ifdef BLADEBUG
      bla_gnl[make_pair(vector<int>(graph.g,graph.g+graph.K), make_pair(vector<int>(graph.m,graph.m+graph.K), vector<int>(graph.l,graph.l+graph.K)))] = 0;
#endif
      memset(graph.gDegrees, 0, sizeof(uchar)*(graph.G+2));
      graph.stab_he_3 = 0;
      for (int q = 0; q < graph.p1; q++)
        graph.stab_he_3 += min(3, (int)graph.he[q]);
      bt_a(0, 1);
    }
}


void
BoundaryComputer::bt_a(int i, int j)
{
  if (j >= graph.K) // We do the carrying by hand.
    {
      i++;
      j = i+1;
    }

  if (i < graph.K-1) // We have to assign a[i][j].
    {
      bool is_i_p1 = i < graph.p1;
      bool is_j_p1 = j < graph.p1;

      // If i is not the first element in its non-divisions range,
      // then we can assume it is not less then the element in the
      // previous row.
      int start_i = (graph.divisions & (1 << i))? 0: graph.a[i-1][j];
      // Also, if j is not the first element in its non-divisions
      // range, than we can assume it is not less than the element in
      // the previous column.
      int start_j = 0;
      if (j > i+1 && !(graph.divisions & (1 << j))) start_j = graph.a[i][j-1];
      int start = max(start_i, start_j);

      // Sure a[i][j] cannot exceed G-sum; but if we put c connections
      // up to now, sure we will have to put at least other K-2-c
      // edges to connect the graph.
      int end = graph.G - graph.sum - max(0, graph.K-2-graph.connections);

      // Consider the first p1 genus 0 curves, after deciding a_ij
      // Max n. of edges: G-sum-a_ij
      // Max n. of stab. h/edges from edges: 2(G-sum-a_ij)
      // Max n. of stab. h/edges from marked points: 0
      // H/edges needed to stabilize: 3p1 - stab_he_3 - min()
      // => 3p1 - stab_he_3 <= G-sum-a_ij
      // => a_ij <= G-sum - 2p1 + stab_he_2
      int max_stab_gained = max(0, 3-graph.he[i]) + max(0, 3-graph.he[j]);
      end = min(end, (2*(graph.G-graph.sum) - 3*graph.p1 + graph.stab_he_3 + max_stab_gained)/2);
      for (int n = start; n <= end; n++)
        {
          // We check the following, to ensure that the sum = G and
          // that all genus 0 curve are stabilized.
          if (j == graph.K-1)
            {
              // If this is the last chance to add edge to a genus 0
              // curve, we add enough to stabilize it.
              if (i < graph.p1 && graph.m[i]+graph.edges[i]+n < 3) continue;
              // If we're finishing...
              if (i == graph.K-2)
                {
                  // For i = K-1, the last chance is actually at i = K-2.
                  if (graph.p1 == graph.K && graph.m[graph.K-1]+graph.edges[graph.K-1]+n < 3) continue;
                  // The grand total has to be G.
                  else if (graph.sum + n != graph.G) continue;
                }
              // If we don't connect the curve now...
              if (n == 0)
                {
                  // A curve has to be connected to at least one
                  // different curve.
                  if (graph.edges[i] == 2*graph.l[i]) continue;
                  // For i = K-1, last chance to connect the last
                  // curve.
                  else if (i == graph.K-2 && graph.edges[graph.K-1] == 2*graph.l[graph.K-1]) continue;
                }
              // All right!
            }
          // Changes induced by a[i][j] = n.
          graph.a[i][j] = n;
          graph.a[j][i] = n;
          unsigned int tmp;
          if (n > 0)
            {
              graph.connections++;
              if (is_i_p1)
                {
                  graph.stab_he_3 -= min((int)graph.he[i],3);
                  graph.stab_he_3 += min((int)graph.he[i]+n,3);
                }
              if (is_j_p1)
                {
                  graph.stab_he_3 -= min((int)graph.he[j],3);
                  graph.stab_he_3 += min((int)graph.he[j]+n,3);
                }
              graph.edges[i] += n;
              graph.edges[j] += n;
              graph.total_edges += n;
              graph.sum += n;
              graph.he[i] += n;
              graph.he[j] += n;
              tmp = graph.divisions;
              if (n > start_i) graph.divisions |= 1 << i;
              if (n > start_j && j > i+1) graph.divisions |= 1 << j;
              graph.gDegrees[n]++;
            }

          bt_a(i, j+1);

          // We go back to the previous situation.
          if (n > 0)
            {
              graph.gDegrees[n]--;
              graph.divisions = tmp;
              graph.he[i] -= n;
              graph.he[j] -= n;
              graph.sum -= n;
              graph.edges[i] -= n;
              graph.edges[j] -= n;
              graph.total_edges -= n;
              graph.connections--;
              if (is_i_p1)
                {
                  graph.stab_he_3 += min((int)graph.he[i],3);
                  graph.stab_he_3 -= min((int)graph.he[i]+n,3);
                }
              if (is_j_p1)
                {
                  graph.stab_he_3 += min((int)graph.he[j],3);
                  graph.stab_he_3 -= min((int)graph.he[j]+n,3);
                }
            }
        }
    }
  else // If we decided all the matrix, we check if the graph is
       // acceptable. In case, we print it.
    {
      if (computeOnlyCodim == -1 ||
          graph.total_edges == computeOnlyCodim)
        {
          // Note: we could also compute all canonical labelling
          // whether or not we'll use it: for example, for (6,0) there
          // are just ~700 graphs for which the canonical labelling
          // won't be used, but they are with K = 3.
          graph.nautyK = -1;

          if (correct()) add_to_store();
        }
    }
}

void
BoundaryComputer::add_to_store(void)
{
#ifdef BLADEBUG
  bla_g[vector<int>(graph.g,graph.g+graph.K)]++;
  bla_gn[make_pair(vector<int>(graph.g,graph.g+graph.K), vector<int>(graph.m,graph.m+graph.K))]++;
  bla_gnl[make_pair(vector<int>(graph.g,graph.g+graph.K), make_pair(vector<int>(graph.m,graph.m+graph.K), vector<int>(graph.l,graph.l+graph.K)))]++;
#endif
  Graph *ng = new Graph(graph);
  store[ng->total_edges].push_back(ng);
  GraphClass bucket(graph);
  divstore[bucket].push_back(ng);
}

bool
BoundaryComputer::correct()
{
  tried[graph.K]++;
  // First condition (sum = G): already ensured.
  // Second condition (stability) already ensured.
  // Third condition (connectedness).
  memset(v, 0, sizeof(bool)*graph.K);
  if (visit(0) < graph.K)
    {
      unconnected[graph.K]++;
      return false;
    }
  else if (duplicate())
    {
      duplicated[graph.K]++;
      return false;
    }
  else
    {
      found[graph.K]++;
      return true;
    }
}

int
BoundaryComputer::visit(int i)
{
  v[i] = true;
  int s = 1;
  for (int n = 0; n < graph.K; n++)
    if (graph.a[i][n] != 0 && !v[n])
      s += visit(n);
  return s;
}

bool
BoundaryComputer::duplicate(void)
{
  GraphClass bucket(graph);
  vector< Graph* > littlestore = divstore[bucket];
  for (vector< Graph* >::iterator s = littlestore.begin(); s != littlestore.end(); ++s)
    if ((*s)->Equal(graph))
      return true;
  return false;
}

void
BoundaryComputer::Statistics(FILE* file)
{
  if (!computed)
    {
      fprintf(file, "Not yet computed!\n");
      return;
    }

#if HAVE_GETRUSAGE
  int ms = 0;
  for (int i = 0; i < (int)statisticsTime.size(); i++)
    ms += statisticsTime[i];
  fprintf(file, "Total time: %d.%01d s.\n", ms/1000, ms/100%10);
  int kB = 0;
  for (int i = 0; i < (int)statisticsTime.size(); i++)
    if (kB < statisticsMemory[i]) kB = statisticsMemory[i];
  fprintf(file, "Total memory: %d.%01d MB.\n", kB/1024, kB/102 % 10);
#endif

  int s = 0;
  fprintf(file, "Tried       ");
  for (int i = 0; i < (int)tried.size(); i++)
    {
      fprintf(file, "%5d ", tried[i]);
      s += tried[i];
    }
  fprintf(file, "= %7d\n", s);
  s=0;
  fprintf(file, "Unconnected ");
  for (int i = 0; i < (int)unconnected.size(); i++)
    {
      fprintf(file, "%5d ", unconnected[i]);
      s += unconnected[i];
    }
  fprintf(file, "= %7d\n", s);
  s=0;
  fprintf(file, "Duplicated  ");
  for (int i = 0; i < (int)duplicated.size(); i++)
    {
      fprintf(file, "%5d ", duplicated[i]);
      s += duplicated[i];
    }
  fprintf(file, "= %7d\n", s);
  s=0;
  fprintf(file, "Found       ");
  for (int i = 0; i < (int)found.size(); i++)
    {
      fprintf(file, "%5d ", found[i]);
      s += found[i];
    }
  fprintf(file, "= %7d\n", s);

  // Statistic subdivided by number of edges.
  for (int i = 0; i < (int)statistics.size(); ++i)
    fprintf(file, "%3d: %5d\n", i, statistics[i]);
}

void
BoundaryComputer::TerseStatistics(FILE* file)
{
  if (!computed)
    {
      fprintf(file, "Not yet computed!\n");
      return;
    }

  fprintf(file, "(\n");

  fprintf(file, " (");
  for (int i = 0; i < (int)statisticsTime.size(); i++)
    fprintf(file, "%d,", statisticsTime[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < (int)statisticsMemory.size(); i++)
    fprintf(file, "%d,", statisticsMemory[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < (int)found.size(); i++)
    fprintf(file, "%d,", found[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < (int)statistics.size(); i++)
    fprintf(file, "%d,", statistics[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < (int)unconnected.size(); i++)
    fprintf(file, "%d,", unconnected[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < (int)duplicated.size(); i++)
    fprintf(file, "%d,", duplicated[i]);
  fprintf(file, "),\n");

  fprintf(file, ")\n");
}
