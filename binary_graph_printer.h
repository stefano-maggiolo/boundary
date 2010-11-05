#ifndef BINARY_GRAPH_PRINTER_H
#define BINARY_GRAPH_PRINTER_H

/*
  Binary format specifications (for small graph, with chars instead of
  ints):

  A graph is composed of:

  Type    | B    | Repetitions   | Meaning
  uchar   | 1    | 1             | K (valid from 1)
  uchar   | 1    | K             | g (from 0 to K-1)
  uchar   | 1    | K             | m (from 0 to K-1)
  uchar   | 1    | K             | l (from 0 to K-1)
  uchar   | 1    | (K-2)*(K-1)/2 | a (row by row)
  Tot: 2 (K^2 + 3 K + 4) bytes

  A file is composed of:

  Type    | B    | Repetitions   | Meaning
  uchar   | 1    | 1             | Magic (0)
  Graph   | ...  | #             | the graphs
  uchar   | 1    | 1             | Endoffile (0)

  TODO: binary format for decorated graphs (magic=1?)
*/

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class BinaryGraphPrinter : public GraphPrinter
{
 public:
  BinaryGraphPrinter(FILE* file, int g, int n, int printOnlyCodim);
  BinaryGraphPrinter(FILE* file, int g, int n);
  BinaryGraphPrinter(const BinaryGraphPrinter& L);
  void BeginPrint(void);
  void PrintSomeGraph(map< int, vector< Graph > >& store);
  void EndPrint(void);

 private:
  FILE* file;
  int printOnlyCodim;
  int G, M;
  map< int, vector< Graph > > store;
};

#endif
