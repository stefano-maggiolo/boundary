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
      graph.a[1][1] = graph.G-n;
      graph.g[1] = n;
      graph.m[1] = graph.M;
      graph.l[1] = graph.G-n;
      graph.edges[1] =  2*(graph.G-n);
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
  if (i < graph.K) // We have to assign g[i].
    {
      // We impose the vector g to be non-decreasing, to avoid
      // generating isomorphic graphs.
      int start = 0;
      if (i > 0) start = graph.g[i-1];

      // Since we want G = sum + SUM_{j>=i}g[j] + edges, we have
      // 2edges+marked>=3*p1 (to stabilize the genus 0 curves), edges>=K-1
      // (to connect the graph) and g[j]>=g[i], then we get the
      // following.
      int end = (graph.G - graph.sum - max((3*graph.p1-graph.M+1)/2, graph.K-1)) / (graph.K-i);
      for (int n = start; n <= end; n++)
        {
          // We do the changes induced by g[i] = n.
          graph.g[i] = n;
          if (n == 0) graph.p1++;
          graph.sum += n;
          // If we are increasing the genus with respect to the
          // previous one, we cannote exchange anymore components < i
          // with components >= i, so we put a division.
          if (n > start) graph.divisions[i] = true;

          bt_g(i+1);

          // We go back to the previous situation
          graph.sum -= n;
          if (n == 0) graph.p1--;
          graph.divisions[i] = false;
        }
    }
  else // If we decided all the genera, we go to assign values to the marked points
    bt_m(0);
}

void
BoundaryComputer::bt_m(int i)
{
  if (i < graph.K) // We have to assign m[i].
	{
	// We impose the vector m to be non-decreasing for i's
	// such that the genus is equal, to avoid generating isomorphic graphs.
      int start = graph.divisions[i]? 0: graph.m[i-1];
	// m[i] is always smaller or equal than M-msum.
      int end = graph.M-graph.msum;
      for (int n = start; n <= end; n++)
		{
		  if (i == graph.K-1 && n != end) continue;

		  graph.m[i] = n;
		  bool tmp = graph.divisions[i];
		  if (n > start) graph.divisions[i] = true;
		  graph.msum += n;

		  bt_m(i+1);

		  graph.msum -= n;
		  graph.divisions[i] = tmp;
		}

	}
  else  // If we decided all the marked points, we go to assign values
        // to the diagonal of the adjacency matrix.
    bt_l(0);
}

void
BoundaryComputer::bt_l(int i)
{
  if (i < graph.K) // We have to assign l[i].
    {
      // We may permute i with the elements inside all the range
      // around i that does not meet divisions. In particular, if i is
      // not the first element after a division, we may assume to have
      // permuted the components in such a way that they are
      // non-decreasing.
      int start = graph.divisions[i]? 0: graph.l[i-1];

      // Sure l[i] cannot exceed G-sum; moreover we will have to put
      // at least other K-2 edges to connect the graph.
      int end = graph.G - graph.sum - (graph.K - 1);
      for (int n = start; n <= end; n++)
        {
          // We do the changes induced by l[i] = n.
          graph.l[i] = n;
          graph.a[i][i] = n;
          bool tmp = graph.divisions[i];
          if (n > start) graph.divisions[i] = true;
          graph.edges[i] += 2*n;
          graph.total_edges += n;
          graph.sum += n;

          bt_l(i+1);

          // We go back to the previous situation.
          graph.sum -= n;
          graph.total_edges -= n;
          graph.edges[i] -= 2*n;
          graph.divisions[i] = tmp;
        }
    }
  else // If we decided all diagonal elements, we go to assign values
       // to the rest of the upper triangle of the matrix, row by row,
       // from left to right.
    {
      memset(graph.gDegrees, 0, sizeof(uchar)*(graph.G+2));
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

      // If i is not the first element in its non-divisions range,
      // then we can assume it is not less then the element in the
      // previous row.
      int start_i = graph.divisions[i]? 0: graph.a[i-1][j];
      // Also, if j is not the first element in its non-divisions
      // range, than we can assume it is not less than the element in
      // the previous column.
      int start_j = 0;
      if (j > i+1 && !graph.divisions[j]) start_j = graph.a[i][j-1];
      int start = max(start_i, start_j);

      // Sure a[i][j] cannot exceed G-sum; but if we put c connections
      // up to now, sure we will have to put at least other K-2-c
      // edges to connect the graph.
      int end = graph.G - graph.sum - max(0, graph.K-2-graph.connections);
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
          bool tmpi = graph.divisions[i], tmpj = graph.divisions[j];
          if (n > 0) graph.connections++;
          graph.edges[i] += n;
          graph.edges[j] += n;
          graph.total_edges += n;
          if (n > start_i) graph.divisions[i] = true;
          if (n > start_j && j > i+1) graph.divisions[j] = true;
          graph.sum += n;
          graph.gDegrees[n]++;

          bt_a(i, j+1);

          // We go back to the previous situation.
          graph.gDegrees[n]--;
          graph.sum -= n;
          graph.divisions[i] = tmpi;
          graph.divisions[j] = tmpj;
          graph.edges[i] -= n;
          graph.edges[j] -= n;
          graph.total_edges -= n;
          if (n > 0) graph.connections--;
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
    if (graph.a[i][n] != 0 && v[n] == false)
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
