#ifndef LATEX_GRAPH_PRINTER_H
#define LATEX_GRAPH_PRINTER_H

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class LaTeXGraphPrinter : public GraphPrinter
{
 public:
  LaTeXGraphPrinter(FILE* file, int g, int n, int printOnlyCodim);
  LaTeXGraphPrinter(FILE* file, int g, int n);
  LaTeXGraphPrinter(const LaTeXGraphPrinter& L);
  void BeginPrint(void);
  void PrintSomeGraph(map< int, vector< Graph* > >& store);
  void EndPrint(void);

 private:
  // Print LaTeX header, body (of the current graph) and footer to cout.
  void print_middle(Graph *graph, bool level_by_vertices);

  FILE* file;
  int printOnlyCodim;
  int rowLength;
  int G, M;
};

#endif
