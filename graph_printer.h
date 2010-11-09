#ifndef GRAPH_PRINTER_H
#define GRAPH_PRINTER_H

#include "graph.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

enum Printer { LaTeX, Text, Binary, Null };

class GraphPrinter
{
 public:
  virtual void BeginPrint(void) = 0;
  virtual void PrintSomeGraph(map< uchar, vector< Graph* > >& store) = 0;
  virtual void EndPrint(void) = 0;
};

#endif
