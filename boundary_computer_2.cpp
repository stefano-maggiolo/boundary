#include "boundary_computer_2.h"

using namespace std;

BoundaryComputer2::BoundaryComputer2(int g, int n)
{
  G = g;
  M = n;
  computed = false;
}

vector< Graph >&
BoundaryComputer2::GetAllResults(void)
{
  vector< Graph > *ret = new vector< Graph >();

  for (map< int, vector< Graph > >::iterator i = store.begin(); i != store.end(); ++i)
    for (vector< Graph >::iterator j = i->second.begin(); j != i->second.end(); ++j)
      ret->push_back(*j);
  return *ret;
}

map< int, vector< Graph > >&
BoundaryComputer2::GetAllResultsByCodimension(void)
{
  return store;
}

void
BoundaryComputer2::Compute(GraphPrinter &printer, enum Statistics stats, int computeOnlyCodim)
{
  statistics.assign(3*G-3+M+1, 0);
  statisticsTime.assign(2*G-2+M+1, 0);
  statisticsMemory.assign(2*G-2+M+1, 0);

  printer.BeginPrint();
  this->computeOnlyCodim = computeOnlyCodim;

  // Per ogni K
  // Per ogni possibile partizione sd di 1...K
  // Per ogni possibile multigrafo con simple_divisions = sd,
  //  e sum lati <= G+K-1, a meno di isomorfismo
  // Per ogni possibile dato g,m,l crescente
  // Aggiungi

  // Initialization.
  tried.assign(2*G-2+M+1,0);
  found.assign(2*G-2+M+1,0);
  duplicated.assign(2*G-2+M+1,0);
  unconnected.assign(2*G-2+M+1,0);

  // We do the case with one component separately to not speed down
  // the rest of the computation.

  // TODO: is it necessary?
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
      graph.a.assign(1, vector< int >(1, graph.G-n));
      graph.g.assign(1, n);
      graph.m.assign(1, graph.M);
      graph.l.assign(1, graph.G-n);
      graph.edges.assign(1, 2*(graph.G-n));
      graph.total_edges = graph.G-n;
      if (computeOnlyCodim == -1 || computeOnlyCodim == graph.total_edges)
        {
          found[1]++;
          addToStore(graph);
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
  if (stats== Full)
    fprintf(stderr, "done in %3d.%01d s, using %4d.%01d MB.\n",
            statisticsTime[graph.K] / 1000,
            statisticsTime[graph.K] / 100 % 10,
            statisticsMemory[graph.K] / 1024,
            statisticsMemory[graph.K] / 102 % 10);
#else  
  if (stats== Full)
    fprintf(stderr, "done.\n");
#endif
  
  printer.PrintSomeGraph(store);
  for (map< int, vector< Graph > >::iterator s = store.begin(); s != store.end(); ++s)
    statistics[s->first] += s->second.size();
  store.clear();
  
  // We have at most 2G-2+M components for a curve of genus G and M
  // marked points.
  for (graph.K = 2; graph.K <= 2*graph.G-2+graph.M; graph.K++)
    {
      if (computeOnlyCodim != -1 && graph.K > computeOnlyCodim + 1) break;

      if (stats== Full) fprintf(stderr, "K = %2d... ", graph.K);
#if HAVE_GETRUSAGE
      getrusage(RUSAGE_SELF, &rusageBegin);
#endif
      
      // Every time, we rebuild the adjacency matrix with the right
      // dimension.
      graph.a.assign(graph.K, vector< int >(graph.K, 0));
      // We clear divisions, but divisions[0] exists by definition.
      divisions.assign(graph.K, false);
      divisions[0] = true;
      // We rebuild the genera and the marked points vectors.
      graph.g.assign(graph.K, 0);
      graph.m.assign(graph.K, 0);
      graph.l.assign(graph.K, 0);
      graph.connections = 0;
      graph.edges.assign(graph.K, 0);
      graph.total_edges = 0;

      // And we start with the formula corresponding to no edge and
      // no genera assigned.
      graph.sum = -graph.K+1;
      // No marked points assigned, by now.
      graph.msum=0;
      // Nor genus 0 curves.
      graph.p1 = 0;
      // Nor simple divisions
      graph.simple_divisions.assign(1, 0);
      
      // Let's start.
      bt_simple_divisions(0);

      // Now we have all the possible graphs without considering g,m,l
      for (map< int, vector< Graph > >::iterator t = intermediateStore.begin();
           t != intermediateStore.end(); ++t)
        for (vector< Graph >::iterator s = t->second.begin();
             s != t->second.end();
             ++s)
          {
            fprintf(stderr, "From the following graph weobtain...\n");
            s->PrintNormal();
            bt_g(*s, 0, 0);
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
        fprintf(stderr, "done.\n");
#endif
      
      printer.PrintSomeGraph(store);
      for (map< int, vector< Graph > >::iterator s = store.begin(); s != store.end(); ++s)
        statistics[s->first] += s->second.size();
      store.clear();

      intermediateStore.clear();
    }

  printer.EndPrint();
  computed = true;

  if (stats== Full) Statistics(stderr);
  else if (stats== Terse) TerseStatistics(stderr);
}

void
BoundaryComputer2::Compute(GraphPrinter &printer, enum Statistics stat)
{
  Compute(printer, stat, -1);
}

void
BoundaryComputer2::Compute(GraphPrinter &printer)
{
  Compute(printer, Full, -1);
}

void
BoundaryComputer2::bt_simple_divisions(int last)
{
  if (last == graph.K)
    {
      // Make a fake data g,m,l just to compute isomorphism classes
      for (int i = 1; i < graph.simple_divisions.size(); ++i)
        for (int j = graph.simple_divisions[i-1]; j < graph.simple_divisions[i]; ++j)
          graph.g[j] = i-1;
      // TODO we can infer some conditions on just the partition,
      // even if we don't know the actual data g,m,l?
      bt_a(0, 1);
    }
  else
    for (int i = 1; i <= graph.K-last; ++i)
      {
        graph.simple_divisions.push_back(last+i);
        divisions[last+i] = true;
        bt_simple_divisions(last+i);
        divisions[last+i] = false;
        graph.simple_divisions.pop_back();
      }
}

void
BoundaryComputer2::bt_a(int i, int j)
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
      int start_i = divisions[i]? 0: graph.a[i-1][j];
      // Also, if j is not the first element in its non-divisions
      // range, than we can assume it is not less than the element in
      // the previous column.
      int start_j = 0;
      if (j > i+1 && !divisions[j]) start_j = graph.a[i][j-1];
      int start = max(start_i, start_j);

      // Sure a[i][j] cannot exceed G-sum; but if we put c connections
      // up to now, sure we will have to put at least other K-2-c
      // edges to connect the graph.
      int end = graph.G - graph.sum - max(0, graph.K-2-graph.connections);
      for (int n = start; n <= end; n++)
        {
          map< int, int > tmp;
          // We check the following, to ensure that the sum = G and
          // that all genus 0 curve are stabilized.
          if (j == graph.K-1)
            {
              if (n == 0 && graph.edges[i] == 0) continue;
              else
                {
                }
            }
          // Changes induced by a[i][j] = n.
          graph.a[i][j] = n;
          graph.a[j][i] = n;
          bool tmpi = divisions[i], tmpj = divisions[j];
          if (n > 0) graph.connections++;
          graph.edges[i] += n;
          graph.edges[j] += n;
          graph.total_edges += n;
          if (n > start_i) divisions[i] = true;
          if (n > start_j && j > i+1) divisions[j] = true;
          graph.sum += n;
          
          bt_a(i, j+1);

          // We go back to the previous situation.
          graph.sum -= n;
          divisions[i] = tmpi;
          divisions[j] = tmpj;
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
          // TODO: all these procedures should be in the Graph class.
#ifdef USE_LINES_NO_MAP
          graph.aSorted = graph.a;
          for (int iter = 0; iter < graph.K; ++iter)
            sort(graph.aSorted[iter].begin(), graph.aSorted[iter].end());
          for (int iter = 0; iter < graph.simple_divisions.size()-1; ++iter)
            sort(graph.aSorted.begin() + graph.simple_divisions[iter],
                 graph.aSorted.begin() + graph.simple_divisions[iter+1]);
          sort(graph.aSorted.begin() +
               graph.simple_divisions[graph.simple_divisions.size()-1],
               graph.aSorted.end());
#endif
#ifdef USE_DEGREES_NO_MAP
          graph.aSortedDiv = graph.a;
          int sdSize = graph.simple_divisions.size();
          graph.simple_divisions.push_back(graph.K);
          for (int iterI = 0; iterI < sdSize; ++iterI)
            for (int iterJ = 0; iterJ < sdSize; ++iterJ)
              {
                vector< vector< int > > tmp;
                int beginI = graph.simple_divisions[iterI];
                int endI = graph.simple_divisions[iterI+1];
                int beginJ = graph.simple_divisions[iterJ];
                int endJ = graph.simple_divisions[iterJ+1];
                for (int itI = beginI; itI < endI; ++itI)
                  {
                    vector< int > tmp2(graph.a[itI].begin()+beginJ,
                                       graph.a[itI].begin()+endJ);
                    sort(tmp2.begin(), tmp2.end());
                    tmp.push_back(tmp2);
                  }
                sort(tmp.begin(), tmp.end());
                for (int itI = beginI; itI < endI; ++itI)
                  for (int itJ = beginJ; itJ < endJ; ++itJ)
                    graph.aSortedDiv[itI][itJ] = tmp[itI-beginI][itJ-beginJ];
              }
          graph.simple_divisions.pop_back();
#endif
#ifdef START_LAPACK_COMPUTATION
          graph.ComputeEigenvalues();
#endif
#ifdef USE_NAUTY
          // Note: we could also compute all canonical labelling
          // whether or not we'll use it: for example, for (6,0) there
          // are just ~700 graphs for which the canonical labelling
          // won't be used, but they are with K = 3.
          graph.nautyK = -1;
#endif
          
          if (correct()) addToIntermediateStore();

        }
    }  
}

void
BoundaryComputer2::addToIntermediateStore(void)
{
  Graph *ng = new Graph(graph);
  intermediateStore[ng->total_edges].push_back(*ng);
}

bool
BoundaryComputer2::correct()
{
  tried[graph.K]++;
  // First condition (sum = G): already ensured.
  // Second condition (stability) already ensured.
  // Third condition (connectedness).
  v.assign(graph.K, false);
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
BoundaryComputer2::visit(int i)
{
  v[i] = true;
  int s = 1;
  for (int n = 0; n < graph.K; n++)
    if (graph.a[i][n] != 0 && v[n] == false)
      s += visit(n);
  return s;
}

bool
BoundaryComputer2::duplicate(void)
{
  for (vector< Graph >::iterator s = intermediateStore[graph.total_edges].begin();
       s != intermediateStore[graph.total_edges].end();
       s++)
    if (s->K == graph.K &&
        s->simple_divisions == graph.simple_divisions &&
        //        s->g == graph.g &&
        //        s->m == graph.m &&
        //        s->l == graph.l &&
#ifdef USE_DEGREES_NO_MAP
        s->aSortedDiv == graph.aSortedDiv &&
#endif
#ifdef USE_LINES_NO_MAP
        s->aSorted == graph.aSorted &&
#endif
        s->Equal(graph))
      return true;
  return false;
}

void
BoundaryComputer2::bt_g(Graph& g, int k, int prev)
{
  if (k+1 >= g.simple_divisions.size())
    {
      bt_l(g, 0, 0);
    }
  else
    {
      int remaining = g.K - g.simple_divisions[k];
      for (int i = prev; g.sum + remaining*i <= g.G; ++i)
        {
          g.sum += i * (g.simple_divisions[k+1]-g.simple_divisions[k]);
          for (int j = g.simple_divisions[k]; j < g.simple_divisions[k+1]; ++j)
            g.g[j] = i;
          bt_g(g, k+1, i);
          g.sum -= i * (g.simple_divisions[k+1]-g.simple_divisions[k]);
        }
    }
}

void
BoundaryComputer2::bt_l(Graph& g, int k, int prev)
{
  if (k+1 >= g.simple_divisions.size())
    {
      bt_m(g, 0, 0);
    }
  else
    {
      if (k > 0 && g.g[g.simple_divisions[k]-1] != g.g[g.simple_divisions[k]])
        prev = 0;
      int remaining = g.simple_divisions[k+1] - g.simple_divisions[k];
      if (k+2 == g.simple_divisions.size())
        {
          // Last chance to reach G in g.sum
          int a = g.K-g.simple_divisions[k];
          if ((g.G - g.sum) % a != 0) return;
          else prev = max(prev, (g.G - g.sum) / a);
        }
      for (int i = prev; g.sum + remaining*i <= g.G; ++i)
        {
          g.sum += i * remaining;
          for (int j = g.simple_divisions[k]; j < g.simple_divisions[k+1]; ++j)
            {
              g.l[j] = g.a[j][j] = i;
              g.edges[j] += 2*i;
            }
          g.total_edges += i*remaining;
          bt_l(g, k+1, i);
          g.total_edges -= i*remaining;
          for (int j = g.simple_divisions[k]; j < g.simple_divisions[k+1]; ++j)
            {
              g.edges[j] -= 2*i;
            }
          g.sum -= i * remaining;
        }
    }
}

void
BoundaryComputer2::bt_m(Graph& g, int k, int prev)
{
  if (k+1 >= g.simple_divisions.size())
    {
      addToStore(g);
    }
  else
    {
      int remaining = g.simple_divisions[k+1] - g.simple_divisions[k];
      // We MUST have a sd where we decided.
      if (k > 0 && (
          g.g[g.simple_divisions[k]-1] != g.g[g.simple_divisions[k]] ||
          g.l[g.simple_divisions[k]-1] != g.l[g.simple_divisions[k]]
                    )
          )
        prev = 0;
      // We need to stabilize.
      if (g.g[g.simple_divisions[k]] == 0 && g.l[g.simple_divisions[k]] == 0)
        for (int j = g.simple_divisions[k]; j < g.simple_divisions[k+1]; ++j)
          prev = max(prev, 3-g.edges[j]);
      if (k+2 == g.simple_divisions.size())
        {
          // Last chance to reach G in g.sum
          if ((g.M - g.msum) % remaining != 0) return;
          else prev = max(prev, (g.M - g.msum) / remaining);
        }
      for (int i = prev; g.msum + remaining*i <= g.M; ++i)
        {
          g.msum += i * remaining;
          for (int j = g.simple_divisions[k]; j < g.simple_divisions[k+1]; ++j)
            g.m[j] = i;
          bt_m(g, k+1, i+1);
          g.msum -= i * remaining;
        }
    }
}

void
BoundaryComputer2::addToStore(const Graph& g)
{
  g.PrintNormal();
  Graph *ng = new Graph(g);
  store[ng->total_edges].push_back(*ng);
}

void
BoundaryComputer2::Statistics(FILE* file)
{
  if (!computed)
    {
      fprintf(file, "Not yet computed!\n");
      return;
    }

  int ms = 0;
  for (int i = 0; i < statisticsTime.size(); i++)
    ms += statisticsTime[i];
  fprintf(file, "Total time: %d.%01d s.\n", ms/1000, ms/100%10);
  int kB = 0;
  for (int i = 0; i < statisticsTime.size(); i++)
    if (kB < statisticsMemory[i]) kB = statisticsMemory[i];
  fprintf(file, "Total memory: %d.%01d MB.\n", kB/1024, kB/102 % 10);
  
  int s = 0;
  fprintf(file, "Tried       ");
  for (int i = 0; i < tried.size(); i++)
    {
      fprintf(file, "%5d ", tried[i]);
      s += tried[i];
    }
  fprintf(file, "= %7d\n", s);
  s=0;
  fprintf(file, "Unconnected ");
  for (int i = 0; i < unconnected.size(); i++)
    {
      fprintf(file, "%5d ", unconnected[i]);
      s += unconnected[i];
    }
  fprintf(file, "= %7d\n", s);
  s=0;
  fprintf(file, "Duplicated  ");
  for (int i = 0; i < duplicated.size(); i++)
    {
      fprintf(file, "%5d ", duplicated[i]);
      s += duplicated[i];
    }
  fprintf(file, "= %7d\n", s);
  s=0;
  fprintf(file, "Found       ");
  for (int i = 0; i < found.size(); i++)
    {
      fprintf(file, "%5d ", found[i]);
      s += found[i];
    }
  fprintf(file, "= %7d\n", s);

  // Statistic subdivided by number of edges.
  for (int i = 0; i < statistics.size(); ++i)
    fprintf(file, "%3d: %5d\n", i, statistics[i]);
}

void
BoundaryComputer2::TerseStatistics(FILE* file)
{
  if (!computed)
    {
      fprintf(file, "Not yet computed!\n");
      return;
    }

  fprintf(file, "(\n");

  fprintf(file, " (");
  for (int i = 0; i < statisticsTime.size(); i++)
    fprintf(file, "%d,", statisticsTime[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < statisticsMemory.size(); i++)
    fprintf(file, "%d,", statisticsMemory[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < found.size(); i++)
    fprintf(file, "%d,", found[i]);
  fprintf(file, "),\n");

  fprintf(file, " (");
  for (int i = 0; i < statistics.size(); i++)
    fprintf(file, "%d,", statistics[i]);
  fprintf(file, "),\n");

  fprintf(file, ")\n");
}
