#include "binary_graph_printer.h"

using namespace std;

BinaryGraphPrinter::BinaryGraphPrinter(FILE* file, int g, int n, int printOnlyCodim)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = printOnlyCodim;
}

BinaryGraphPrinter::BinaryGraphPrinter(FILE* file, int g, int n)
{
  this->file = file;
  this->G = g;
  this->M = n;
  this->printOnlyCodim = -1;
}

BinaryGraphPrinter::BinaryGraphPrinter(const BinaryGraphPrinter& L)
{
  file = L.file;
  G = L.G;
  M = L.M;
  printOnlyCodim = L.printOnlyCodim;
}

void
BinaryGraphPrinter::BeginPrint(void)
{
  unsigned char magic = 0;
  fwrite(&magic, sizeof(unsigned char), 1, file);
}

void
BinaryGraphPrinter::PrintSomeGraph(map< uchar, vector< Graph* > >& store)
{
  for (int i = 0; i <= 3*G-3+M; i++)
    for (map< uchar, vector< Graph* > >::iterator s = store.begin(); s != store.end(); ++s)
      for (vector< Graph* >::iterator t = s->second.begin(); t != s->second.end(); ++t)
        if ((*t)->total_edges == i)
          if (printOnlyCodim == -1 || (*t)->total_edges == printOnlyCodim)
            (*t)->PrintBinary(file);
}

void
BinaryGraphPrinter::EndPrint(void)
{
  unsigned char eof = 0;
  fwrite(&eof, sizeof(unsigned char), 1, file);
}
