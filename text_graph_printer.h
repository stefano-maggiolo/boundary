#ifndef TEXT_GRAPH_PRINTER_H
#define TEXT_GRAPH_PRINTER_H

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class TextGraphPrinter : public GraphPrinter
{
 public:
  TextGraphPrinter(FILE* file, int g, int n, int printOnlyCodim);
  TextGraphPrinter(FILE* file, int g, int n);
  TextGraphPrinter(const TextGraphPrinter& L);
  void BeginPrint(void);
  void PrintSomeGraph(map< int, vector< Graph* > >& store);
  void EndPrint(void);

 private:
  FILE* file;
  int printOnlyCodim;
  int G, M;
};

#endif
