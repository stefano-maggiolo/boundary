#include "binary_graph_reader.h"

using namespace std;

BinaryGraphReader::BinaryGraphReader(FILE* f)
  :
  file(f)  
{
}

BinaryGraphReader::BinaryGraphReader(const char* filename)
{
  file = fopen(filename, "rb");
}

BinaryGraphReader::BinaryGraphReader(const BinaryGraphReader& R)
  :
  file(R.file),
  store(R.store)
{
}

map< int, vector< Graph > >&
BinaryGraphReader::Read(void)
{
  unsigned char tmp;
  store.clear();
  assert(fread(&tmp, sizeof(unsigned char), 1, file) == 1);
  if (tmp != 0) return store;
  Graph* g;
  g = new Graph(file);
  while (g->K > 0)
    {
      g = new Graph(file);
      store[g->K].push_back(*g);
    }
  fclose(file);
  return store;
}

