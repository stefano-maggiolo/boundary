#ifndef BINARY_GRAPH_READER_H
#define BINARY_GRAPH_READER_H

#include "graph.h"

#include <map>
#include <vector>
#include <cstdio>

using namespace std;

class BinaryGraphReader
{
 public:
  BinaryGraphReader(const BinaryGraphReader& R);
  BinaryGraphReader(const char* filename);
  BinaryGraphReader(FILE* file);
  map< int, vector< Graph > >& Read(void);

 private:
  FILE* file;
  map< int, vector< Graph > > store;
};

#endif
