#ifndef COMPUTER_H
#define COMPUTER_H

#include "graph.h"
#include "graph_printer.h"

#include <map>
#include <vector>
#include <cstdio>

enum Statistics { Full, Terse, No };

using namespace std;

class Computer
{
 public:
  virtual vector< Graph >& GetAllResults(void) = 0;
  virtual map< int, vector< Graph > >& GetAllResultsByCodimension(void) = 0;
  virtual void Compute(GraphPrinter &printer, enum Statistics stat, int computeOnlyCodim) = 0;
  virtual void Compute(GraphPrinter &printer, enum Statistics stat) = 0;
  virtual void Compute(GraphPrinter &printer) = 0;
  virtual void Statistics(FILE* file) = 0;
  virtual void TerseStatistics(FILE* file) = 0;
};

#endif

