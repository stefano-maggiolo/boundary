#ifndef NULL_GRAPH_PRINTER_H
#define NULL_GRAPH_PRINTER_H

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class NullGraphPrinter : public GraphPrinter
{
 public:
  NullGraphPrinter(FILE* file, int g, int n, int printOnlyCodim) {}
  NullGraphPrinter(FILE* file, int g, int n) {}
  NullGraphPrinter(const NullGraphPrinter& L) {}
  void BeginPrint(void) {}
  void PrintSomeGraph(map< int, vector< Graph > >& store) {}
  void EndPrint(void) {}
};

#endif
